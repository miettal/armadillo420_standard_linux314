#include <autoconf.h>
#include <platform.h>
#include <machine.h>
#include <arch/ioregs.h>
#include <io.h>
#include <string.h>
#include <flash.h>
#include <i2c_ep93xx_gpio.h>
#include <mac.h>
#include <map.h>
#include <led.h>
#include <medium.h>
#include <net/net.h>
#include <net/ep93xx.h>
#include "board.h"
#include "mm.h"

char target_name[256];
char *target_profile = target_name;

static struct board_private board_private_data;

#define FLASH_ADDR(offset) (FLASH_START + (offset))
#define RAM_ADDR(offset) (DRAM_START + (offset))

static void update_target_profile(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;
	char *ptr = target_name;
	char *board;

	switch (priv->board_id) {
	case BOARD_ID_A210:  board = "210"; break;
	case BOARD_ID_A220:  board = "220"; break;
	case BOARD_ID_A230:  board = "230"; break;
	case BOARD_ID_A240:  board = "240"; break;
	case BOARD_ID_A210C: board = "210C"; break;
	default:             board = "2x0(Unknown)"; break;
	}

	ptr += hsprintf(ptr, "armadillo-%s", board);
	if (strlen(CONFIG_PROFILE_NAME))
		hsprintf(ptr, "/%s", CONFIG_PROFILE_NAME);
}

static void armadillo2x0_get_mac_address(struct platform_info *pinfo, u8 *addr)
{
	ep93xx_get_mac(addr);
}

static void armadillo2x0_udelay(struct platform_info *pinfo, u32 usecs)
{
	write32(TIMER1CONTROL, 0x48);
	write32(TIMER1LOAD, (usecs >> 1) + ((usecs & 1) ? 1 : 0));
	write32(TIMER1CONTROL, 0xc8);

	while (read32(TIMER1VALUE));
}

static void armadillo2x0_led(struct platform_info *pinfo, u32 leds, int on)
{
	struct board_private *priv = pinfo->private_data;
	struct ep93xx_gpio {
		addr_t ddr;
		addr_t dr;
		u32 mask;
	} *gr, *rd, led[] = {
		[0] = { /* A210: Green */
			.ddr = GPIO_PEDDR, .dr = GPIO_PEDR, .mask = 0x02,
		},
		[1] = { /* A210: Red, Others: Green */
			.ddr = GPIO_PEDDR, .dr = GPIO_PEDR, .mask = 0x01,
		},
		[2] = { /* Others: Red */
			.ddr = GPIO_PCDDR, .dr = GPIO_PCDR, .mask = 0x80,
		},
	};
	u32 val;

	switch (priv->board_id) {
	case BOARD_ID_A210:
		gr = &led[0];
		rd = &led[1];
		break;
	case BOARD_ID_A220:
	case BOARD_ID_A230:
	case BOARD_ID_A240:
	case BOARD_ID_A210C:
	default:
		gr = &led[1];
		rd = &led[2];
		break;
	}

	if (leds & LED_RED) {
		val = read32(rd->dr) & ~rd->mask;
		if (on)
			val |= rd->mask;
		write32(rd->dr, val);
		val = read32(rd->ddr) | rd->mask;
		write32(rd->ddr, val);
	}
	if (leds & LED_GREEN) {
		val = read32(gr->dr) & ~gr->mask;
		if (on)
			val |= gr->mask;
		write32(gr->dr, val);
		val = read32(gr->ddr) | gr->mask;
		write32(gr->ddr, val);
	}
}

static void armadillo2x0_led_on(struct platform_info *pinfo, u32 leds)
{
	armadillo2x0_led(pinfo, leds, 1);
}
static void armadillo2x0_led_off(struct platform_info *pinfo, u32 leds)
{
	armadillo2x0_led(pinfo, leds, 0);
}

static int armadillo2x0_is_autoboot(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;
	int autoboot = priv->jumper;
#if defined(CONFIG_AUTOBOOT_CANCEL_BY_TACTSW)
	autoboot &= priv->tactsw;
#endif
	return autoboot;
}

