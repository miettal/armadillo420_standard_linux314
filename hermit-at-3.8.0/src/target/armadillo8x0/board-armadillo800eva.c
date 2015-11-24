#include <autoconf.h>
#include <hermit.h>
#include <herrno.h>
#include <init.h>
#include <platform.h>

static void adjust_recovery_boot_args(struct platform_info *pinfo,
				      int *argc, char *argv[]);
static void adjust_recovery_boot_device(struct platform_info *pinfo,
					char *device);

static struct page_table pt_list[] = {
	/* Physical,  Virtual,    Size,       Option */
	/*-------------------------------------------*/
	/* SDRAM */
	{0x40000000, 0x40000000, 0x00b00000, 0xc0e},
	{0x40b00000, 0x40b00000, 0x00100000, 0xc02},
	{0x40c00000, 0x40c00000, 0x00300000, 0xc0e},
	{0x40f00000, 0xfff00000, 0x00100000, 0xc02}, /* for vector */
	{0x41000000, 0x41000000, 0x1f000000, 0xc0e},
	/* Peripheral modules */
	{0xc2000000, 0xc2000000, 0x00100000, 0xc02}, /* GIC */
	{0xc2800000, 0xc2800000, 0x00100000, 0xc02}, /* GIC */
	{0xe6000000, 0xe6000000, 0x02000000, 0xc02},
	{0xffc00000, 0xffc00000, 0x00300000, 0xc02},
	{0xfff00000, 0xe5f00000, 0x00100000, 0xc02},
	/* GbEther */
	{0xe9a00000, 0xe9a00000, 0x00200000, 0xc02},
};

#include "board.c"

int rma1_uart_request_port(int port)
{
	switch (port) {
	case 1:
		pinmux(xUART_TXD);
		pinmux(xUART_RXD);
		break;
	default:
		return -H_EINVAL;
	}
	return 0;
}

int rma1_mmc_request_port(void)
{
	pinmux(PIN_FN_MMC_CLK_103);
	pinmux(PIN_FN_MMC_CMD_104);
	pinmux(PIN_FN_MMC_D0_149);
	pinmux(PIN_FN_MMC_D1_148);
	pinmux(PIN_FN_MMC_D2_147);
	pinmux(PIN_FN_MMC_D3_146);
	pinmux(PIN_FN_MMC_D4_145);
	pinmux(PIN_FN_MMC_D5_144);
	pinmux(PIN_FN_MMC_D6_143);
	pinmux(PIN_FN_MMC_D7_142);
	pinmux(xMMC0_RST_B);

	return 0;
}

int rma1_sdhc_request_port(int port)
{
	pinmux(PIN_FN_SDHI0_CLK_82);
	pinmux(PIN_FN_SDHI0_CMD_76);
	pinmux(PIN_FN_SDHI0_D0_77);
	pinmux(PIN_FN_SDHI0_D1_78);
	pinmux(PIN_FN_SDHI0_D2_79);
	pinmux(PIN_FN_SDHI0_D3_80);
	pinmux(PIN_FN_SDHI0_CD_81);
	pinmux(PIN_FN_SDHI0_WP_82);

	pinmux(xSDHI0_VS_B);
	pinmux(xSDHI0_PON);
	pinmux(xSDSLOT1_PON);
	pinmux(xSDSLOT1_CD);

	return 0;
}

