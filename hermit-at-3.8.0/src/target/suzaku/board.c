#include <autoconf.h>
#include <arch/cpu.h>
#include <platform.h>
#include <io.h>
#include <string.h>
#include <flash.h>
#include <map.h>
#include <led.h>
#include "board.h"

char target_name[256];
char *target_profile = target_name;

#define FLASH_ADDR(offset) (FLASH_START + (offset))
#define RAM_ADDR(offset) (DRAM_START + (offset))

static void update_target_profile(void)
{
	char *ptr = target_name;

	ptr += hsprintf(ptr, "%s", CONFIG_PLATFORM_NAME);
	if (strlen(CONFIG_PROFILE_NAME))
		hsprintf(ptr, "/%s", CONFIG_PROFILE_NAME);
}

static int suzaku_is_autoboot(struct platform_info *pinfo)
{
	return read32(SYSREG) & SYSREG_AUTOBOOT;
}

static void suzaku_led_on(struct platform_info *pinfo, u32 leds)
{
	if (leds & LED_RED)
		write32(XPAR_OPB_GPIO_LED_BASEADDR, 0x0);
}
static void suzaku_led_off(struct platform_info *pinfo, u32 leds)
{
	if (leds & LED_RED)
		write32(XPAR_OPB_GPIO_LED_BASEADDR, 0x1);
}

static int is_flash_spi(void)
{
	return read32(SYSREG) & SYSREG_FLASH_SPI;
}

static int is_compatible_mode(void)
{
	return read32(SYSREG) & SYSREG_COMPAT;
}

static int is_flash_8MiB(void)
{
	return ((flash_get_size(FLASH_START) == FLASH_8MiB) &&
		(!is_compatible_mode()));
}

static void suzaku_init_flash(struct platform_info *pinfo)
{
	u32 size_8mb;

	char flash_spi[] = { "0x800000(all)ro,"
			     "0x100000@0(fpga),"
			     "0x20000(bootloader)ro,"
			     "0x6d0000(image),"
			     "0x300000@0x120000(kernel),"
			     "0x3d0000(user),"
			     "-(config)" };
	char flash_8mb[] = { "0x800000(all)ro,"
			     "0x80000@0(free),"
			     "0x80000(fpga),"
			     "0x20000(bootloader)ro,"
			     "0x6d0000(image),"
			     "0x300000@0x120000(kernel),"
			     "0x3d0000(user),"
			     "-(config)" };
	char flash_4mb[] = { "0x400000(all)ro,"
			     "0x80000@0(fpga),"
			     "0x20000(bootloader)ro,"
			     "0x350000(image),"
			     "0x170000@0x100000(kernel),"
			     "0x1e0000(user),"
			     "-(config)" };
	char *map;

	if (is_flash_spi()) {
		/* SPI 8MB */
		flash_initialize(FLASH_TYPE_SPI, FLASH_START);
		map = flash_spi;
	} else {
		flash_initialize(FLASH_TYPE_AMD, FLASH_START);
		size_8mb = is_flash_8MiB();
		if (size_8mb) {
			/* CFI 8MB */
			map = flash_8mb;
		} else {
			/* CFI 4MB */
			pinfo->map->param.base = FLASH_ADDR(0x00090000);
			map = flash_4mb;
		}
	}

	hsprintf(pinfo->default_mtdparts, "suzaku-nor:%s", map);
}

static void suzaku_init_map(struct platform_info *pinfo)
{
	hsprintf(pinfo->default_ramparts, "%p(dram-1)", DRAM_SIZE);

	switch (DRAM_SIZE) {
	case 0x01000000: /* 16MB */
		pinfo->map->initrd.size = 0x00a00000;
		pinfo->map->free.base = RAM_ADDR(0x00800000);
		pinfo->map->free.size = 0x00600000;
		break;
	case 0x02000000: /* 32MB */
	default:
		break;
	}
}

static void suzaku_init_cpu(struct platform_info *pinfo)
{
#if defined(CONFIG_ARCH_MICROBLAZE)
	if (is_flash_spi()) {
		/* SZ130 */
		cpu_info.icache_size = 8192;
		cpu_info.icache_line = 16;
		cpu_info.dcache_size = 8192;
		cpu_info.dcache_line = 16;
	} else {
		/* SZ010, SZ030 */
		cpu_info.icache_size = 8192;
		cpu_info.icache_line = 4;
		cpu_info.dcache_size = 0;
		cpu_info.dcache_line = 0;
	}
#endif
#if defined(CONFIG_ARCH_PPC)
	if (is_flash_spi()) {
		/* SZ410 */
		lp.memsize = 1024 * 1024 * 64;		/*  64     MiB */
		lp.intfreq = 100000000 * 7 / 2;		/* 350.0000MHz */
		lp.busfreq = 100000000 * 7 / 8;		/*  87.5000MHz */
	} else {
		/* SZ310 */
		lp.memsize = 1024 * 1024 * 32;		/*  32     MiB */
		lp.intfreq = 3686400 * 18 * 4;		/* 265.4208MHz */
		lp.busfreq = 3686400 * 18 * 4 / 4;	/*  66.3552MHz */
	}
#endif
}

static struct memory_map suzaku_memory_map = {
	.flash		= { FLASH_ADDR(0x00000000), FLASH_SIZE },
	.param		= { FLASH_ADDR(0x00110000), 0x00010000 },

	.ram		= { RAM_ADDR(0x00000000), DRAM_SIZE },
	.kernel		= { RAM_ADDR(0x00000000), 0x00400000 },
	.initrd		= { RAM_ADDR(0x00400000), 0x00c00000 },
	.free		= { RAM_ADDR(0x01000000), 0x00e00000 },

	.gunzip		= { DRAM_END - 0x00120000, 0x00020000 },
/*      .hermit		= { DRAM_END - 0x00100000, 0x00080000 }, */
/*      .stack		= { DRAM_END - 0x00080000, 0x00080000 }, */
};

static void suzaku_init(struct platform_info *pinfo)
{
	update_target_profile();

	suzaku_init_flash(pinfo);
	suzaku_init_map(pinfo);
	suzaku_init_cpu(pinfo);
}

struct platform_info platform_info = {
	.init = suzaku_init,
	.is_autoboot = suzaku_is_autoboot,
	.led_on = suzaku_led_on,
	.led_off = suzaku_led_off,

	.map = &suzaku_memory_map,
};
