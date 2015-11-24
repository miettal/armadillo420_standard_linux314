/*
 * Copyright (C) 2015 Atmark Techno, Inc. All Rights Reserved.
 *
 * Based on mach-mx25_3ds.c which is :
 *   Copyright 2009 Sascha Hauer, <kernel@pengutronix.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#include <linux/types.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/pinctrl/machine.h>
#include <linux/platform_device.h>
#include <linux/usb/otg.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/fixed.h>
#include <linux/platform_data/pca953x.h>
#include <linux/leds.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/usb/imx_usb.h>
#include <linux/iio/adc/ti-adc081c.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>
#include <asm/memory.h>
#include <asm/mach/map.h>
#include <asm/system_info.h>

#include "common.h"
#include "devices-imx25.h"
#include "hardware.h"
#include "iomux-mx25.h"
#include "mx25.h"
#include "imx25-named_gpio.h"
#include "armadillo_iotg_std_extif.h"

#define SYSTEM_REV_BASE_BOARD_GEN_SHIFT	(16)
#define SYSTEM_REV_BASE_BOARD_GEN_MASK	(0xff << SYSTEM_REV_BASE_BOARD_GEN_SHIFT)

enum base_board_gens {
	SYSTEM_REV_BASE_BOARD_GEN1 = 0,
	SYSTEM_REV_BASE_BOARD_GEN2 = 1,
};

#define EXT_GPIO_NR(nr)	(IMX25_NR_GPIOS + (nr))

static const struct imxuart_platform_data uart1_pdata __initconst = {
	/* .flags = IMXUART_HAVE_RTSCTS, */
};

static unsigned long pin_cfgs_100kup[] = {
	PAD_CTL_PUS_100K_UP,
};

static unsigned long pin_cfgs_100kdown_sre_fast[] = {
	PAD_CTL_PUS_100K_DOWN | PAD_CTL_SRE_FAST,
};

static unsigned long pin_cfgs_100kup_hys[] = {
	PAD_CTL_PUS_100K_UP | PAD_CTL_HYS,
};

static unsigned long pin_cfgs_sre_fast[] = {
	PAD_CTL_SRE_FAST,
};

static unsigned long pin_cfgs_sre_fast_hys[] = {
	PAD_CTL_SRE_FAST | PAD_CTL_HYS,
};

static unsigned long pin_cfgs_dse_low[] = {
	PAD_CTL_DSE_LOW,
};

static unsigned long pin_cfgs_ode[] = {
	PAD_CTL_ODE,
};

static unsigned long pin_cfgs_none[] = {
	NO_PAD_CTRL,
};

