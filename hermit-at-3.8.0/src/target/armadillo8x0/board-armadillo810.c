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
	/* CS0 */
	{0x00000000, 0x00000000, 0x08000000, 0xc02},
	/* GbEther */
	{0xe9a00000, 0xe9a00000, 0x00200000, 0xc02},
};

#include "board.c"

int rma1_uart_request_port(int port)
{
	switch (port) {
	case 2: /* UART2 */
		pinmux(xUART2_TXD);
		pinmux(xUART2_RXD);
		pinmux(xUART2_RTS);
		pinmux(xUART2_CTS);
		break;
	case 8: /* UART1 */
		pinmux(xUART1_TXD);
		pinmux(xUART1_RXD);
		pinmux(xUART1_RTS);
		pinmux(xUART1_CTS);
		pinmux(xUART1_FORCEOFF);
		/* UART1 FORCEOFF_B deassert */
		gpio_direction_output(xUART1_FORCEOFF, 1);
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

#define GETHER_MAHR		0xe9a005c0
#define GETHER_MALR		0xe9a005c8
static void armadillo810_setup_macaddr(struct platform_info *pinfo)
{
	u8 mac[8];

	cpg_clr_mstpcr(309);
	armadillo8x0_get_mac_address(pinfo, mac);
	write32(GETHER_MAHR, (mac[0] << 24 | mac[1] << 16 |
			      mac[2] <<  8 | mac[3] <<  0));
	write32(GETHER_MALR, (mac[4] <<  8 | mac[5] <<  0));
	cpg_set_mstpcr(309);
}

static void armadillo810_setup_flash(struct platform_info *pinfo)
{
	int val;

	flash_initialize(FLASH_TYPE_INTEL, FLASH_START);

	val = flash_get_size(FLASH_START);
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

static void armadillo810_setup_map(struct platform_info *pinfo)
{
	size_t size;
	struct board_private *priv = pinfo->private_data;

	size = 1 << DRAM_ID_SIZE(priv->dram_id);
	pinfo->map->ram.size = size;
	hsprintf(pinfo->default_ramparts, "%p(dram-1)", size);
}

static void armadillo810_display_info(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;

	hprintf("       Jumper: %s\n",
		on_off_string(priv->jumper));
}

static void armadillo810_setup_private_data(struct platform_info *pinfo)
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
	pinfo->machine_nr = MACH_ARMADILLO810;
	priv->board_name = "Armadillo-810";
	priv->lot_num &= 0xffffff; /* 24bit mask */
	priv->lot_num = (priv->lot_num ? priv->lot_num : 1); /* workaround */
	priv->boot_mode = (read32(SYSC(RESCNT)) >> 24) & 0x7c;
	priv->jumper = gpio_get_value(xJP1) ? 0 : 1;

	priv->display_info = armadillo810_display_info;
}

static void armadillo810_init(void)
{
	struct platform_info *pinfo = &platform_info;

	armadillo8x0_pre_common_init();

	/* Setup Pinmux */
	pinmux(xJP1);
	pinmux(xLED1);
	pinmux(xLED2);
	pinmux(xLED3);
	pinmux(xLED4);
	pinmux(xEEPROM_SCL);
	pinmux(xEEPROM_SDA);
	pinmux(xDBGMD_EN_N);
	pinmux(xCAM_PON);
	rma1_uart_request_port(2);
	rma1_uart_request_port(8);

	/* LED1/2 ON, LED3/4 OFF */
	gpio_direction_output(xLED1, 1);
	gpio_direction_output(xLED2, 1);
	gpio_direction_output(xLED3, 0);
	gpio_direction_output(xLED4, 0);
	/* PWRSW.CAM Power-Off */
	gpio_direction_output(xCAM_PON, 0);
	/* DBGMD/EXT_IO MUX: select EXT_IO */
        gpio_direction_output(xDBGMD_EN_N, 1);

	rma1_setup_i2c(1);
	at88sc_init();
	armadillo810_setup_private_data(pinfo);
	update_target_profile(pinfo);

	armadillo810_setup_macaddr(pinfo);
	armadillo810_setup_flash(pinfo);
	armadillo810_setup_map(pinfo);

	memdev_add(flash, &mdev_param,
		   "hermit/param", FLASH_ADDR(0x00020000), 0x00008000);

	armadillo8x0_post_common_init();
}
arch_initcall(armadillo810_init);

static void armadillo810_late_init(void)
{
	struct platform_info *pinfo = &platform_info;
	armadillo8x0_setup_console(pinfo);
}
latearch_initcall(armadillo810_late_init);

struct platform_info platform_info = {
	.prepare_normal_boot = armadillo8x0_prepare_normal_boot,
	.is_autoboot = armadillo8x0_is_autoboot,
	.led_on = armadillo8x0_led_on,
	.led_off = armadillo8x0_led_off,
	.udelay = armadillo8x0_udelay,
	.get_mmu_page_tables = armadillo8x0_get_mmu_page_tables,
	.display_info = armadillo8x0_display_info,
	.get_mac_address = armadillo8x0_get_mac_address,

	.private_data = &board_priv,
	.map = &armadillo8x0_memory_map,

	.mmcsd_dev = &rma1_mmcsd,
};
