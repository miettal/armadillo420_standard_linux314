#include <autoconf.h>
#include <platform.h>
#include <machine.h>
#include <arch/ioregs.h>
#include <io.h>
#include <string.h>
#include <map.h>
#include <medium.h>
#include <flash.h>
#include <i2c_ep93xx_gpio.h>
#include <net/net.h>
#include <net/ep93xx.h>
#include "board.h"
#include "mm.h"

char target_name[256];
char *target_profile = target_name;

static void update_target_profile(void)
{
	char *ptr = target_name;

	ptr += hsprintf(ptr, "%s", CONFIG_PLATFORM_NAME);
	if (strlen(CONFIG_PROFILE_NAME))
		hsprintf(ptr, "/%s", CONFIG_PROFILE_NAME);
}

static void armadillo9_get_mac_address(struct platform_info *pinfo, u8 *addr)
{
	ep93xx_get_mac(addr);
}

static void armadillo9_udelay(struct platform_info *pinfo, u32 usecs)
{
	write32(TIMER1CONTROL, 0x48);
	write32(TIMER1LOAD, (usecs >> 1) + ((usecs & 1) ? 1 : 0));
	write32(TIMER1CONTROL, 0xc8);

	while (read32(TIMER1VALUE));
}

static int armadillo9_is_autoboot(struct platform_info *pinfo)
{
	return !!(read32(GPIO_PBDR) & PBDR_JP2);
}

static void armadillo9_adjust_boot_args(struct platform_info *pinfo,
					int *argc, char *argv[])
{
	static char mtdparts[CMDLINE_MAXLEN];
	static char console[128];
	int have_console = 0, have_mtdparts = 0;
	int i;

	for (i=1; i<(*argc); i++) {
		if (!have_console && !strncmp(argv[i], "console=", 8))
			have_console = 1;
		if (!have_mtdparts && !strncmp(argv[i], "mtdparts=", 9))
			have_mtdparts = 1;
	}

	if (!have_console) {
		hsprintf(console, "console=%s", CONFIG_DEFAULT_CONSOLE);
		argv[(*argc)++] = console;
	}

	if (!have_mtdparts) {
		hsprintf(mtdparts, "mtdparts=%s", get_current_flash_map());
		argv[(*argc)++] = mtdparts;
	}
}

static void armadillo9_init_flash(struct platform_info *pinfo)
{
	flash_initialize(FLASH_TYPE_AMD, FLASH_START);

	hsprintf(pinfo->default_mtdparts,
		 "armadillo9-nor:"
		 "0x10000(bootloader)ro,"
		 "0x170000(kernel),"
		 "0x670000(userland),"
		 "0x10000(config),"
		 "-@0(all)ro");
}

static void armadillo9_init_map(struct platform_info *pinfo)
{
	hsprintf(pinfo->default_ramparts,
		 "0x2000000(dram-1),"
		 "0x2000000@0x4000000(dram-2)");
}

#define FLASH_ADDR(offset) (FLASH_START + (offset))
#define RAM_ADDR(offset) (DRAM_START + (offset))

static struct memory_map armadillo9_memory_map = {
	.flash		= { FLASH_ADDR(0x00000000), FLASH_SIZE },
	.param		= { FLASH_ADDR(0x0000e000), 0x00002000 },

	.ram		= { RAM_ADDR(0x00000000), 0x04000000 },
	.boot_param	= { RAM_ADDR(0x00000100), 0x00000f00 },
	.mmu_table	= { RAM_ADDR(0x00014000), 0x00004000 },
	.kernel		= { RAM_ADDR(0x00018000), 0x007e8000 },
	.free		= { RAM_ADDR(0x00800000), 0x01800000 },
	.initrd		= { RAM_ADDR(0x04000000), 0x01000000 },

	.gunzip		= { RAM_ADDR(0x05500000), 0x00020000 },
/*      .hermit		= { RAM_ADDR(0x05600000), 0x00100000 }, */
/*      .stack		= { RAM_ADDR(0x05700000), 0x00100000 }, */
};

static void armadillo9_init(struct platform_info *pinfo)
{
	update_target_profile();

	armadillo9_init_flash(pinfo);
	armadillo9_init_map(pinfo);
	change_console(CONFIG_DEFAULT_CONSOLE);
}

extern struct ide_device armadillo9_ep93xx_ide;
struct platform_info platform_info = {
	.init = armadillo9_init,
	.udelay = armadillo9_udelay,
	.is_autoboot = armadillo9_is_autoboot,
	.get_mmu_page_tables = armadillo9_get_mmu_page_tables,
	.get_mac_address = armadillo9_get_mac_address,
	.adjust_boot_args = armadillo9_adjust_boot_args,

	.map = &armadillo9_memory_map,
	.machine_nr = MACH_ARMADILLO9,

	.net_dev = &ep93xx_eth,
	.ide_dev = &armadillo9_ep93xx_ide,
};
