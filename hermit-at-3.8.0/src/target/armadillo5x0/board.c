#include <autoconf.h>
#include <platform.h>
#include <herrno.h>
#include <htypes.h>
#include <io.h>
#include <string.h>
#include <flash.h>
#include <map.h>
#include <machine.h>
#include <medium.h>
#include <led.h>
#include <delay.h>
#include <arch/ioregs.h>

#include "board.h"

#include <param.h>
#include "mm.h"
#include <net/smsc911x.h>

#define FLASH_ADDR(offset) (FLASH_START + (offset))
#define RAM_ADDR(offset) (DRAM_START + (offset))

char target_name[256];
char *target_profile = target_name;

static struct board_private board_private_data;
static void update_target_profile(void);

static char *armadillo5x0_support_clocks[] = {
	CONFIG_PLATFORM_DEFCLOCK, "400", "532", NULL,
};

static u32 uart_base[] = {
	UART1_BASE_ADDR,
	UART2_BASE_ADDR,
#if defined(CONFIG_PLATFORM_ARMADILLO500_FX)
	UART5_BASE_ADDR,
#endif
};

void mxc_set_mux(u32 mux, u32 config)
{
	u32 offset = (mux & ~0x03);
	u32 field = (mux & 0x03);
	u32 val;

	if (mux < 0x00c || 0x153 < mux)
		return; /* Invalid mux */

	val = read32(IOMUXC_BASE_ADDR + offset);
	val = (val & ~(0xff << (field * 8)));
	val |= ((config & 0xff) << (field * 8));
	write32(IOMUXC_BASE_ADDR + offset, val);
}

u32 mxc_get_mux(u32 mux)
{
	u32 offset = (mux & ~0x03);
	u32 field = (mux & 0x03);
	u32 val;

	if (mux < 0x00c || 0x153 < mux)
		return -1; /* Invalid mux */

	val = read32(IOMUXC_BASE_ADDR + offset);
	return ((val >> (field * 8)) & 0xff);
}

void mxc_set_pad(u32 pad, u32 config)
{
	u32 offset = (pad & ~0x03);
	u32 field = (pad & 0x03);
	u32 val;

	if (pad < 0x154 || 0x30a < pad)
		return; /* Invalid pad */
	if (field >= 0x3)
		return; /* Invalid field */

	val = read32(IOMUXC_BASE_ADDR + offset);
	val = (val & ~(0x3ff << (field * 10)));
	val |= ((config & 0x3ff) << (field * 10));
	write32(IOMUXC_BASE_ADDR + offset, val);
}

int mxc_get_gpio(u32 gpio)
{
	u32 offset = (gpio / 32);
	u32 field = (gpio % 32);
	u32 psr;
	u32 base_addr[3] = { GPIO1_BASE_ADDR,
			     GPIO2_BASE_ADDR,
			     GPIO3_BASE_ADDR };

	if (offset > 2)
		return -1; /* Invalid */

	psr = read32(base_addr[offset] + PSR);
	return ((psr & (1 << field)) != 0);
}

void mxc_set_gpio(u32 gpio, u32 direction, u32 data)
{
	u32 offset = (gpio / 32);
	u32 field = (gpio % 32);
	u32 dr, gdir;
	u32 base_addr[3] = { GPIO1_BASE_ADDR,
			     GPIO2_BASE_ADDR,
			     GPIO3_BASE_ADDR };

	if (offset > 2)
		return; /* Invalid */

	gdir = read32(base_addr[offset] + GDIR);
	gdir &= (~(1 << field));
	gdir |= ((direction ? 1 : 0) << field);
	write32(base_addr[offset] + GDIR, gdir);

	if (direction) {
		dr = read32(base_addr[offset] + DR);
		dr &= (~(1 << field));
		dr |= ((data ? 1 : 0) << field);
		write32(base_addr[offset] + DR, dr);
	}
}

static void init_mux(void)
{
	/* CPU ID */
	mxc_set_mux(MUX_PIN(xCPU1), MUX_O_GPIO | MUX_I_GPIO);
	mxc_set_mux(MUX_PIN(xCPU2), MUX_O_GPIO | MUX_I_GPIO);
	mxc_set_mux(MUX_PIN(xCPU3), MUX_O_GPIO | MUX_I_GPIO);

	/* UART1 */
	mxc_set_mux(MUX_PIN(RXD1), MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(TXD1), MUX_O_FUNC | MUX_I_FUNC);

	init_mux_private();
}

static void init_weim(void)
{
	/* GPR Setting */
	write32(IOMUXC_BASE_ADDR + GPR, 0x2000);

	init_weim_private();
}

