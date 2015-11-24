#include <autoconf.h>
#include <hermit.h>
#include <herrno.h>
#include <init.h>
#include <platform.h>
#include <machine.h>
#include <arch/system.h>
#include <arch/pinmux.h>
#include <htypes.h>
#include <io.h>
#include <string.h>
#include <memdev.h>
#include <param.h>
#include <map.h>
#include <boost.h>
#include <flash.h>
#include <medium.h>
#include <led.h>
#include <i2c.h>
#include <i2c_rma1.h>
#include <net/eth_util.h>
#include <net/rma1.h>
#include <rma1_gpio.h>
#include <rma1_sdhc.h>
#include <rma1_wdt.h>
#include <rma1_irq.h>
#include "board.h"
#include "pinmux.h"

char target_name[256];
char *target_profile = target_name;

static struct board_private board_priv;
static struct memory_device mdev_param;

#define FLASH_ADDR(offset) (FLASH_START + (offset))
#define RAM_ADDR(offset) (DRAM_START + (offset))

static char *get_boot_mode_str(u8 mode)
{
	switch (mode) {
	case BOOT_MODE_NOR:	/* MD(2, 3, 4, 6, 7) = 0, 0, 0, 1, 0 */
		return "NOR";
	case BOOT_MODE_SD:	/* MD(2, 3, 4, 6, 7) = 0, 1, 0, 1, 0 */
		return "SD";
	case BOOT_MODE_EMMC:	/* MD(2, 3, 4, 6, 7) = 1, 1, 0, 1, 0 */
		return "eMMC";
	default:
		return "UNKNOWN";
	}
}

static void armadillo8x0_get_mac_address(struct platform_info *pinfo, u8 *addr)
{
	struct board_private *priv = pinfo->private_data;
	u8 *mac;

	if (is_valid_mac(priv->umac1))
		mac = priv->umac1;
	else
		mac = priv->mac1;

	memcpy(addr, mac, 6);
}

static void armadillo8x0_udelay(struct platform_info *pinfo, u32 usecs)
{
	u32 count;
	u8 tstr;

	/* SUB = 50MHz, TCNT_CLK = 12.5MHz */
	count = (12 * usecs) + (usecs / 2) + (usecs & 1);

	write16(TMU(0, TCR0), 0);
	write32(TMU(0, TCOR0), count);
	write32(TMU(0, TCNT0), count);

	tstr = read8(TMU(0, TSTR));
	write8(TMU(0, TSTR), tstr | 0x1);
	while (!(read16(TMU(0, TCR0)) & 0x100));
	write16(TMU(0, TCR0), 0x0000);
	tstr = read8(TMU(0, TSTR));
	write8(TMU(0, TSTR), tstr & ~0x1);
}

static void __maybe_unused armadillo8x0_led_on(struct platform_info *pinfo,
					       u32 leds)
{
	if (leds & LED_RED)
		gpio_set_value(xLED2, 1);
	if (leds & LED_GREEN)
		gpio_set_value(xLED1, 1);
}

static void __maybe_unused armadillo8x0_led_off(struct platform_info *pinfo,
						u32 leds)
{
	if (leds & LED_RED)
		gpio_set_value(xLED2, 0);
	if (leds & LED_GREEN)
		gpio_set_value(xLED1, 0);
}

static int armadillo8x0_get_mmu_page_tables(struct platform_info *pinfo,
					    u32 mode, struct page_table **pt,
					    int *nr_pt)
{
	*pt = (struct page_table *)&pt_list;
	*nr_pt = ARRAY_SIZE(pt_list);

	return 0;
}

#define on_off_string(x) ((x) ? "ON" : "OFF")
static void __maybe_unused armadillo8x0_display_info(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;

	hprintf("      Board Type: 0x%x\n"
		"Product Revision: %d.%d\n"
		"      Lot Number: 0x%x\n"
		"   Serial Number: %d\n"
		"       Boot Mode: 0x%x (%s)\n",
		priv->hw_type,
		priv->product_rev_major, priv->product_rev_minor,
		priv->lot_num, priv->serial_num,
		priv->boot_mode,
		get_boot_mode_str(priv->boot_mode));

	hprintf(" ORIG MAC-1: ");
	print_mac(priv->mac1);
	if (is_valid_mac(priv->umac1)) {
		hprintf(" USER MAC-1: ");
		print_mac(priv->umac1);
	}
}