int rma1_eth_request_port(void)
{
	/* setup GETHER/MII */
	pinmux(PIN_FN_ETH_CRS_205);
	pinmux(PIN_FN_ETH_MDC_206);
	pinmux(PIN_FN_ETH_MDIO_207);
	pinmux(PIN_FN_ETH_TX_ER_203);
	pinmux(PIN_FN_ETH_RX_ER_204);
	pinmux(PIN_FN_ETH_ERXD0_185);
	pinmux(PIN_FN_ETH_ERXD1_186);
	pinmux(PIN_FN_ETH_ERXD2_187);
	pinmux(PIN_FN_ETH_ERXD3_188);
	pinmux(PIN_FN_ETH_TX_CLK_184);
	pinmux(PIN_FN_ETH_TX_EN_183);
	pinmux(PIN_FN_ETH_ETXD0_171);
	pinmux(PIN_FN_ETH_ETXD1_170);
	pinmux(PIN_FN_ETH_ETXD2_169);
	pinmux(PIN_FN_ETH_ETXD3_168);
	pinmux(PIN_FN_ETH_PHY_INT_164);
	pinmux(PIN_FN_ETH_COL_163);
	pinmux(PIN_FN_ETH_RX_DV_161);
	pinmux(PIN_FN_ETH_RX_CLK_174);
	pinmux(xETH_PHYRST);

	return 0;
}

static void adjust_recovery_boot_args(struct platform_info *pinfo,
				      int *argc, char *argv[])
{
	*argc = 5;
	argv[1] = "console=ttySC1,115200";
	argv[2] = "noinitrd";
	argv[3] = "rootwait";
	argv[4] = "root=/dev/mmcblk1p2";
}

static void adjust_recovery_boot_device(struct platform_info *pinfo,
					char *device)
{
	hsnprintf(device, CMDLINE_MAXLEN, "mmcblk1p2");
}

static void armadillo800eva_setup_map(struct platform_info *pinfo)
{
	size_t size;

	size = 1 << DRAM_SIZE_512MB;
	pinfo->map->ram.size = size;
	hsprintf(pinfo->default_ramparts, "%p(dram-1)", size);
#if !defined(CONFIG_SYSTEM_ENTRYPOINT_FLASH)
	pinfo->map->flash.size = 16*1024*1024;
	hsprintf(pinfo->default_mtdparts,
		 "armadillo8x0-nor:%p(all)ro,0x20000@0(bootloader)ro,"
		 "0x600000(kernel),%p(userland),-(config)",
		 pinfo->map->flash.size, pinfo->map->flash.size - 0x640000);
#endif
}

static void armadillo800eva_display_info(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;

	hprintf("   Board Type: 0x%x\n"
		"     Revision: 0x%x\n"
		"          Lot: 0x%x\n"
		"Serial Number: %d\n"
		"    Boot Mode: 0x%x (%s)\n",
		priv->hw_type, priv->product_rev_major,
		priv->lot_num, priv->serial_num,
		priv->boot_mode,
		get_boot_mode_str(priv->boot_mode));

	hprintf(" ORIG MAC-1: ");
	print_mac(priv->mac1);
	if (is_valid_mac(priv->umac1)) {
		hprintf(" USER MAC-1: ");
		print_mac(priv->umac1);
	}

	hprintf("       Jumper: %s\n"
		"      Tact-SW: %s,%s,%s,%s\n",
		on_off_string(priv->jumper),
		on_off_string(priv->tactsw[0]),
		on_off_string(priv->tactsw[1]),
		on_off_string(priv->tactsw[2]),
		on_off_string(priv->tactsw[3]));
}