static void armadillo2x0_adjust_boot_args(struct platform_info *pinfo,
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

static void
armadillo2x0_set_board_private(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;
	u32 val;

	memzero(priv, sizeof(struct board_private));

	val = read32(GPIO_PBDDR);
	write32(GPIO_PBDDR, val & ~0x80);
	val = read32(GPIO_PHDDR);
	write32(GPIO_PHDDR, val & ~0x3c);
	val = read32(GPIO_PFDDR);
	write32(GPIO_PFDDR, val & ~0x80);

	priv->board_id = (read32(GPIO_PHDR) >> 2) & 0x7;
	priv->dram_id = (read32(GPIO_PHDR) >> 5) & 0x1;
	priv->jumper = (read32(GPIO_PBDR) >> 7) & 0x1;
	priv->tactsw = (read32(GPIO_PFDR) >> 7) & 0x1;
}

static void armadillo2x0_display_info(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;

	hprintf("Board ID: %p\n", priv->board_id);
	hprintf("DRAM ID: %p\n", priv->dram_id);
}

static void
armadillo2x0_init_flash(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;
	char *region_all = "-@0(all)ro";
	char *region_bootloader = "0x10000(bootloader)ro";
	char *region_kernel = "0x170000(kernel)";
	char *region_userland;
	char *region_userland_4m = "0x270000(userland)";
	char *region_userland_8m = "0x670000(userland)";
	char *region_config = "0x10000(config)";
	int val;
	char map_char;

	if (priv->board_id == BOARD_ID_A210 ||
	    priv->board_id == BOARD_ID_A210C)
		map_char = '1';
	else
		map_char = 'x';

	flash_initialize(FLASH_TYPE_AMD, FLASH_START);

	val = flash_get_size(FLASH_START);
	switch (val) {
	case 23: /* 8MB */
		region_userland = region_userland_8m;
		break;
	case 22: /* 4MB */
	default:
		region_userland = region_userland_4m;
		break;
	}
	pinfo->map->flash.size = 1 << val;
	hsprintf(pinfo->default_mtdparts, "armadillo2%c0-nor:%s,%s,%s,%s,%s",
		 map_char, region_bootloader, region_kernel,
		 region_userland, region_config, region_all);
}

static void
armadillo2x0_init_map(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;
	char *dram1;
	char *dram1_s = "0x800000(dram-1)";
	char *dram1_m = "0x2000000(dram-1)";
	char *dram2;
	char *dram2_s = "0x800000@0x1000000(dram-2)";
	char *dram2_m = "0x2000000@0x4000000(dram-2)";
	char *dram3;
	char *dram3_s = "0x1000000@0x4800000(dram-3)";
	size_t size;
	if (priv->dram_id == DRAM_ID_MULTI) {
		/* multi: 32MB@16bits x2 */
		dram1 = dram1_m;
		dram2 = dram2_m;
		dram3 = NULL;
		size = 0x4000000;
	} else {
		/* single: 32MB@16bits x1 */
		dram1 = dram1_s;
		dram2 = dram2_s;
		dram3 = dram3_s;
		size = 0x2000000;
	}
	pinfo->map->ram.size = size;
	hsprintf(pinfo->default_ramparts, "%s,%s%s%s",
		 dram1, dram2, (dram3) ? "," : "", dram3);
}

static void armadillo2x0_init_console(struct platform_info *pinfo)
{
	change_console(CONFIG_DEFAULT_CONSOLE);
}

static void armadillo2x0_init(struct platform_info *pinfo)
{
	armadillo2x0_set_board_private(pinfo);

	update_target_profile(pinfo);

	armadillo2x0_init_flash(pinfo);
	armadillo2x0_init_map(pinfo);
	armadillo2x0_init_console(pinfo);
}

static struct memory_map armadillo2x0_memory_map = {
	.flash		= { FLASH_ADDR(0x00000000), 0 },
	.param		= { FLASH_ADDR(0x0000e000), 0x00002000 },

	.ram		= { RAM_ADDR(0x00000000), 0 },
	.boot_param	= { RAM_ADDR(0x00000100), 0x00000f00 },
	.mmu_table	= { RAM_ADDR(0x00014000), 0x00004000 },
	.kernel		= { RAM_ADDR(0x00018000), 0x006e8000 },
	.free		= { RAM_ADDR(0x01000000), 0x00800000 },
	.initrd		= { RAM_ADDR(0x04800000), 0x00d00000 },
	.gunzip		= { RAM_ADDR(0x05500000), 0x00020000 },
/*	.hermit		= { RAM_ADDR(0x05600000), 0x00100000 }, */
/*	.stack		= { RAM_ADDR(0x05700000), 0x00100000 }, */
};

struct platform_info platform_info = {
	.init = armadillo2x0_init,
	.udelay = armadillo2x0_udelay,
	.is_autoboot = armadillo2x0_is_autoboot,
	.led_on = armadillo2x0_led_on,
	.led_off = armadillo2x0_led_off,
	.get_mmu_page_tables = armadillo2x0_get_mmu_page_tables,
	.display_info = armadillo2x0_display_info,
	.get_mac_address = armadillo2x0_get_mac_address,
	.adjust_boot_args = armadillo2x0_adjust_boot_args,

	.private_data = &board_private_data,
	.map = &armadillo2x0_memory_map,
	.machine_nr = MACH_ARMADILLO9,

	.net_dev = &ep93xx_eth,
};