static int armadillo8x0_is_autoboot(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;
	int autoboot = priv->jumper ? 0 : 1;

	if (priv->boot_mode == BOOT_MODE_SD &&
	    priv->tactsw[0]) {
		hprintf("*** Recovery Mode ******************************\n");
		autoboot = 1;
	    	pinfo->adjust_boot_args = adjust_recovery_boot_args;
	    	pinfo->adjust_boot_device = adjust_recovery_boot_device;
	}

	return autoboot;
}

static void armadillo8x0_prepare_normal_boot(struct platform_info *pinfo)
{
	pinfo->adjust_boot_args = NULL;
	pinfo->adjust_boot_device = NULL;
}

static void update_target_profile(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;
	char *ptr = target_name;

	ptr += hsprintf(ptr, "%s", priv->board_name ? : "Armadillo-800 EVA");
	if (strlen(CONFIG_PROFILE_NAME))
		hsprintf(ptr, "/%s", CONFIG_PROFILE_NAME);
}

static void armadillo8x0_setup_console(struct platform_info *pinfo)
{
	char console[128];
	int ret;
	int autoboot = armadillo8x0_is_autoboot(pinfo);

#if defined(CONFIG_CMD_SETENV)
	ret = get_param_value("console", console, 128);
	if (ret == 0 && strcmp(console, "none") == 0)
		if (!autoboot)
			ret = 1;
	if (ret)
#endif
		hsprintf(console, "%s", CONFIG_DEFAULT_CONSOLE);

	/* we must set up to STANDARD_CONSOLE to match our hardware.
	   FIXME: this is redundant and just using a side effect of
	   the function to initialize it */
	change_console(CONFIG_STANDARD_CONSOLE);
	ret = change_console(console);
	if (ret && !autoboot)
		change_console(CONFIG_STANDARD_CONSOLE);
}

static void __maybe_unused armadillo8x0_setup_ethernet(struct platform_info *pinfo)
{
	struct net_device *ndev = pinfo->net_dev;

	cpg_clr_mstpcr(309);
	if (ndev && ndev->set_mac) {
		armadillo8x0_get_mac_address(pinfo, ndev->eth_mac);
		ndev->set_mac(ndev, ndev->eth_mac);
	}
}

static void __maybe_unused armadillo8x0_rebooted_by_watchdog(void)
{
	hprintf("rebooted by watchdog timedout.\n");
}

static void armadillo8x0_setup_watchdog(void)
{
#if defined(CONFIG_WDT_RMA1)
	rma1_irq_init();
	callback_rebooted_by_watchdog = armadillo8x0_rebooted_by_watchdog;
#endif
}

static struct memory_map armadillo8x0_memory_map = {
	/* default memory map: RAM = 512MB, (FLA = 128MB) */
	.ram		= { RAM_ADDR(0x00000000), 0 },
	.boot_param	= { RAM_ADDR(0x00000100), 0x00000f00 },
	.mmu_table	= { RAM_ADDR(0x00004000), 0x00004000 },
	.kernel		= { RAM_ADDR(0x00008000), 0x007f8000 },
/*	.hermit		= { RAM_ADDR(0x00800000), 0x00300000 }, */
/*	.fec_desc	= { RAM_ADDR(0x00b00000), 0x00100000 }, */
	.gunzip		= { RAM_ADDR(0x00c00000), 0x00100000 },
/*	.stack(irq)	= { RAM_ADDR(0x00d00000), 0x00100000 }, */
/*	.stack(svc)	= { RAM_ADDR(0x00e00000), 0x00100000 }, */
/*	.vector		= { RAM_ADDR(0x00f00000), 0x00100000 }, */
	.free		= { RAM_ADDR(0x01000000), 0x08000000 },
	.initrd		= { RAM_ADDR(0x09000000), 0x17000000 },
	.param          = { RAM_ADDR(0x05018000), 0x00008000 },
};

struct boot_device boot_device_table[] = {
	/*			minor	minor	*/
	/* name		major	start	inc	*/
	{"ram",		1,	0,	1,	},
	{"mtdblock",	31,	0,	1,	},
	{"mmcblk0p",	0,	1,	1,	},
};

static void armadillo8x0_pre_common_init(void)
{
	/* disable cache on ICB */
	write32(0xfe950098, 0x01600164);

	/* Module Enable */
	cpg_clr_mstpcr(125); /* TMU0 */
}

static void armadillo8x0_post_common_init(void)
{
	armadillo8x0_setup_watchdog();
}