static const struct pinctrl_map armadillo_iotg_std_pinctrl_map[] = {
	/* FEC */
	PIN_MAP_MUX_GROUP_DEFAULT("imx25-fec.0", "imx25-pinctrl.0",
				  "fec_mdc__fec_mdc", "fec"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx25-fec.0", "imx25-pinctrl.0",
				  "fec_mdio__fec_mdio", "fec"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx25-fec.0", "imx25-pinctrl.0",
				  "fec_tdata0__fec_tdata0", "fec"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx25-fec.0", "imx25-pinctrl.0",
				  "fec_tdata1__fec_tdata1", "fec"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx25-fec.0", "imx25-pinctrl.0",
				  "fec_tx_en__fec_tx_en", "fec"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx25-fec.0", "imx25-pinctrl.0",
				  "fec_rdata0__fec_rdata0", "fec"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx25-fec.0", "imx25-pinctrl.0",
				  "fec_rdata1__fec_rdata1", "fec"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx25-fec.0", "imx25-pinctrl.0",
				  "fec_rx_dv__fec_rx_dv", "fec"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx25-fec.0", "imx25-pinctrl.0",
				  "fec_tx_clk__fec_tx_clk", "fec"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx25-fec.0", "imx25-pinctrl.0",
				  "upll_bypclk__gpio_3_16", "gpio3"),

	PIN_MAP_CONFIGS_PIN_DEFAULT("imx25-fec.0", "imx25-pinctrl.0",
				    "MX25_PAD_FEC_TDATA0",
				    pin_cfgs_sre_fast),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx25-fec.0", "imx25-pinctrl.0",
				    "MX25_PAD_FEC_TDATA1",
				    pin_cfgs_sre_fast),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx25-fec.0", "imx25-pinctrl.0",
				    "MX25_PAD_FEC_TX_EN",
				    pin_cfgs_sre_fast),

	/* eSDHC1 */
	PIN_MAP_MUX_GROUP_DEFAULT("sdhci-esdhc-imx25.0", "imx25-pinctrl.0",
				  "sd1_cmd__sd1_cmd", "sd1"),
	PIN_MAP_MUX_GROUP_DEFAULT("sdhci-esdhc-imx25.0", "imx25-pinctrl.0",
				  "sd1_clk__sd1_clk", "sd1"),
	PIN_MAP_MUX_GROUP_DEFAULT("sdhci-esdhc-imx25.0", "imx25-pinctrl.0",
				  "sd1_data0__sd1_data0", "sd1"),
	PIN_MAP_MUX_GROUP_DEFAULT("sdhci-esdhc-imx25.0", "imx25-pinctrl.0",
				  "sd1_data1__sd1_data1", "sd1"),
	PIN_MAP_MUX_GROUP_DEFAULT("sdhci-esdhc-imx25.0", "imx25-pinctrl.0",
				  "sd1_data2__sd1_data2", "sd1"),
	PIN_MAP_MUX_GROUP_DEFAULT("sdhci-esdhc-imx25.0", "imx25-pinctrl.0",
				  "sd1_data3__sd1_data3", "sd1"),
	PIN_MAP_MUX_GROUP_DEFAULT("sdhci-esdhc-imx25.0", "imx25-pinctrl.0",
				  "nfrb__gpio_3_31", "gpio3"),

	PIN_MAP_CONFIGS_PIN_DEFAULT("sdhci-esdhc-imx25.0", "imx25-pinctrl.0",
				    "MX25_PAD_SD1_CMD",
				    pin_cfgs_sre_fast),
	PIN_MAP_CONFIGS_PIN_DEFAULT("sdhci-esdhc-imx25.0", "imx25-pinctrl.0",
				    "MX25_PAD_SD1_CLK",
				    pin_cfgs_sre_fast),
	PIN_MAP_CONFIGS_PIN_DEFAULT("sdhci-esdhc-imx25.0", "imx25-pinctrl.0",
				    "MX25_PAD_SD1_DATA0",
				    pin_cfgs_sre_fast),
	PIN_MAP_CONFIGS_PIN_DEFAULT("sdhci-esdhc-imx25.0", "imx25-pinctrl.0",
				    "MX25_PAD_SD1_DATA1",
				    pin_cfgs_sre_fast),
	PIN_MAP_CONFIGS_PIN_DEFAULT("sdhci-esdhc-imx25.0", "imx25-pinctrl.0",
				    "MX25_PAD_SD1_DATA2",
				    pin_cfgs_sre_fast),
	PIN_MAP_CONFIGS_PIN_DEFAULT("sdhci-esdhc-imx25.0", "imx25-pinctrl.0",
				    "MX25_PAD_SD1_DATA3",
				    pin_cfgs_sre_fast),
	PIN_MAP_CONFIGS_PIN_DEFAULT("sdhci-esdhc-imx25.0", "imx25-pinctrl.0",
				    "MX25_PAD_NFRB", pin_cfgs_none),

	PIN_MAP_CONFIGS_PIN_DEFAULT("sdhci-esdhc-imx25.0", "imx25-pinctrl.0",
				    "MX25_PAD_GRP_DSE_SDHC1", pin_cfgs_dse_low),

	PIN_MAP_MUX_GROUP("sdhci-esdhc-imx25.0",
			  "state_engcm02759_workaround",
			  "imx25-pinctrl.0", "sd1_clk__gpio_2_24", "gpio2"),
	PIN_MAP_CONFIGS_PIN("sdhci-esdhc-imx25.0",
			    "state_engcm02759_workaround",
			    "imx25-pinctrl.0", "MX25_PAD_SD1_CLK",
			    pin_cfgs_100kdown_sre_fast),

	/* eSDHC2 */
	PIN_MAP_MUX_GROUP_DEFAULT("sdhci-esdhc-imx25.1", "imx25-pinctrl.0",
				  "csi_d6__sd2_cmd", "sd2"),
	PIN_MAP_MUX_GROUP_DEFAULT("sdhci-esdhc-imx25.1", "imx25-pinctrl.0",
				  "csi_d7__sd2_clk", "sd2"),
	PIN_MAP_MUX_GROUP_DEFAULT("sdhci-esdhc-imx25.1", "imx25-pinctrl.0",
				  "csi_mclk__sd2_data0", "sd2"),
	PIN_MAP_MUX_GROUP_DEFAULT("sdhci-esdhc-imx25.1", "imx25-pinctrl.0",
				  "csi_vsync__sd2_data1", "sd2"),
	PIN_MAP_MUX_GROUP_DEFAULT("sdhci-esdhc-imx25.1", "imx25-pinctrl.0",
				  "csi_hsync__sd2_data2", "sd2"),
	PIN_MAP_MUX_GROUP_DEFAULT("sdhci-esdhc-imx25.1", "imx25-pinctrl.0",
				  "csi_pixclk__sd2_data3", "sd2"),
	PIN_MAP_MUX_GROUP_DEFAULT("sdhci-esdhc-imx25.1", "imx25-pinctrl.0",
				  "clko__gpio_2_21", "gpio2"),
	PIN_MAP_MUX_GROUP_DEFAULT("sdhci-esdhc-imx25.1", "imx25-pinctrl.0",
				  "csi_d8__gpio_1_7", "gpio1"),

	PIN_MAP_CONFIGS_PIN_DEFAULT("sdhci-esdhc-imx25.1", "imx25-pinctrl.0",
				    "MX25_PAD_CSI_D6", pin_cfgs_sre_fast),
	PIN_MAP_CONFIGS_PIN_DEFAULT("sdhci-esdhc-imx25.1", "imx25-pinctrl.0",
				    "MX25_PAD_CSI_D7", pin_cfgs_sre_fast),
	PIN_MAP_CONFIGS_PIN_DEFAULT("sdhci-esdhc-imx25.1", "imx25-pinctrl.0",
				    "MX25_PAD_CSI_MCLK", pin_cfgs_sre_fast),
	PIN_MAP_CONFIGS_PIN_DEFAULT("sdhci-esdhc-imx25.1", "imx25-pinctrl.0",
				    "MX25_PAD_CSI_VSYNC", pin_cfgs_sre_fast),
	PIN_MAP_CONFIGS_PIN_DEFAULT("sdhci-esdhc-imx25.1", "imx25-pinctrl.0",
				    "MX25_PAD_CSI_HSYNC", pin_cfgs_sre_fast),
	PIN_MAP_CONFIGS_PIN_DEFAULT("sdhci-esdhc-imx25.1", "imx25-pinctrl.0",
				    "MX25_PAD_CSI_PIXCLK",
				    pin_cfgs_sre_fast_hys),
	PIN_MAP_CONFIGS_PIN_DEFAULT("sdhci-esdhc-imx25.1", "imx25-pinctrl.0",
				    "MX25_PAD_CSI_D8", pin_cfgs_100kup_hys),

	PIN_MAP_CONFIGS_PIN_DEFAULT("sdhci-esdhc-imx25.1", "imx25-pinctrl.0",
				    "MX25_PAD_GRP_DSE_CSI", pin_cfgs_dse_low),

	PIN_MAP_MUX_GROUP("sdhci-esdhc-imx25.1",
			  "state_engcm02759_workaround",
			  "imx25-pinctrl.0", "csi_d7__gpio_1_6", "gpio1"),
	PIN_MAP_CONFIGS_PIN("sdhci-esdhc-imx25.1",
			    "state_engcm02759_workaround",
			    "imx25-pinctrl.0", "MX25_PAD_CSI_D7",
			    pin_cfgs_100kdown_sre_fast),

	/* Fixed voltage regulator */
	PIN_MAP_MUX_GROUP_DEFAULT("reg-fixed-voltage.1", "imx25-pinctrl.0",
				  "nfre_b__gpio_3_27", "gpio3"),
	PIN_MAP_MUX_GROUP_DEFAULT("reg-fixed-voltage.2", "imx25-pinctrl.0",
				  "de_b__gpio_2_20", "gpio2"),
	PIN_MAP_MUX_GROUP_DEFAULT("reg-fixed-voltage.3", "imx25-pinctrl.0",
				  "nfwe_b__gpio_3_26", "gpio3"),

	/* I2C1 */
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-i2c.0", "imx25-pinctrl.0",
				  "i2c1_clk__i2c1_clk", "i2c1"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-i2c.0", "imx25-pinctrl.0",
				  "i2c1_dat__i2c1_dat", "i2c1"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-i2c.0", "imx25-pinctrl.0",
				    "MX25_PAD_I2C1_CLK", pin_cfgs_ode),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-i2c.0", "imx25-pinctrl.0",
				    "MX25_PAD_I2C1_DAT", pin_cfgs_ode),

	/* I2C-GPIO3 */
	PIN_MAP_MUX_GROUP_DEFAULT("i2c-gpio.3", "imx25-pinctrl.0",
				  "cspi1_ss1__gpio_1_17", "gpio1"),
	PIN_MAP_MUX_GROUP_DEFAULT("i2c-gpio.3", "imx25-pinctrl.0",
				  "cspi1_sclk__gpio_1_18", "gpio1"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("i2c-gpio.3", "imx25-pinctrl.0",
				    "MX25_PAD_CSPI1_SS1", pin_cfgs_100kup),
	PIN_MAP_CONFIGS_PIN_DEFAULT("i2c-gpio.3", "imx25-pinctrl.0",
				    "MX25_PAD_CSPI1_SCLK", pin_cfgs_100kup),

	/* I2C-GPIO4 */
	PIN_MAP_MUX_GROUP_DEFAULT("i2c-gpio.4", "imx25-pinctrl.0",
				  "kpp_col1__gpio_3_2", "gpio3"),
	PIN_MAP_MUX_GROUP_DEFAULT("i2c-gpio.4", "imx25-pinctrl.0",
				  "kpp_col0__gpio_3_1", "gpio3"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("i2c-gpio.4", "imx25-pinctrl.0",
				    "MX25_PAD_KPP_COL1", pin_cfgs_100kup),
	PIN_MAP_CONFIGS_PIN_DEFAULT("i2c-gpio.4", "imx25-pinctrl.0",
				    "MX25_PAD_KPP_COL0", pin_cfgs_100kup),

	/* GPIO_KEY */
	PIN_MAP_MUX_GROUP_DEFAULT("gpio-keys", "imx25-pinctrl.0",
				  "nfwp_b__gpio_3_30", "gpio3"),

	/* ADC081C021 */
	PIN_MAP_MUX_GROUP_DEFAULT("3-0054", "imx25-pinctrl.0",
				  "vstby_req__gpio_3_17", "gpio3"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("3-0054", "imx25-pinctrl.0",
				    "MX25_PAD_VSTBY_REQ", pin_cfgs_none),
};

static const struct fec_platform_data mx25_fec_pdata __initconst = {
	.phy    = PHY_INTERFACE_MODE_RMII,
	.link_led_gpio	= IMX_GPIO_NR(3, 16),
	.link_led_active_low	= true,
};

#define FEC_PHY_RST		IMX_GPIO_NR(3, 18)
static void __init armadillo_iotg_std_fec_reset(void)
{
	mxc_iomux_v3_setup_pad(MX25_PAD_VSTBY_ACK__GPIO_3_18);

	imx25_named_gpio_request(FEC_PHY_RST, "FEC PHY reset");
	gpio_direction_output(FEC_PHY_RST, 0); /* assert reset */
	/* PHY spec says 100us min and Armadillo-400 has low pass filter
	 * for PHY_RST pin (300us). */
	udelay(1000);
	gpio_set_value(FEC_PHY_RST, 1); /* deassert reset */
}

static struct imx_usb_platform_data usbh2_pdata __initdata = {
	.phy_mode = USBPHY_INTERFACE_MODE_SERIAL,
	.dr_mode = USB_DR_MODE_HOST,
};

static struct imx_usb_platform_data otg_pdata __initdata = {
	.phy_mode = USBPHY_INTERFACE_MODE_UTMI,
	.dr_mode = USB_DR_MODE_HOST,
};

static const struct imxi2c_platform_data mx25_i2c0_data __initconst = {
	.bitrate = 100000,
};

static const struct esdhc_platform_data
armadillo_iotg_std_esdhc1_pdata __initconst = {
	.cd_gpio = IMX_GPIO_NR(3, 31),
	.clk_gpio = IMX_GPIO_NR(2, 24),
	.wp_type = ESDHC_WP_NONE,
	.cd_type = ESDHC_CD_GPIO,
	.max_bus_width = 4,
	.f_max = 52000000,
	.support_vsel = false,
};

static const struct esdhc_platform_data
armadillo_iotg_std_esdhc2_pdata __initconst = {
	.wp_gpio = IMX_GPIO_NR(2, 21),
	.cd_gpio = IMX_GPIO_NR(1, 7),
	.clk_gpio = IMX_GPIO_NR(1, 6),
	.wp_type = ESDHC_WP_GPIO,
	.cd_type = ESDHC_CD_GPIO,
	.max_bus_width = 4,
	.f_max = 52000000,
	.support_vsel = false,
};

static struct i2c_gpio_platform_data i2c_gpio3_pdata= {
	.sda_pin		= IMX_GPIO_NR(1, 17),
	.sda_is_open_drain	= 0,
	.scl_pin		= IMX_GPIO_NR(1, 18),
	.scl_is_open_drain	= 0,
	.udelay			= 5,
};

static struct i2c_gpio_platform_data i2c_gpio4_pdata= {
	.sda_pin		= IMX_GPIO_NR(3, 2),
	.sda_is_open_drain	= 0,
	.scl_pin		= IMX_GPIO_NR(3, 1),
	.scl_is_open_drain	= 0,
	.udelay			= 5,
};

static struct adc081c_platform_data armadillo_iotg_std_adc081c_plat_data = {
	.alert_gpio	= IMX_GPIO_NR(3, 17),
};

static struct pca953x_platform_data armadillo_iotg_std_pca953x_plat_data = {
	.gpio_base      = IMX25_NR_GPIOS,
};

static struct i2c_board_info armadillo_iotg_i2c_gpio3[] = {
	{
		I2C_BOARD_INFO("s35390a", 0x30),
		/* irq number is run-time assigned */
	},
	{
		.type = "lm75b",
		.addr = 0x48,
	},
	{
		I2C_BOARD_INFO("adc081c", 0x54),
		.platform_data = &armadillo_iotg_std_adc081c_plat_data,
	},
	{
		.type = "pca9538",
		.platform_data = &armadillo_iotg_std_pca953x_plat_data,
	},
};

static struct gpio_keys_button armadillo_iotg_std_gpio_key_buttons[] = {
	{KEY_1, IMX_GPIO_NR(3, 30), 1, "SW1", EV_KEY, 0},
};

static struct gpio_keys_platform_data armadillo_iotg_std_gpio_key_data = {
	.buttons = armadillo_iotg_std_gpio_key_buttons,
	.nbuttons = ARRAY_SIZE(armadillo_iotg_std_gpio_key_buttons),
};

static struct gpio_keys_button armadillo_iotg_std_gpio_key_polled_buttons[] = {
	{KEY_2, EXT_GPIO_NR(0), 1, "SW2", EV_KEY, 0},
	{KEY_3, EXT_GPIO_NR(1), 1, "SW3", EV_KEY, 0},
};

static struct gpio_keys_platform_data
armadillo_iotg_std_gpio_key_polled_data = {
	.buttons = armadillo_iotg_std_gpio_key_polled_buttons,
	.nbuttons = ARRAY_SIZE(armadillo_iotg_std_gpio_key_polled_buttons),
	.poll_interval = 20,	/* msecs */
};

static struct gpio_led armadillo_iotg_std_led_pins[] = {
	{"led1",  "default-on", IMX_GPIO_NR(3, 29), 0},
	{"led2",  "default-on", IMX_GPIO_NR(3, 28), 0},
	{"led3",  "default-off", EXT_GPIO_NR(2), 0},
	{"led4",  "default-off", EXT_GPIO_NR(3), 0},
	{"yellow", NULL,         IMX_GPIO_NR(4, 30), 0},
};

static struct gpio_led_platform_data armadillo_iotg_std_led_data = {
	.leds = armadillo_iotg_std_led_pins,
	.num_leds = ARRAY_SIZE(armadillo_iotg_std_led_pins),
};

/*
 * MTD NOR Flash
 */
static struct mtd_partition armadillo_iotg_std_nor_flash_partitions[] = {
	{
		.name		= "nor.bootloader",
		.offset		= 0x00000000,
		.size		= 4 * SZ_32K,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "nor.kernel",
		.offset		= MTDPART_OFS_APPEND,
		.size		= 32 * SZ_128K,
		.mask_flags	= 0,
	}, {
		.name		= "nor.userland",
		.offset		= MTDPART_OFS_APPEND,
		.size		= 215 * SZ_128K,
		.mask_flags	= 0,
	}, {
		.name		= "nor.config",
		.offset		= MTDPART_OFS_APPEND,
		.size		= 8 * SZ_128K,
		.mask_flags	= 0,
	},
};

static const struct physmap_flash_data
		armadillo_iotg_std_nor_flash_pdata __initconst = {
	.width		= 2,
	.parts		= armadillo_iotg_std_nor_flash_partitions,
	.nr_parts	= ARRAY_SIZE(armadillo_iotg_std_nor_flash_partitions),
};

static const struct resource
armadillo_iotg_std_nor_flash_resource __initconst = {
	.flags		= IORESOURCE_MEM,
	.start		= MX25_CS0_BASE_ADDR,
	.end		= MX25_CS0_BASE_ADDR + SZ_32M - 1,
};

/*
 * Regulator
 */
static struct regulator_consumer_supply fixed3v3_consumers[] = {
	REGULATOR_SUPPLY("vref", "3-0054"),
};

static struct regulator_consumer_supply esdhc1_consumers[] = {
	REGULATOR_SUPPLY("vmmc", "sdhci-esdhc-imx25.0"),
};

static struct regulator_init_data armadillo_iotg_std_esdhc1_regulator_data = {
	.constraints	= {
		.valid_ops_mask	= REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies	= ARRAY_SIZE(esdhc1_consumers),
	.consumer_supplies	= esdhc1_consumers,
};

static struct fixed_voltage_config
armadillo_iotg_std_esdhc1_regulator_config = {
	.supply_name		= "eSDHC1 Vcc",
	.microvolts		= 3300000,
	.gpio			= IMX_GPIO_NR(3, 27),
	.enable_high		= 0,
	.enabled_at_boot	= 0,
	.init_data		= &armadillo_iotg_std_esdhc1_regulator_data,
};

static struct regulator_consumer_supply esdhc2_consumers[] = {
	REGULATOR_SUPPLY("vmmc", "sdhci-esdhc-imx25.1"),
};

static struct regulator_init_data armadillo_iotg_std_esdhc2_regulator_data = {
	.constraints	= {
		.valid_ops_mask	= REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies	= ARRAY_SIZE(esdhc2_consumers),
	.consumer_supplies	= esdhc2_consumers,
};

static struct fixed_voltage_config
armadillo_iotg_std_esdhc2_regulator_config = {
	.supply_name		= "eSDHC2 Vcc",
	.microvolts		= 3300000,
	.gpio			= IMX_GPIO_NR(2, 20),
	.startup_delay		= 115000, /* for AWL13(10ms + 1.5ms) */
	.enable_high		= 1,
	.enabled_at_boot	= 0,
	.init_data		= &armadillo_iotg_std_esdhc2_regulator_data,
};

static struct regulator_consumer_supply usb_consumers[] = {
	REGULATOR_SUPPLY("vbus", "imx27-usb.0"),
	REGULATOR_SUPPLY("vbus", "imx27-usb.1"),
};

static struct regulator_init_data armadillo_iotg_std_usb_regulator_data = {
	.constraints	= {
		.valid_ops_mask	= REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies	= ARRAY_SIZE(usb_consumers),
	.consumer_supplies	= usb_consumers,
};

static struct fixed_voltage_config
armadillo_iotg_std_usb_regulator_config = {
	.supply_name		= "USB VBUS",
	.microvolts		= 5000000,
	.gpio			= IMX_GPIO_NR(3, 26),
	.enable_high		= 0,
	.enabled_at_boot	= 1,
	.init_data		= &armadillo_iotg_std_usb_regulator_data,
};

static inline u8 armadillo_iotg_std_get_base_board_gen(void)
{
	return ((system_rev & SYSTEM_REV_BASE_BOARD_GEN_MASK) >>
		 SYSTEM_REV_BASE_BOARD_GEN_SHIFT);
}

static void __init armadillo_iotg_std_set_pca9538_addr(void)
{
	u8 base_board_gen;

	base_board_gen = armadillo_iotg_std_get_base_board_gen();
	switch (base_board_gen) {
	case SYSTEM_REV_BASE_BOARD_GEN1:
		armadillo_iotg_i2c_gpio3[3].addr = 0x70;
		break;
	case SYSTEM_REV_BASE_BOARD_GEN2:
		armadillo_iotg_i2c_gpio3[3].addr = 0x71;
		break;
	default:
		break;
	}
}

#define AIOTG_STD_RESET_N_3G_GPIO	IMX_GPIO_NR(3, 15)
#define AIOTG_STD_W_DISABLE_3G_GPIO	IMX_GPIO_NR(1, 16)
#define AIOTG_STD_PPS_3G_GPIO		IMX_GPIO_NR(1, 16)
#define AIOTG_STD_PWR_ON_3G_GPIO	EXT_GPIO_NR(6)
#define AIOTG_STD_GPIO12_3G_GPIO	EXT_GPIO_NR(7)
static void __init armadillo_iotg_std_3g_init(void)
{
	u8 base_board_gen;
	iomux_v3_cfg_t pads[] = {
		MX25_PAD_EXT_ARMCLK__GPIO_3_15, /* RESET_N_3G */
		MX25_PAD_CSPI1_SS0__GPIO_1_16, /* W_DISABLE_3G/PPS_3G */
	};

	base_board_gen = armadillo_iotg_std_get_base_board_gen();
	switch (base_board_gen) {
	case SYSTEM_REV_BASE_BOARD_GEN1:
		mxc_iomux_v3_setup_multiple_pads(pads, ARRAY_SIZE(pads));

		imx25_named_gpio_request(AIOTG_STD_RESET_N_3G_GPIO,
					 "RESET_N_3G");
		imx25_named_gpio_request(AIOTG_STD_W_DISABLE_3G_GPIO,
					 "W_DISABLE_3G");

		gpio_direction_output(AIOTG_STD_RESET_N_3G_GPIO, 0);
		mdelay(100);
		gpio_direction_output(AIOTG_STD_RESET_N_3G_GPIO, 1);
		gpio_direction_output(AIOTG_STD_W_DISABLE_3G_GPIO, 1);

		gpio_export(AIOTG_STD_RESET_N_3G_GPIO, false);
		gpio_export(AIOTG_STD_W_DISABLE_3G_GPIO, false);
		break;
	case SYSTEM_REV_BASE_BOARD_GEN2:
		mxc_iomux_v3_setup_multiple_pads(pads, ARRAY_SIZE(pads));

		imx25_named_gpio_request(AIOTG_STD_RESET_N_3G_GPIO,
					 "RESET_N_3G");
		imx25_named_gpio_request(AIOTG_STD_PPS_3G_GPIO, "PPS_3G");
		imx25_named_gpio_request(AIOTG_STD_PWR_ON_3G_GPIO, "PWR_ON_3G");
		imx25_named_gpio_request(AIOTG_STD_GPIO12_3G_GPIO, "GPIO12_3G");

		gpio_direction_output(AIOTG_STD_PWR_ON_3G_GPIO, 1);
		gpio_direction_output(AIOTG_STD_RESET_N_3G_GPIO, 0);
		mdelay(50);
		gpio_direction_output(AIOTG_STD_RESET_N_3G_GPIO, 1);
		mdelay(20);
		gpio_direction_output(AIOTG_STD_RESET_N_3G_GPIO, 0);

		gpio_direction_input(AIOTG_STD_PPS_3G_GPIO);
		gpio_direction_input(AIOTG_STD_GPIO12_3G_GPIO);

		gpio_export(AIOTG_STD_RESET_N_3G_GPIO, false);
		gpio_export(AIOTG_STD_PPS_3G_GPIO, false);
		gpio_export(AIOTG_STD_PWR_ON_3G_GPIO, false);
		gpio_export(AIOTG_STD_GPIO12_3G_GPIO, false);
		break;
	default:
		break;
	}
}

#define AIOTG_STD_SD_AWLAN_SEL		EXT_GPIO_NR(4)
static void __init armadillo_iotg_std_esdhc2_select_function(void)
{
	imx25_named_gpio_request(AIOTG_STD_SD_AWLAN_SEL, "SD_AWLAN_SEL");
	gpio_direction_output(AIOTG_STD_SD_AWLAN_SEL,
			      CONFIG_AIOTG_STD_IS_ESDHC2_SD_AWLAN);
	gpio_export(AIOTG_STD_SD_AWLAN_SEL, false);
}

#define AIOTG_STD_EXT_USB_SEL_GPIO	EXT_GPIO_NR(5)
static void __init armadillo_iotg_std_usb_select_port(void)
{
	imx25_named_gpio_request(AIOTG_STD_EXT_USB_SEL_GPIO, "EXT_USB_SEL");
	gpio_direction_output(AIOTG_STD_EXT_USB_SEL_GPIO,
			      CONFIG_AIOTG_STD_IS_USB_CON1);
	gpio_export(AIOTG_STD_EXT_USB_SEL_GPIO, false);
}

#define AIOTG_STD_TEMP_SENSOR_OS_GPIO	IMX_GPIO_NR(2, 22)
static void __init armadillo_iotg_std_temp_sensor_init(void)
{
	iomux_v3_cfg_t temp_sensor_os_pad = NEW_PAD_CTRL(MX25_PAD_CSPI1_RDY__GPIO_2_22, 0);

	mxc_iomux_v3_setup_pad(temp_sensor_os_pad);

	imx25_named_gpio_request(AIOTG_STD_TEMP_SENSOR_OS_GPIO, "TEMP_ALERT_N");
	gpio_direction_input(AIOTG_STD_TEMP_SENSOR_OS_GPIO);
	gpio_export(AIOTG_STD_TEMP_SENSOR_OS_GPIO, false);
}

#define RTC_ALM_INT	IMX_GPIO_NR(1, 15)
static void __init armadillo_iotg_std_rtc_init(void)
{
	mxc_iomux_v3_setup_pad(MX25_PAD_CSPI1_MISO__GPIO_1_15);

	gpio_request(RTC_ALM_INT, "RTC_ALM_INT");
	gpio_direction_input(RTC_ALM_INT);
	armadillo_iotg_i2c_gpio3[0].irq = gpio_to_irq(RTC_ALM_INT);
}

static void __init armadillo_iotg_std_init(void)
{
	imx25_soc_init();

	imx25_add_imx_pinctrl("imx25-pinctrl");
	pinctrl_register_mappings(armadillo_iotg_std_pinctrl_map,
				  ARRAY_SIZE(armadillo_iotg_std_pinctrl_map));

	imx25_add_imx_uart1(&uart1_pdata);

	imx25_add_usbmisc_imx();
	imx25_add_usb_phy_gen_xceiv_otg();
	imx25_add_usb_phy_gen_xceiv_hs();
	imx25_add_imx_usb_otg(&otg_pdata);
	imx25_add_imx_usb_hs(&usbh2_pdata);
	imx25_add_imx2_wdt();

	platform_device_register_resndata(NULL, "physmap-flash", -1,
			&armadillo_iotg_std_nor_flash_resource, 1,
			&armadillo_iotg_std_nor_flash_pdata,
			sizeof(armadillo_iotg_std_nor_flash_pdata));

	imx25_named_gpio_init();

	armadillo_iotg_std_fec_reset();
	imx25_add_fec(&mx25_fec_pdata);

	armadillo_iotg_std_set_pca9538_addr();
	armadillo_iotg_std_rtc_init();
	armadillo_iotg_std_temp_sensor_init();

	imx25_add_imx_i2c0(&mx25_i2c0_data);
	imx_add_i2c_gpio(3, &i2c_gpio3_pdata);
	if (IS_ENABLED(CONFIG_AIOTG_STD_ADDON_AUTO_DETECT))
		imx_add_i2c_gpio(4, &i2c_gpio4_pdata);
	i2c_register_board_info(3, armadillo_iotg_i2c_gpio3,
				ARRAY_SIZE(armadillo_iotg_i2c_gpio3));

	regulator_register_always_on(0, "adc081c Vref", fixed3v3_consumers,
				     ARRAY_SIZE(fixed3v3_consumers), 3300000);
	platform_device_register_data(NULL, "reg-fixed-voltage", 1,
				      &armadillo_iotg_std_esdhc1_regulator_config,
				      sizeof(armadillo_iotg_std_esdhc1_regulator_config));
	platform_device_register_data(NULL, "reg-fixed-voltage", 2,
				      &armadillo_iotg_std_esdhc2_regulator_config,
				      sizeof(armadillo_iotg_std_esdhc2_regulator_config));
	platform_device_register_data(NULL, "reg-fixed-voltage", 3,
				      &armadillo_iotg_std_usb_regulator_config,
				      sizeof(armadillo_iotg_std_usb_regulator_config));

	imx25_add_sdhci_esdhc_imx(0, &armadillo_iotg_std_esdhc1_pdata);
	imx25_add_sdhci_esdhc_imx(1, &armadillo_iotg_std_esdhc2_pdata);

	imx_add_gpio_keys(&armadillo_iotg_std_gpio_key_data);
	imx_add_gpio_keys_polled(&armadillo_iotg_std_gpio_key_polled_data);

	gpio_led_register_device(-1, &armadillo_iotg_std_led_data);

	if (IS_ENABLED(CONFIG_AIOTG_STD_EXTIF_OPTIONS))
		armadillo_iotg_std_extif_init();
}

#define PCA9538_NGPIO	(8)
static void __init armadillo_iotg_std_init_late(void)
{
	imx25_named_extgpio_init(PCA9538_NGPIO);
	armadillo_iotg_std_3g_init();
	armadillo_iotg_std_esdhc2_select_function();
	armadillo_iotg_std_usb_select_port();
}

static void __init armadillo_iotg_std_timer_init(void)
{
	mx25_clocks_init();
}

MACHINE_START(ARMADILLO410, "Armadillo-410")
	/* Maintainer: Atmark Techno, Inc.  */
	.atag_offset	= 0x100,
	.map_io		= mx25_map_io,
	.init_early	= imx25_init_early,
	.init_irq	= mx25_init_irq,
	.handle_irq	= imx25_handle_irq,
	.init_time	= armadillo_iotg_std_timer_init,
	.init_machine	= armadillo_iotg_std_init,
	.init_late	= armadillo_iotg_std_init_late,
	.restart	= mxc_restart,
MACHINE_END
