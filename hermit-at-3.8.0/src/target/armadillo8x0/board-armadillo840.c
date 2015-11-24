#include <autoconf.h>
#include <hermit.h>
#include <herrno.h>
#include <init.h>
#include <platform.h>
#include <at88sc.h>

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
	/* CS0, CS2 */
	{0x00000000, 0x00000000, 0x08000000, 0xc02},
	{0x08000000, 0x08000000, 0x08000000, 0xc02},
	/* GbEther */
	{0xe9a00000, 0xe9a00000, 0x00200000, 0xc02},
};

#include "board.c"

int rma1_uart_request_port(int port)
{
	switch (port) {
	case 2: /* CON4 */
		pinmux(xUART_TXD);
		pinmux(xUART_RXD);
		pinmux(xUART_RTS);
		pinmux(xUART_CTS);
		break;
	default:
		return -H_EINVAL;
	}

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
	pinmux(xSDSLOT1_PON);

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
	argv[1] = "console=ttySC2,115200";
	argv[2] = "noinitrd";
	argv[3] = "rootwait";
	argv[4] = "root=/dev/mmcblk0p2";
}

static void adjust_recovery_boot_device(struct platform_info *pinfo,
					char *device)
{
	hsnprintf(device, CMDLINE_MAXLEN, "mmcblk0p2");
}

static void armadillo840_led_on(struct platform_info *pinfo, u32 leds)
{
	if (leds & LED_RED)
		gpio_set_value(xLED2, 1);

	if (leds & LED_GREEN)
		gpio_set_value(xLED1, 1);
}

static void armadillo840_led_off(struct platform_info *pinfo, u32 leds)
{
	if (leds & LED_RED)
		gpio_set_value(xLED2, 0);

	if (leds & LED_GREEN)
		gpio_set_value(xLED1, 0);
}

static void armadillo840_setup_flash(struct platform_info *pinfo)
{
	int val;

	flash_initialize(FLASH_TYPE_INTEL, FLASH_START);

	val = flash_get_size(FLASH_START);
	pinfo->map->flash.base = FLASH_START;
	pinfo->map->flash.size = 1 << val;
	hsprintf(pinfo->default_mtdparts,
		 "physmap-flash.0:"
		 "%p(all)ro,"
		 "0x40000@0(bootloader)ro,"
		 "0x40000(config),"
		 "0x40000(license)ro,"
		 "0x400000(firmware)ro,"
		 "0x400000(kernel),"
		 "-(userland)",
		 pinfo->map->flash.size);
}

static void armadillo840_setup_map(struct platform_info *pinfo)
{
	size_t size, sub_part_off, sub_part_size;
	struct board_private *priv = pinfo->private_data;

	size = 1 << DRAM_ID_SIZE(priv->dram_id);
	pinfo->map->ram.size = size;

	sub_part_off  = DRAM_ACM_OFF + DRAM_ACM_SIZE;
	sub_part_size = size - sub_part_off;

	if (sub_part_size > 0)
		hsprintf(pinfo->default_ramparts, "%p(dram-1),%p@%p(dram-2)",
			 DRAM_ACM_OFF, sub_part_size, sub_part_off);
	else
		hsprintf(pinfo->default_ramparts, "%p(dram-1)", DRAM_ACM_OFF);
}

static void armadillo840_display_info(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;

	hprintf("       Jumper: %s\n",
		on_off_string(priv->jumper));
}

static void armadillo840_setup_private_data(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;
	u8 val;

	at88sc_read(0x00, (void *)priv->mac1, 6);
	at88sc_read(0x40, (void *)priv->umac1, 6);
	at88sc_read(0x20, (void *)&priv->hw_type, 4);
	at88sc_read(0x30, (void *)&priv->dram_id, 4);
	at88sc_read(0x24, (void *)&priv->product_rev_major, 2);
	at88sc_read(0x26, (void *)&priv->product_rev_minor, 2);
	at88sc_read(0x2c, (void *)&priv->lot_num, 3);
	at88sc_read(0x2f, (void *)&val, 1);
	if (val)
		at88sc_read(0x28, (void *)&priv->serial_num, 4);

	pinfo->system_rev = ((u32)(priv->product_rev_major << 16) |
			     (u32)priv->product_rev_minor);
	pinfo->serial_num = priv->serial_num;
	pinfo->machine_nr = MACH_ARMADILLO840;
	priv->board_name = "Armadillo-840";
	priv->lot_num &= 0xffffff; /* 24bit mask */
	priv->lot_num = (priv->lot_num ? priv->lot_num : 1); /* workaround */
	priv->boot_mode = (read32(SYSC(RESCNT)) >> 24) & 0x7c;
	priv->jumper = gpio_get_value(xJP1) ? 0 : 1;

	priv->display_info = armadillo840_display_info;
}

static void armadillo840_init(void)
{
	struct platform_info *pinfo = &platform_info;

	armadillo8x0_pre_common_init();

	/* CS2BCR */
	write32(0xfec10008, 0x12490400);

	/* Setup Pinmux */
	pinmux(xJP1);
	pinmux(xLED1);
	pinmux(xLED2);
	pinmux(xEEPROM_SCL);
	pinmux(xEEPROM_SDA);
	pinmux(xDBGMD_EN_N);
	rma1_uart_request_port(2);
	rma1_sdhc_request_port(0);
	rma1_eth_request_port();

	/* LED1/2 ON */
	gpio_direction_output(xLED1, 1);
	gpio_direction_output(xLED2, 1);

	/* DBGMD/EXT_IO MUX: select EXT_IO */
	gpio_direction_output(xDBGMD_EN_N, 1);

	rma1_setup_i2c(1);
	rma1_setup_i2c(2);
	at88sc_init();
	armadillo840_setup_private_data(pinfo);
	update_target_profile(pinfo);

	armadillo8x0_setup_ethernet(pinfo);
	armadillo840_setup_flash(pinfo);
	armadillo840_setup_map(pinfo);

	memdev_add(flash, &mdev_param,
		   "hermit/param", FLASH_ADDR(0x00020000), 0x00008000);

	armadillo8x0_post_common_init();
}
arch_initcall(armadillo840_init);

static void armadillo840_late_init(void)
{
	struct platform_info *pinfo = &platform_info;
	armadillo8x0_setup_console(pinfo);
}
latearch_initcall(armadillo840_late_init);

struct platform_info platform_info = {
	.prepare_normal_boot = armadillo8x0_prepare_normal_boot,
	.is_autoboot = armadillo8x0_is_autoboot,
	.led_on = armadillo840_led_on,
	.led_off = armadillo840_led_off,
	.udelay = armadillo8x0_udelay,
	.get_mmu_page_tables = armadillo8x0_get_mmu_page_tables,
	.display_info = armadillo8x0_display_info,
	.get_mac_address = armadillo8x0_get_mac_address,

	.private_data = &board_priv,
	.map = &armadillo8x0_memory_map,

	.net_dev = &rma1_eth,
	.mmcsd_dev = &rma1_mmcsd,
};