static void init_uart(void)
{
	int baud = 115200;
	int i;

	/* periferal clock(ipg_per_clk) use USB clock source(60MHz) */
	write32(CCM_BASE_ADDR + CCMR,
		read32(CCM_BASE_ADDR + CCMR) & ~(CCMR_PERCS));

	for (i = 0; i < ARRAY_SIZE(uart_base); i++) {
		write32(uart_base[i] + UCR3, 0x0704);
		write32(uart_base[i] + UFCR, 0x0a01);
		write32(uart_base[i] + ONEMS, 0x81ea);

		switch(baud){
		case 115200:
		default:
			write32(uart_base[i] + UBIR, 0x0fa2);
			write32(uart_base[i] + UBMR, 0xfe80);
			break;
		}

		write32(uart_base[i] + UCR1, UCR1_UARTEN);
		write32(uart_base[i] + UCR2, UCR2_RXEN | UCR2_TXEN | 0x4021);
		write32(uart_base[i] + USR2, 0xffff);
		write32(uart_base[i] + USR1, 0xffff);
	}
}

static void disable_uart(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(uart_base); i++) {
		while(!(read32(uart_base[i] + USR2) & USR2_TXDC));
		write32(uart_base[i] + UCR1,
			read32(uart_base[i] + UCR1) & ~(UCR1_UARTEN));
	}
}

static void enable_uart(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(uart_base); i++)
		write32(uart_base[i] + UCR1,
			read32(uart_base[i] + UCR1) | UCR1_UARTEN);
}

static char *get_active_clock(void)
{
#if defined(CONFIG_SYSTEM_ENTRYPOINT_FLASH) && defined(CONFIG_CMD_SETENV)
	static char buf[256];

	if (get_param_value("@clock", buf, 256) == 0)
		return buf;
	else
#endif
		return armadillo5x0_support_clocks[0];
}

static int do_change_clock(struct platform_info *pinfo, char *clock)
{
	u32 ccmr;
	u32 prcs;
	u32 mpctl;
	u32 pdr0;

	ccmr = read32(CCM_BASE_ADDR + CCMR);
	prcs = (ccmr & CCMR_PRCS);

	if (strcmp(clock, "400") == 0) {
		pdr0 = PDR0_400MHZ;
		mpctl = ((prcs == PRCS_FPM) ?
			 MPCTL_FPM_400 : MPCTL_CKIH_400);
	} else	if (strcmp(clock, "532") == 0) {
		pdr0 = PDR0_532MHZ;
		mpctl = ((prcs == PRCS_FPM) ?
			 MPCTL_FPM_532 : MPCTL_CKIH_532);
	} else
		return -1;

	write32(CCM_BASE_ADDR + CCMR, (ccmr & ~(CCMR_MPE | CCMR_SPE | CCMR_UPE)) | CCMR_MDS);
	write32(CCM_BASE_ADDR + PDR0, pdr0);
	write32(CCM_BASE_ADDR + MPCTL, mpctl);
	write32(CCM_BASE_ADDR + CCMR, ccmr);
	return 0;
}

static int armadillo5x0_change_clock(struct platform_info *pinfo, char *clock)
{
	int ret;

	disable_uart();
	ret = do_change_clock(pinfo, clock);
	enable_uart();

	update_target_profile();

	return ret;
}

static void armadillo5x0_led_on(struct platform_info *pinfo, u32 leds)
{
	PRIVATE_LED_ON();
}

static void armadillo5x0_led_off(struct platform_info *pinfo, u32 leds)
{
	PRIVATE_LED_OFF();
}

static void armadillo5x0_udelay(struct platform_info *pinfo, u32 usecs)
{
	u32 base = EPIT1_BASE_ADDR;

	write32(base + EPITCR, 0x0102041a);
	write32(base + EPITSR, 0x01);
	write32(base + EPITCMPR, 0xffffffff - usecs);

	write32(base + EPITCR, 0x0102041b);

	while (!(read32(base + EPITSR) & 0x1));

	write32(base + EPITSR, 0x01);

	write32(base + EPITCR, 0x0102041a);
}

static int armadillo5x0_is_autoboot(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;
	return !priv->jumper1;
}

static int get_board_info(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;

	memzero(priv, sizeof(struct board_private));

	/* CPU Board ID */
	mxc_set_gpio(GPIO_PIN(xCPU1), GPIO_INPUT, 0);
	mxc_set_gpio(GPIO_PIN(xCPU2), GPIO_INPUT, 0);
	mxc_set_gpio(GPIO_PIN(xCPU3), GPIO_INPUT, 0);

	priv->cpu_board_id = mxc_get_gpio(GPIO_PIN(xCPU1)) ? (1 << 0):0;
	priv->cpu_board_id |= mxc_get_gpio(GPIO_PIN(xCPU2)) ? (1 << 1):0;
	priv->cpu_board_id |= mxc_get_gpio(GPIO_PIN(xCPU3)) ? (1 << 2):0;

	get_board_info_private(priv);

	return 0;
}

