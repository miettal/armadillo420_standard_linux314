#include <autoconf.h>
#include <platform.h>
#include <machine.h>
#include <flash.h>
#include <io.h>
#include <string.h>
#include <map.h>
#include <medium.h>
#include <arch/ioregs.h>
#include <net/net.h>
#include <net/ns9750.h>
#include <i2c_ns9750.h>
#include <led.h>
#include "board.h"
#include "mm.h"
#include "ide_a3x0.h"

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

static void armadillo3x0_get_mac_address(struct platform_info *pinfo, u8 *addr)
{
	ns9750_get_mac(addr);
}

static void armadillo3x0_udelay(struct platform_info *pinfo, u32 usecs)
{
	write32(NS9750_SCM_PHYS_BASE + NS_SCM_TMR0_CTRL, 0);
	write32(NS9750_SCM_PHYS_BASE + NS_SCM_TMR0_RELOAD_COUNT, usecs * 50);
	write32(NS9750_SCM_PHYS_BASE + NS_SCM_TMR0_CTRL, 0x8086);

	while (read32(NS9750_SCM_PHYS_BASE + NS_SCM_TMR0_READ));

	write32(NS9750_SCM_PHYS_BASE + NS_SCM_TMR0_CTRL, 0);
}

static int armadillo3x0_is_autoboot(struct platform_info *pinfo)
{
	u32 val = read32(NS9750_BBU_PHYS_BASE + NS_BBU_GPIO_STATUS_1);
	return val & 0x10000 ? 1 : 0;
}

static void armadillo3x0_led(struct platform_info *pinfo, u32 leds, int on)
{
	u32 val;

	if (leds & LED_RED) {
		val = read8(A3X0_EXT_PHYS_BASE + EXT_MISC_GCR) & ~0x08;
		write8(A3X0_EXT_PHYS_BASE + EXT_MISC_GCR,
		       val | (on ? 0x08 : 0x00));
	}
}

static void armadillo3x0_led_on(struct platform_info *pinfo, u32 leds)
{
	armadillo3x0_led(pinfo, leds, 1);
}
static void armadillo3x0_led_off(struct platform_info *pinfo, u32 leds)
{
	armadillo3x0_led(pinfo, leds, 0);
}

static void armadillo3x0_display_info(struct platform_info *pinfo)
{
	hprintf("ipl   : %x\n", read32(FLASH_START + 0x10));
}

static void armadillo3x0_adjust_boot_args(struct platform_info *pinfo,
					int *argc, char *argv[])
{
	static char console[128];
	int have_console = 0;
	int i;

	for (i=1; i<(*argc); i++) {
		if (!have_console && !strncmp(argv[i], "console=", 8))
			have_console = 1;
	}

	if (!have_console) {
		hsprintf(console, "console=%s", CONFIG_DEFAULT_CONSOLE);
		argv[(*argc)++] = console;
	}
}

static int is_spi_bootmode(void)
{
	if (read32(NS9750_SCM_PHYS_BASE + NS_SCM_MISC_SYS_CONFIG) & 0x0800)
		/* External-ROM boot mode: reset_done = 1 */
		return 0;
	/* SPI EEPROM boot mode: reset_done = 0 */
	return 1;
}

static flash_protect flash_protects[] = {
	{ FLASH_ADDR(0x00000000), 0x00002000 },
	{ 0, 0 },
};

static void armadillo3x0_init_flash(struct platform_info *pinfo)
{
	flash_initialize(FLASH_TYPE_AMD, FLASH_START);

	if (is_spi_bootmode()) {
		flash_protects[0].start = 0;
		flash_protects[0].size = 0;
	}
	flash_register_protect_table(flash_protects);

	hsprintf(pinfo->default_mtdparts,
		 "armadillo3x0-nor:0x800000(all)ro,"
		 "0x2000@0(ipl)ro,"
		 "0xe000(bootloader)ro,"
		 "0x200000(kernel),"
		 "0x5e0000(userland),"
		 "-(config)");
}

static void armadillo3x0_init_map(struct platform_info *pinfo)
{
	hsprintf(pinfo->default_ramparts,
		 "0x4000000(dram-1)");
}

static void armadillo3x0_init(struct platform_info *pinfo)
{
	update_target_profile();

	armadillo3x0_init_flash(pinfo);
	armadillo3x0_init_map(pinfo);
	change_console(CONFIG_DEFAULT_CONSOLE);
}

static struct memory_map armadillo3x0_memory_map = {
	.flash		= { FLASH_ADDR(0x00000000), FLASH_SIZE },
	.param		= { FLASH_ADDR(0x0000e000), 0x00002000 },

	.ram		= { RAM_ADDR(0x00000000), DRAM_SIZE },
	.boot_param	= { RAM_ADDR(0x00000100), 0x00000f00 },
	.mmu_table	= { RAM_ADDR(0x00024000), 0x00004000 },
	.kernel		= { RAM_ADDR(0x00028000), 0x007d8000 },
	.initrd		= { RAM_ADDR(0x00800000), 0x01800000 },
	.free		= { RAM_ADDR(0x02000000), 0x00f00000 },
	.gunzip		= { RAM_ADDR(0x02f00000), 0x00020000 },
/*      .hermit		= { RAM_ADDR(0x03000000), 0x00f00000 }, */
/*      .stack		= { RAM_ADDR(0x03f00000), 0x00100000 }, */
};

struct platform_info platform_info = {
	.init = armadillo3x0_init,
	.udelay = armadillo3x0_udelay,
	.is_autoboot = armadillo3x0_is_autoboot,
	.led_on = armadillo3x0_led_on,
	.led_off = armadillo3x0_led_off,
	.get_mmu_page_tables = armadillo3x0_get_mmu_page_tables,
	.get_mac_address = armadillo3x0_get_mac_address,
	.display_info = armadillo3x0_display_info,
	.adjust_boot_args = armadillo3x0_adjust_boot_args,

	.map = &armadillo3x0_memory_map,
	.machine_nr = MACH_ARMADILLO300,

	.net_dev = &ns9750_eth,
	.ide_dev = &armadillo3x0_ide,
};