static void armadillo800eva_setup_private_data(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;
	u8 addr = 0;
	u8 buf[4] = {0, 0, 0, 0};
	struct i2c_device i2c_dev = {
		.port = 1,
		.devid = 0xa0,
		.wait = 5,
	};
	struct i2c_msg msgs[] = {
		[0] = { &addr, 1, I2C_M_WR },
		[1] = { NULL, 6, I2C_M_RD | I2C_REPSTART },
	};

	/* MAC1 Address */
	msgs[1].buf = priv->mac1;
	rma1_i2c_xfer(&i2c_dev, msgs, 2);

	/* USER MAC1 Address */
	addr = 64;
	msgs[1].buf = priv->umac1;
	rma1_i2c_xfer(&i2c_dev, msgs, 2);

	/* Hardware Type */
	addr = 32;
	msgs[1].buf = (u8 *)&priv->hw_type;
	msgs[1].len = 4;
	rma1_i2c_xfer(&i2c_dev, msgs, 2);

	/* Hardware Revision */
	addr = 36;
	msgs[1].buf = (u8 *)&priv->product_rev_major;
	msgs[1].len = 2;
	rma1_i2c_xfer(&i2c_dev, msgs, 2);

	/* Lot Number */
	addr = 38;
	msgs[1].buf = (u8 *)&priv->lot_num;
	msgs[1].len = 2;
	rma1_i2c_xfer(&i2c_dev, msgs, 2);

	/* Serial Number */
	addr = 47;
	msgs[1].buf = buf;
	msgs[1].len = 1;
	rma1_i2c_xfer(&i2c_dev, msgs, 2);
	if (buf[0]) {
		addr = 40;
		msgs[1].buf = (u8 *)&priv->serial_num;
		msgs[1].len = 4;
		rma1_i2c_xfer(&i2c_dev, msgs, 2);
	} else {
		priv->serial_num = 0;
	}

	pinfo->system_rev = ((u32)(priv->product_rev_major << 16) |
			     (u32)priv->lot_num);
	pinfo->serial_num = priv->serial_num;
	pinfo->machine_nr = MACH_ARMADILLO800EVA;

	priv->boot_mode = (read32(SYSC(RESCNT)) >> 24) & 0x7c;
	priv->jumper = gpio_get_value(xJP1) ? 0 : 1;
	priv->tactsw[0] = gpio_get_value(xSW1) ? 0 : 1;
	priv->tactsw[1] = gpio_get_value(xSW2) ? 0 : 1;
	priv->tactsw[2] = gpio_get_value(xSW3) ? 0 : 1;
	priv->tactsw[3] = gpio_get_value(xSW4) ? 0 : 1;
}

static void armadillo800eva_init(void)
{
	struct platform_info *pinfo = &platform_info;

	armadillo8x0_pre_common_init();

	/* Setup Pinmux */
	pinmux(xJP1);
	pinmux(xSW1);
	pinmux(xSW2);
	pinmux(xSW3);
	pinmux(xSW4);
	pinmux(xLED1);
	pinmux(xLED2);
	pinmux(xLED3);
	pinmux(xLED4);
	rma1_uart_request_port(1);
	rma1_mmc_request_port();
	rma1_sdhc_request_port(0);
	rma1_eth_request_port();

	/* LED1/2 ON, LED3/4 OFF */
	gpio_direction_output(xLED1, 1);
	gpio_direction_output(xLED2, 1);
	gpio_direction_output(xLED3, 0);
	gpio_direction_output(xLED4, 0);

	rma1_setup_i2c(1);
	armadillo800eva_setup_private_data(pinfo);
	update_target_profile(pinfo);

	memset((void *)RAM_ADDR(0x05000000), 0, 0x00020000);
	armadillo800eva_setup_map(pinfo);
	armadillo8x0_setup_console(pinfo);
	armadillo8x0_setup_ethernet(pinfo);

	memdev_add(emmcboot1, &mdev_param,
		   "hermit/param", 0x00000000, 0x00002000);

	armadillo8x0_post_common_init();
}
arch_initcall(armadillo800eva_init);

static void armadillo800eva_late_init(void)
{
	struct platform_info *pinfo = &platform_info;
	armadillo8x0_setup_console(pinfo);
}
latearch_initcall(armadillo800eva_late_init);

struct platform_info platform_info = {
	.prepare_normal_boot = armadillo8x0_prepare_normal_boot,
	.is_autoboot = armadillo8x0_is_autoboot,
	.led_on = armadillo8x0_led_on,
	.led_off = armadillo8x0_led_off,
	.udelay = armadillo8x0_udelay,
	.get_mmu_page_tables = armadillo8x0_get_mmu_page_tables,
	.display_info = armadillo800eva_display_info,
	.get_mac_address = armadillo8x0_get_mac_address,

	.private_data = &board_priv,
	.map = &armadillo8x0_memory_map,

	.net_dev = &rma1_eth,
	.mmcsd_dev = &rma1_mmcsd,
};