static char *get_board_name(u32 id)
{
	return "Armadillo-500";
}

static void update_target_profile(void)
{
	char *ptr = target_name;
	char *active_clock;

	ptr += hsprintf(ptr, "%s", get_board_name(0));
	if (strlen(CONFIG_PROFILE_NAME))
		ptr += hsprintf(ptr, "/%s", CONFIG_PROFILE_NAME);

	active_clock = get_active_clock();
	ptr += hsprintf(ptr, " %s", active_clock);
	if (strcmp(active_clock, armadillo5x0_support_clocks[0]))
		ptr += hsprintf(ptr, "@%s", armadillo5x0_support_clocks[0]);
	ptr += hsprintf(ptr, "MHz");
}

static int armadillo5x0_smsc911x_init(struct platform_info *pinfo)
{
#if defined(CONFIG_ETHERNET_SMSC911X)
	mxc_set_mux(MUX_PIN(GPIO1_0),	MUX_O_GPIO | MUX_I_GPIO);
	mxc_set_mux(MUX_PIN(GPIO1_1),	MUX_O_GPIO | MUX_I_GPIO);
	mxc_set_mux(MUX_PIN(CS3),	MUX_O_FUNC | MUX_I_FUNC);

	pinfo->net_dev->base_addr = 0xB2000000;
#endif
	return 0;
}

static void armadillo5x0_init_console(struct platform_info *pinfo)
{
	char console[128];
	int ret;
	int autoboot = armadillo5x0_is_autoboot(pinfo);

#if defined(CONFIG_SYSTEM_ENTRYPOINT_FLASH) && defined(CONFIG_CMD_SETENV)
	ret = get_param_value("console", console, 128);
	if (ret == 0 && strcmp(console, "none") == 0)
		if (!autoboot)
			ret = 1;
	if (ret)
#endif
		hsprintf(console, "%s", CONFIG_DEFAULT_CONSOLE);

	ret = change_console(console);
	if (ret && !autoboot)
		change_console(CONFIG_STANDARD_CONSOLE);
}

static void board_init(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;

	init_mux();
	init_weim();
	init_uart();

	init_ipu();
	init_i2c();

	/* get board info */
	get_board_info(pinfo);

	if (strcmp(armadillo5x0_support_clocks[0], "auto") == 0) {
		if ((priv->cpu_board_id == DRAM_ID_32Mx2_r201) ||
		    (priv->cpu_board_id == DRAM_ID_64Mx2_r201))
			armadillo5x0_support_clocks[0] = "532";
		else
			armadillo5x0_support_clocks[0] = "400";
	}
}

#if defined(CONFIG_ETHERNET_SMSC911X)
static void armadillo5x0_get_mac_address(struct platform_info *pinfo, u8 *addr)
{
	smsc911x_get_mac(addr);
}
#endif

static void armadillo5x0_display_info(struct platform_info *pinfo)
{
	struct board_private *priv = &board_private_data;
	hprintf("CPU Silicon Rev: %p\n", read32(IIM_BASE_ADDR + SREV) & 0xff);
	hprintf("CPU Board ID   : %p\n", priv->cpu_board_id);
#if defined(CONFIG_HAS_BASE_BOARD_ID)
	hprintf("BASE Board ID  : %p\n", priv->base_board_id);
#endif
	hprintf("HAB-TYPE       : %p\n", read32(IIM_BASE_ADDR + HAB1) & 0x07);
}

static void armadillo5x0_init_flash(struct platform_info *pinfo)
{
	char *region_all;
	char *region_all_8m = "0x800000(all)ro";
	char *region_all_16m = "0x1000000(all)ro";
	char *region_all_32m = "0x2000000(all)ro";
	char *region_all_64m = "0x4000000(all)ro";
	char *region_bootloader = "0x20000@0(bootloader)ro";
	char *region_kernel = "0x200000(kernel)";
	char *region_userland;
	char *region_userland_8m = "0x5c0000(userland)";
	char *region_userland_16m = "0xdc0000(userland)";
	char *region_userland_32m = "0x1dc0000(userland)";
	char *region_userland_64m = "0x3dc0000(userland)";
	char *region_config = "-(config)";
	int val;

	flash_initialize(FLASH_TYPE_INTEL, FLASH_START);

	val = flash_get_size(FLASH_START);
	switch (val) {
	case 23: /* 8MB */
		region_all = region_all_8m;
		region_userland = region_userland_8m;
		break;
	case 25: /* 32MB */
		region_all = region_all_32m;
		region_userland = region_userland_32m;
		break;
	case 26: /* 64MB */
		region_all = region_all_64m;
		region_userland = region_userland_64m;
		break;
	case 24: /* 16MB */
	default:
		region_all = region_all_16m;
		region_userland = region_userland_16m;
		break;
	}
	pinfo->map->flash.size = 1 << val;
	hsprintf(pinfo->default_mtdparts, "armadillo5x0-nor:%s,%s,%s,%s,%s",
		 region_all, region_bootloader, region_kernel,
		 region_userland, region_config);

#if defined(CONFIG_ARMADILLO500_DEV_WORKAROUND_A500_ERRATUM_7)
	{
		/*
		 * Set the NOR WP active low.
		 *
		 * See A500-Erratum #7 for details.
		 */
		uint32_t rcr_data;

		write16(FLASH_ADDR(0), 0x0090);
		rcr_data = read16(FLASH_ADDR(0x0a));

		rcr_data &= ~BIT(10);
		rcr_data <<= 1;

		write16(FLASH_ADDR(rcr_data), 0x0060);
		write16(FLASH_ADDR(rcr_data), 0x0003);

		write16(FLASH_ADDR(0), 0x00ff);
	}
#endif
}

static void armadillo5x0_init_map(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;
	char *dram1;
	char *dram1_64m = "0x4000000(dram-1)";
	char *dram1_128m = "0x8000000(dram-1)";
	char *dram1_256m = "0x10000000(dram-1)";
	size_t size;

	switch (priv->cpu_board_id) {
	case DRAM_ID_64Mx2:
	case DRAM_ID_64Mx2_r201:
		dram1 = dram1_128m;
		size = 0x4000000;
		break;
	case DRAM_ID_128Mx2:
		dram1 = dram1_256m;
		size = 0x8000000;
		break;
	case DRAM_ID_32Mx2:
	case DRAM_ID_32Mx2_r201:
	default:
		dram1 = dram1_64m;
		size = 0x2000000;
		break;
	}
	pinfo->map->ram.size = size;
	hsprintf(pinfo->default_ramparts, "%s", dram1);
}

static void armadillo5x0_init(struct platform_info *pinfo)
{
	pinfo->is_autoboot = armadillo5x0_is_autoboot;

	board_init(pinfo);

	armadillo5x0_init_flash(pinfo);
	armadillo5x0_init_map(pinfo);

	armadillo5x0_init_console(pinfo);

	do_change_clock(pinfo, get_active_clock());

	/* update target_profile string */
	update_target_profile();

	armadillo5x0_smsc911x_init(pinfo);
}

static struct memory_map armadillo5x0_memory_map = {
	.flash		= { FLASH_ADDR(0x00000000), 0 },
	.param		= { FLASH_ADDR(0x00018000), 0x00008000 },

	.ram		= { RAM_ADDR(0x00000000), 0 },
	.boot_param	= { RAM_ADDR(0x00000100), 0x00000f00 },
	.mmu_table	= { RAM_ADDR(0x00004000), 0x00004000 },
	.kernel		= { RAM_ADDR(0x00008000), 0x007f8000 },
	.initrd		= { RAM_ADDR(0x00800000), 0x01800000 },
	.free		= { RAM_ADDR(0x02000000), 0x00f00000 },
	.gunzip		= { RAM_ADDR(0x02f00000), 0x00020000 },
/*      .hermit		= { RAM_ADDR(0x03000000), 0x00f00000 }, */
/*      .stack		= { RAM_ADDR(0x03f00000), 0x00100000 }, */
};

struct ide_device armadillo5x0_pcmcia;
extern struct mmcsd_device mx3_mmcsd;

struct platform_info platform_info = {
	.init = armadillo5x0_init,
	.led_on = armadillo5x0_led_on,
	.led_off = armadillo5x0_led_off,
	.udelay = armadillo5x0_udelay,
	.get_mmu_page_tables = armadillo5x0_get_mmu_page_tables,
	.change_clock = armadillo5x0_change_clock,
	.display_info = armadillo5x0_display_info,
#if defined(CONFIG_ETHERNET_SMSC911X)
	.get_mac_address = armadillo5x0_get_mac_address,
#endif

	.private_data = &board_private_data,
	.map = &armadillo5x0_memory_map,
	.machine_nr = MACH_NUMBER,
	.support_clocks = armadillo5x0_support_clocks,

#if defined(CONFIG_ETHERNET_SMSC911X)
	.net_dev = &smsc911x,
#endif
	.ide_dev = &armadillo5x0_pcmcia,

#if defined(CONFIG_MMCSD_MX31_SDHC)
	.mmcsd_dev = &mx3_mmcsd,
#endif
};
