/*
 * Copyright (C) 2015 Atmark Techno, Inc. All Rights Reserved.
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
#include <linux/gpio.h>
#include <linux/pinctrl/machine.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>
#include <asm/memory.h>
#include <asm/mach/map.h>

#include "common.h"
#include "devices-imx25.h"
#include "hardware.h"
#include "iomux-mx25.h"
#include "mx25.h"
#include "imx25-named_gpio.h"

static const struct imxuart_platform_data uart_pdata __initconst = {
	/* .flags = IMXUART_HAVE_RTSCTS, */
};

static unsigned long pin_cfgs_100kdown[] = {
	PAD_CTL_PUS_100K_DOWN,
};

static unsigned long pin_cfgs_100kup[] = {
	PAD_CTL_PUS_100K_UP,
};

static unsigned long pin_cfgs_none[] = {
	NO_PAD_CTRL,
};

static unsigned long pin_cfgs_100kup_ode[] = {
	PAD_CTL_PUS_100K_UP | PAD_CTL_ODE,
};

static unsigned long pin_cfgs_22kup[] = {
	PAD_CTL_PUS_22K_UP,
};

static const struct pinctrl_map armadillo_iotg_std_extif_uart_map[] = {
	/* uart1 */
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-uart.0", "imx25-pinctrl.0",
				  "uart1_rxd__uart1_rxd", "uart1"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-uart.0", "imx25-pinctrl.0",
				  "uart1_txd__uart1_txd", "uart1"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-uart.0", "imx25-pinctrl.0",
				    "MX25_PAD_UART1_RXD", pin_cfgs_100kup),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-uart.0", "imx25-pinctrl.0",
				    "MX25_PAD_UART1_TXD", pin_cfgs_none),
#if defined(CONFIG_AIOTG_STD_UART1_HW_FLOW)
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-uart.0", "imx25-pinctrl.0",
				  "uart1_rts__uart1_rts", "uart1"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-uart.0", "imx25-pinctrl.0",
				  "uart1_cts__uart1_cts", "uart1"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-uart.0", "imx25-pinctrl.0",
				    "MX25_PAD_UART1_RTS", pin_cfgs_100kup),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-uart.0", "imx25-pinctrl.0",
				    "MX25_PAD_UART1_CTS", pin_cfgs_none),
#endif

	/* uart3 */
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-uart.2", "imx25-pinctrl.0",
				  "kpp_row0__uart3_rxd", "uart3"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-uart.2", "imx25-pinctrl.0",
				  "kpp_row1__uart3_txd", "uart3"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-uart.2", "imx25-pinctrl.0",
				    "MX25_PAD_KPP_ROW0", pin_cfgs_100kup),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-uart.2", "imx25-pinctrl.0",
				    "MX25_PAD_KPP_ROW1", pin_cfgs_none),
#if defined(CONFIG_AIOTG_STD_UART3_HW_FLOW)
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-uart.2", "imx25-pinctrl.0",
				  "kpp_row2__uart3_rts", "uart3"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-uart.2", "imx25-pinctrl.0",
				  "kpp_row3__uart3_cts", "uart3"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-uart.2", "imx25-pinctrl.0",
				    "MX25_PAD_KPP_ROW2", pin_cfgs_100kup),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-uart.2", "imx25-pinctrl.0",
				    "MX25_PAD_KPP_ROW3", pin_cfgs_none),
#endif

	/* uart4 KPP */
#if defined(CONFIG_AIOTG_STD_UART4_KPP)
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-uart.3", "imx25-pinctrl.0",
				  "kpp_col0__uart4_rxd_mux", "uart4"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-uart.3", "imx25-pinctrl.0",
				  "kpp_col1__uart4_txd_mux", "uart4"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-uart.3", "imx25-pinctrl.0",
				    "MX25_PAD_KPP_COL0", pin_cfgs_100kup),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-uart.3", "imx25-pinctrl.0",
				    "MX25_PAD_KPP_COL1", pin_cfgs_none),
#endif
#if defined(CONFIG_AIOTG_STD_UART4_HW_FLOW_KPP)
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-uart.3", "imx25-pinctrl.0",
				  "kpp_col2__uart4_rts", "uart4"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-uart.3", "imx25-pinctrl.0",
				  "kpp_col3__uart4_cts", "uart4"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-uart.3", "imx25-pinctrl.0",
				    "MX25_PAD_KPP_COL2", pin_cfgs_100kup),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-uart.3", "imx25-pinctrl.0",
				    "MX25_PAD_KPP_COL3", pin_cfgs_none),
#endif

	/* uart4 LCD */
#if defined(CONFIG_AIOTG_STD_UART4_LCD)
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-uart.3", "imx25-pinctrl.0",
				  "ld8__uart4_rxd", "uart4"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-uart.3", "imx25-pinctrl.0",
				  "ld9__uart4_txd", "uart4"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-uart.3", "imx25-pinctrl.0",
				    "MX25_PAD_LD8", pin_cfgs_100kup),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-uart.3", "imx25-pinctrl.0",
				    "MX25_PAD_LD9", pin_cfgs_none),
#endif
#if defined(CONFIG_AIOTG_STD_UART4_HW_FLOW_LCD)
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-uart.3", "imx25-pinctrl.0",
				  "ld10__uart4_rts", "uart4"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-uart.3", "imx25-pinctrl.0",
				  "ld11__uart4_cts", "uart4"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-uart.3", "imx25-pinctrl.0",
				    "MX25_PAD_LD10", pin_cfgs_100kup),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-uart.3", "imx25-pinctrl.0",
				    "MX25_PAD_LD11", pin_cfgs_none),
#endif

	/* uart5 */
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-uart.4", "imx25-pinctrl.0",
				  "csi_d2__uart5_rxd_mux", "uart5"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-uart.4", "imx25-pinctrl.0",
				  "csi_d3__uart5_txd_mux", "uart5"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-uart.4", "imx25-pinctrl.0",
				    "MX25_PAD_CSI_D2", pin_cfgs_100kup),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-uart.4", "imx25-pinctrl.0",
				    "MX25_PAD_CSI_D3", pin_cfgs_none),
#if defined(CONFIG_AIOTG_STD_UART5_HW_FLOW)
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-uart.4", "imx25-pinctrl.0",
				  "csi_d4__uart5_rts", "uart5"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-uart.4", "imx25-pinctrl.0",
				  "csi_d5__uart5_cts", "uart5"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-uart.4", "imx25-pinctrl.0",
				    "MX25_PAD_CSI_D4", pin_cfgs_100kup),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-uart.4", "imx25-pinctrl.0",
				    "MX25_PAD_CSI_D5", pin_cfgs_none),
#endif
};

static const struct pinctrl_map armadillo_iotg_std_extif_i2c_map[] = {
	/* i2c2 */
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-i2c.1", "imx25-pinctrl.0",
				  "gpio_c__i2c2_clk", "i2c2"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-i2c.1", "imx25-pinctrl.0",
				  "gpio_d__i2c2_dat", "i2c2"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-i2c.1", "imx25-pinctrl.0",
				    "MX25_PAD_GPIO_C", pin_cfgs_100kup_ode),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-i2c.1", "imx25-pinctrl.0",
				    "MX25_PAD_GPIO_D", pin_cfgs_100kup_ode),

	/* i2c3 gpio pin*/
#if defined(CONFIG_AIOTG_STD_I2C3_CON1_24_25)
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-i2c.2", "imx25-pinctrl.0",
				  "gpio_a__i2c3_clk", "i2c3"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-i2c.2", "imx25-pinctrl.0",
				  "gpio_b__i2c3_dat", "i2c3"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-i2c.2", "imx25-pinctrl.0",
				    "MX25_PAD_GPIO_A", pin_cfgs_100kup_ode),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-i2c.2", "imx25-pinctrl.0",
				    "MX25_PAD_GPIO_B", pin_cfgs_100kup_ode),
#endif
	/* i2c3 LCD pin */
#if defined(CONFIG_AIOTG_STD_I2C3_CON1_51_52)
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-i2c.2", "imx25-pinctrl.0",
				  "hsync__i2c3_clk", "i2c3"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx21-i2c.2", "imx25-pinctrl.0",
				  "vsync__i2c3_dat", "i2c3"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-i2c.2", "imx25-pinctrl.0",
				    "MX25_PAD_HSYNC", pin_cfgs_100kup_ode),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx21-i2c.2", "imx25-pinctrl.0",
				    "MX25_PAD_VSYNC", pin_cfgs_100kup_ode),
#endif
};

static const struct imxi2c_platform_data mx25_i2c1_data __initconst = {
	.bitrate = 100000,
};

static const struct imxi2c_platform_data mx25_i2c2_data __initconst = {
	.bitrate = 100000,
};

static struct i2c_board_info armadillo_iotg_std_extif_i2c1[] = {
};

static struct i2c_board_info armadillo_iotg_std_extif_i2c2[] = {
};

static const struct pinctrl_map armadillo_iotg_std_extif_pwm_map[] = {
	/* pwmo1 */
	PIN_MAP_MUX_GROUP_DEFAULT("imx27-pwm.0", "imx25-pinctrl.0",
				  "pwm__pwm", "pwm1"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx27-pwm.0", "imx25-pinctrl.0",
				    "MX25_PAD_PWM", pin_cfgs_100kup),

	/* pwmo2 */
	PIN_MAP_MUX_GROUP_DEFAULT("imx27-pwm.1", "imx25-pinctrl.0",
				  "gpio_a__pwm2_pwmo", "pwm2"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx27-pwm.1", "imx25-pinctrl.0",
				    "MX25_PAD_GPIO_A", pin_cfgs_100kup),

	/* pwmo3 */
	PIN_MAP_MUX_GROUP_DEFAULT("imx27-pwm.2", "imx25-pinctrl.0",
				  "gpio_b__pwm3_pwmo", "pwm3"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx27-pwm.2", "imx25-pinctrl.0",
				    "MX25_PAD_GPIO_B", pin_cfgs_100kup),

	/* pwmo4 */
	PIN_MAP_MUX_GROUP_DEFAULT("imx27-pwm.3", "imx25-pinctrl.0",
				  "gpio_c__pwm4_pwmo", "pwm4"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx27-pwm.3", "imx25-pinctrl.0",
				    "MX25_PAD_GPIO_C", pin_cfgs_100kup),
};

static const struct pinctrl_map armadillo_iotg_std_extif_spi_map[] = {
	/* cspi2 */
	PIN_MAP_MUX_GROUP_DEFAULT("imx35-cspi.1", "imx25-pinctrl.0",
				  "ld12__cspi2_mosi", "cspi2"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx35-cspi.1", "imx25-pinctrl.0",
				  "ld13__cspi2_miso", "cspi2"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx35-cspi.1", "imx25-pinctrl.0",
				  "ld14__cspi2_sclk", "cspi2"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx35-cspi.1", "imx25-pinctrl.0",
				  "ld15__cspi2_rdy", "cspi2"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx35-cspi.1", "imx25-pinctrl.0",
				    "MX25_PAD_LD12", pin_cfgs_100kup),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx35-cspi.1", "imx25-pinctrl.0",
				    "MX25_PAD_LD13", pin_cfgs_100kup),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx35-cspi.1", "imx25-pinctrl.0",
				    "MX25_PAD_LD14", pin_cfgs_100kup),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx35-cspi.1", "imx25-pinctrl.0",
				    "MX25_PAD_LD15", pin_cfgs_100kup),
#if defined(CONFIG_AIOTG_STD_SPI2_SS0)
	PIN_MAP_MUX_GROUP_DEFAULT("imx35-cspi.1", "imx25-pinctrl.0",
				  "oe_acd__gpio_1_25", "gpio1"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx35-cspi.1", "imx25-pinctrl.0",
				    "MX25_PAD_OE_ACD", pin_cfgs_100kup),
#endif
#if defined(CONFIG_AIOTG_STD_SPI2_SS1)
	PIN_MAP_MUX_GROUP_DEFAULT("imx35-cspi.1", "imx25-pinctrl.0",
				  "gpio_c__gpio_c", "gpio1"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx35-cspi.1", "imx25-pinctrl.0",
				    "MX25_PAD_GPIO_C", pin_cfgs_100kup),
#endif

	/* cspi3 */
	PIN_MAP_MUX_GROUP_DEFAULT("imx35-cspi.2", "imx25-pinctrl.0",
				  "csi_d2__cspi3_mosi", "cspi3"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx35-cspi.2", "imx25-pinctrl.0",
				  "csi_d3__cspi3_miso", "cspi3"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx35-cspi.2", "imx25-pinctrl.0",
				  "csi_d4__cspi3_sclk", "cspi3"),
	PIN_MAP_MUX_GROUP_DEFAULT("imx35-cspi.2", "imx25-pinctrl.0",
				  "csi_d5__cspi3_rdy", "cspi3"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx35-cspi.2", "imx25-pinctrl.0",
				    "MX25_PAD_D2", pin_cfgs_100kup),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx35-cspi.2", "imx25-pinctrl.0",
				    "MX25_PAD_D3", pin_cfgs_100kup),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx35-cspi.2", "imx25-pinctrl.0",
				    "MX25_PAD_D4", pin_cfgs_100kup),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx35-cspi.2", "imx25-pinctrl.0",
				    "MX25_PAD_D5", pin_cfgs_100kup),
#if defined(CONFIG_AIOTG_STD_SPI3_SS0)
	PIN_MAP_MUX_GROUP_DEFAULT("imx35-cspi.2", "imx25-pinctrl.0",
				  "gpio_d__gpio_d", "gpio1"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx35-cspi.2", "imx25-pinctrl.0",
				    "MX25_PAD_GPIO_D", pin_cfgs_100kup),
#endif
#if defined(CONFIG_AIOTG_STD_SPI3_SS1)
	PIN_MAP_MUX_GROUP_DEFAULT("imx35-cspi.2", "imx25-pinctrl.0",
				  "csi_d9__gpio_4_21", "gpio4"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("imx35-cspi.2", "imx25-pinctrl.0",
				    "MX25_PAD_CSI_D9", pin_cfgs_100kup),
#endif
};

static const struct pinctrl_map armadillo_iotg_std_extif_can_map[] = {
	/* can1 */
	PIN_MAP_MUX_GROUP_DEFAULT("flexcan.0", "imx25-pinctrl.0",
				  "gpio_a__can1_tx", "can1"),
	PIN_MAP_MUX_GROUP_DEFAULT("flexcan.0", "imx25-pinctrl.0",
				  "gpio_b__can1_rx", "can1"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("flexcan.0", "imx25-pinctrl.0",
				    "MX25_PAD_GPIO_A", pin_cfgs_none),
	PIN_MAP_CONFIGS_PIN_DEFAULT("flexcan.0", "imx25-pinctrl.0",
				    "MX25_PAD_GPIO_B", pin_cfgs_100kup),
	/* can2 */
	PIN_MAP_MUX_GROUP_DEFAULT("flexcan.1", "imx25-pinctrl.0",
				  "gpio_c__can2_tx", "can2"),
	PIN_MAP_MUX_GROUP_DEFAULT("flexcan.1", "imx25-pinctrl.0",
				  "gpio_d__can2_rx", "can2"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("flexcan.1", "imx25-pinctrl.0",
				    "MX25_PAD_GPIO_C", pin_cfgs_none),
	PIN_MAP_CONFIGS_PIN_DEFAULT("flexcan.1", "imx25-pinctrl.0",
				    "MX25_PAD_GPIO_D", pin_cfgs_100kup),
};

static int spi1_cs[] = {
#if defined(CONFIG_AIOTG_STD_SPI2_SS0)
	IMX_GPIO_NR(1, 25), /* SS0 */
#endif
#if defined(CONFIG_AIOTG_STD_SPI2_SS1)
	IMX_GPIO_NR(1, 2), /* SS1 */
#endif
};

static int spi2_cs[] = {
#if defined(CONFIG_AIOTG_STD_SPI3_SS0)
	IMX_GPIO_NR(1, 3), /* SS0 */
#endif
#if defined(CONFIG_AIOTG_STD_SPI3_SS1)
	IMX_GPIO_NR(4, 21), /* SS0 */
#endif
};

static const struct spi_imx_master spi1_pdata __initconst = {
	.chipselect	= spi1_cs,
	.num_chipselect	= ARRAY_SIZE(spi1_cs),
};

static const struct spi_imx_master spi2_pdata __initconst = {
	.chipselect	= spi2_cs,
	.num_chipselect	= ARRAY_SIZE(spi2_cs),
};

static struct spi_board_info armadillo_iotg_std_spi1_board_info[] __initdata = {
};

static struct spi_board_info armadillo_iotg_std_spi2_board_info[] __initdata = {
};

static const struct pinctrl_map armadillo_iotg_std_extif_w1_map[] = {
	PIN_MAP_MUX_GROUP_DEFAULT("mxc_w1.0", "imx25-pinctrl.0",
				  "rtck__owire", "owire"),
	PIN_MAP_CONFIGS_PIN_DEFAULT("mxc_w1.0", "imx25-pinctrl.0",
				    "MX25_PAD_RTCK", pin_cfgs_22kup),
};

static const struct pinctrl_map armadillo_iotg_std_extif_gpio_map[] = {
	/* CON1_3, CON2_24 */
#if defined(CONFIG_AIOTG_STD_CON1_3_CON2_24_GPIO1_2)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "gpio_c__gpio_c", "gpio1"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_GPIO_C", pin_cfgs_100kdown),
#endif

	/* CON1_4, CON2_25 */
#if defined(CONFIG_AIOTG_STD_CON1_4_CON2_25_GPIO1_3)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "gpio_d__gpio_d", "gpio1"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_GPIO_D", pin_cfgs_100kdown),
#endif

	/* CON1_5, CON2_33 */
#if defined(CONFIG_AIOTG_STD_CON1_5_CON2_33_GPIO1_26)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "pwm__gpio_1_26", "gpio1"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_PWM", pin_cfgs_none),
#endif

	/* CON1_6, CON2_32 */
#if defined(CONFIG_AIOTG_STD_CON1_6_CON2_32_GPIO3_14)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "rtck__gpio_3_14", "gpio3"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_RTCK", pin_cfgs_100kdown),
#endif

	/* CON1_7, CON2_41 */
#if defined(CONFIG_AIOTG_STD_CON1_7_CON2_41_GPIO4_22)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "uart1_rxd__gpio_4_22", "gpio4"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_UART1_RXD",
					pin_cfgs_100kdown),
#endif

	/* CON1_8, CON2_40 */
#if defined(CONFIG_AIOTG_STD_CON1_8_CON2_40_GPIO4_23)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "uart1_txd__gpio_4_23", "gpio4"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_UART1_TXD",
					pin_cfgs_100kdown),
#endif

	/* CON1_9, CON2_7, CON2_39 */
#if defined(CONFIG_AIOTG_STD_CON1_9_CON2_7_39_GPIO4_24)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "uart1_rts__gpio_4_24", "gpio4"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_UART1_RTS",
					pin_cfgs_100kdown),
#endif

	/* CON1_10, CON2_8, CON2_38 */
#if defined(CONFIG_AIOTG_STD_CON1_10_CON2_8_38_GPIO4_25)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "uart1_cts__gpio_4_25", "gpio4"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_UART1_CTS",
					pin_cfgs_100kdown),
#endif

	/* CON1_11, CON2_50 */
#if defined(CONFIG_AIOTG_STD_CON1_11_CON2_50_GPIO4_21)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "csi_d9__gpio_4_21", "gpio4"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_CSI_D9", pin_cfgs_100kdown),
#endif

	/* CON1_12, CON2_16, CON2_37 */
#if defined(CONFIG_AIOTG_STD_CON1_12_CON2_16_37_GPIO1_27)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "csi_d2__gpio_1_27", "gpio4"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_CSI_D2", pin_cfgs_100kdown),
#endif

	/* CON1_13, CON2_17, CON2_36 */
#if defined(CONFIG_AIOTG_STD_CON1_13_CON2_17_36_GPIO1_28)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "csi_d3__gpio_1_28", "gpio1"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_CSI_D3", pin_cfgs_100kdown),
#endif

	/* CON1_14, CON2_12, CON2_18, CON2_35 */
#if defined(CONFIG_AIOTG_STD_CON1_14_CON2_12_18_35_GPIO1_29)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "csi_d4__gpio_1_29", "gpio1"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_CSI_D4", pin_cfgs_100kdown),
#endif

	/* CON1_15, CON2_13, CON2_19, CON2_34 */
#if defined(CONFIG_AIOTG_STD_CON1_15_CON2_13_19_34_GPIO1_30)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "csi_d5__gpio_1_30", "gpio1"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_CSI_D5", pin_cfgs_100kdown),
#endif

	/* CON1_16, CON2_49 */
#if defined(CONFIG_AIOTG_STD_CON1_16_CON2_49_GPIO2_29)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "kpp_row0__gpio_2_29", "gpio2"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_KPP_ROW0", pin_cfgs_100kdown),
#endif

	/* CON1_17, CON2_48 */
#if defined(CONFIG_AIOTG_STD_CON1_17_CON2_48_GPIO2_30)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "kpp_row1__gpio_2_30", "gpio2"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_KPP_ROW1", pin_cfgs_100kdown),
#endif

	/* CON1_18, CON2_47 */
#if defined(CONFIG_AIOTG_STD_CON1_18_CON2_47_GPIO2_31)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "kpp_row2__gpio_2_31", "gpio2"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_KPP_ROW2", pin_cfgs_100kdown),
#endif

	/* CON1_19, CON2_46 */
#if defined(CONFIG_AIOTG_STD_CON1_19_CON2_46_GPIO3_0)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "kpp_row3__gpio_3_0", "gpio3"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_KPP_ROW3", pin_cfgs_100kdown),
#endif

	/* CON1_20, CON2_20, CON2_45 */
#if defined(CONFIG_AIOTG_STD_CON1_20_CON2_20_45_GPIO3_1)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "kpp_col0__gpio_3_1", "gpio3"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_KPP_COL0", pin_cfgs_100kdown),
#endif

	/* CON1_21, CON2_21, CON2_44 */
#if defined(CONFIG_AIOTG_STD_CON1_21_CON2_21_44_GPIO3_2)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "kpp_col1__gpio_3_2", "gpio3"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_KPP_COL1", pin_cfgs_100kdown),
#endif

	/* CON1_22, CON2_22, CON2_43 */
#if defined(CONFIG_AIOTG_STD_CON1_22_CON2_22_43_GPIO3_3)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "kpp_col2__gpio_3_3", "gpio3"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_KPP_COL2", pin_cfgs_100kdown),
#endif

	/* CON1_23, CON2_23, CON2_42 */
#if defined(CONFIG_AIOTG_STD_CON1_23_CON2_23_42_GPIO3_4)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "kpp_col3__gpio_3_4", "gpio3"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_KPP_COL3", pin_cfgs_100kdown),
#endif

	/* CON1_24 */
#if defined(CONFIG_AIOTG_STD_CON1_24_GPIO1_0)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "gpio_a__gpio_a", "gpio1"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_GPIO_A", pin_cfgs_100kdown),
#endif

	/* CON1_25 */
#if defined(CONFIG_AIOTG_STD_CON1_25_GPIO1_1)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "gpio_b__gpio_b", "gpio1"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_GPIO_B", pin_cfgs_100kdown),
#endif

	/* CON1_32 */
#if defined(CONFIG_AIOTG_STD_CON1_32_GPIO1_5)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "gpio_f__gpio_f", "gpio1"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_GPIO_F", pin_cfgs_none),
#endif

	/* CON1_33 */
#if defined(CONFIG_AIOTG_STD_CON1_33_GPIO1_4)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "gpio_e__gpio_e", "gpio1"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_GPIO_E", pin_cfgs_none),
#endif

	/* CON1_42 */
#if defined(CONFIG_AIOTG_STD_CON1_42_GPIO1_21)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "ld7__gpio_1_21", "gpio1"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_LD7", pin_cfgs_none),
#endif

	/* CON1_43 */
#if defined(CONFIG_AIOTG_STD_CON1_43_GPIO1_20)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "ld6__gpio_1_20", "gpio1"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_LD6", pin_cfgs_none),
#endif

	/* CON1_44 */
#if defined(CONFIG_AIOTG_STD_CON1_44_GPIO1_19)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "ld5__gpio_1_19", "gpio1"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_LD5", pin_cfgs_none),
#endif

	/* CON1_45 */
#if defined(CONFIG_AIOTG_STD_CON1_45_GPIO2_19)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "ld4__gpio_2_19", "gpio2"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_LD4", pin_cfgs_none),
#endif

	/* CON1_46 */
#if defined(CONFIG_AIOTG_STD_CON1_46_GPIO2_18)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "ld3__gpio_2_18", "gpio2"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_LD3", pin_cfgs_none),
#endif

	/* CON1_47 */
#if defined(CONFIG_AIOTG_STD_CON1_47_GPIO2_17)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "ld2__gpio_2_17", "gpio2"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_LD2", pin_cfgs_none),
#endif

	/* CON1_48 */
#if defined(CONFIG_AIOTG_STD_CON1_48_GPIO2_16)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "ld1__gpio_2_16", "gpio2"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_LD1", pin_cfgs_none),
#endif

	/* CON1_49 */
#if defined(CONFIG_AIOTG_STD_CON1_49_GPIO2_15)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "ld0__gpio_2_15", "gpio2"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_LD0", pin_cfgs_none),
#endif

	/* CON1_50 */
#if defined(CONFIG_AIOTG_STD_CON1_50_GPIO1_25)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "oe_acd__gpio_1_25", "gpio1"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_OE_ACD", pin_cfgs_none),
#endif

	/* CON1_51 */
#if defined(CONFIG_AIOTG_STD_CON1_51_GPIO1_23)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "vsync__gpio_1_23", "gpio1"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_VSYNC", pin_cfgs_none),
#endif

	/* CON1_52 */
#if defined(CONFIG_AIOTG_STD_CON1_52_GPIO1_22)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "hsync__gpio_1_22", "gpio1"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_HSYNC", pin_cfgs_none),
#endif

	/* CON1_53 */
#if defined(CONFIG_AIOTG_STD_CON1_53_GPIO1_24)
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("imx25-pinctrl.0",
				      "lsclk__gpio_1_24", "gpio1"),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("imx25-pinctrl.0",
					"MX25_PAD_LSCLK", pin_cfgs_none),
#endif
};

struct aiotg_extif_gpio {
	int gpio;
	const char *name;
};

static const struct aiotg_extif_gpio aiotg_extif_gpios[] = {
	/* CON1_3, CON2_24 */
#if defined(CONFIG_AIOTG_STD_CON1_3_CON2_24_GPIO1_2)
	{ IMX_GPIO_NR(1, 2), "CON1_3" },
#endif

	/* CON1_4, CON2_25 */
#if defined(CONFIG_AIOTG_STD_CON1_4_CON2_25_GPIO1_3)
	{ IMX_GPIO_NR(1, 3), "CON1_4" },
#endif

	/* CON1_5, CON2_33 */
#if defined(CONFIG_AIOTG_STD_CON1_5_CON2_33_GPIO1_26)
	{ IMX_GPIO_NR(1, 26), "CON1_5" },
#endif

	/* CON1_6, CON2_32 */
#if defined(CONFIG_AIOTG_STD_CON1_6_CON2_32_GPIO3_14)
	{ IMX_GPIO_NR(3, 14), "CON1_6" },
#endif

	/* CON1_7, CON2_41 */
#if defined(CONFIG_AIOTG_STD_CON1_7_CON2_41_GPIO4_22)
	{ IMX_GPIO_NR(4, 22), "CON1_7" },
#endif

	/* CON1_8, CON2_40 */
#if defined(CONFIG_AIOTG_STD_CON1_8_CON2_40_GPIO4_23)
	{ IMX_GPIO_NR(4, 23), "CON1_8" },
#endif

	/* CON1_9, CON2_7, CON2_39 */
#if defined(CONFIG_AIOTG_STD_CON1_9_CON2_7_39_GPIO4_24)
	{ IMX_GPIO_NR(4, 24), "CON1_9" },
#endif

	/* CON1_10, CON2_8, CON2_38 */
#if defined(CONFIG_AIOTG_STD_CON1_10_CON2_8_38_GPIO4_25)
	{ IMX_GPIO_NR(4, 25), "CON1_10" },
#endif

	/* CON1_11, CON2_50 */
#if defined(CONFIG_AIOTG_STD_CON1_11_CON2_50_GPIO4_21)
	{ IMX_GPIO_NR(4, 21), "CON1_11" },
#endif

	/* CON1_12, CON2_16, CON2_37 */
#if defined(CONFIG_AIOTG_STD_CON1_12_CON2_16_37_GPIO1_27)
	{ IMX_GPIO_NR(1, 27), "CON1_12" },
#endif

	/* CON1_13, CON2_17, CON2_36 */
#if defined(CONFIG_AIOTG_STD_CON1_13_CON2_17_36_GPIO1_28)
	{ IMX_GPIO_NR(1, 28), "CON1_13" },
#endif

	/* CON1_14, CON2_12, CON2_18, CON2_35 */
#if defined(CONFIG_AIOTG_STD_CON1_14_CON2_12_18_35_GPIO1_29)
	{ IMX_GPIO_NR(1, 29), "CON1_14" },
#endif

	/* CON1_15, CON2_13, CON2_19, CON2_34 */
#if defined(CONFIG_AIOTG_STD_CON1_15_CON2_13_19_34_GPIO1_30)
	{ IMX_GPIO_NR(1, 30), "CON1_15" },
#endif

	/* CON1_16, CON2_49 */
#if defined(CONFIG_AIOTG_STD_CON1_16_CON2_49_GPIO2_29)
	{ IMX_GPIO_NR(2, 29), "CON1_16" },
#endif

	/* CON1_17, CON2_48 */
#if defined(CONFIG_AIOTG_STD_CON1_17_CON2_48_GPIO2_30)
	{ IMX_GPIO_NR(2, 30), "CON1_17" },
#endif

	/* CON1_18, CON2_47 */
#if defined(CONFIG_AIOTG_STD_CON1_18_CON2_47_GPIO2_31)
	{ IMX_GPIO_NR(2, 31), "CON1_18" },
#endif

	/* CON1_19, CON2_46 */
#if defined(CONFIG_AIOTG_STD_CON1_19_CON2_46_GPIO3_0)
	{ IMX_GPIO_NR(3, 0), "CON1_19" },
#endif

	/* CON1_20, CON2_20, CON2_45 */
#if defined(CONFIG_AIOTG_STD_CON1_20_CON2_20_45_GPIO3_1)
	{ IMX_GPIO_NR(3, 1), "CON1_20" },
#endif

	/* CON1_21, CON2_21, CON2_44 */
#if defined(CONFIG_AIOTG_STD_CON1_21_CON2_21_44_GPIO3_2)
	{ IMX_GPIO_NR(3, 2), "CON1_21" },
#endif

	/* CON1_22, CON2_22, CON2_43 */
#if defined(CONFIG_AIOTG_STD_CON1_22_CON2_22_43_GPIO3_3)
	{ IMX_GPIO_NR(3, 3), "CON1_22" },
#endif

	/* CON1_23, CON2_23, CON2_42 */
#if defined(CONFIG_AIOTG_STD_CON1_23_CON2_23_42_GPIO3_4)
	{ IMX_GPIO_NR(3, 4), "CON1_23" },
#endif

	/* CON1_24 */
#if defined(CONFIG_AIOTG_STD_CON1_24_GPIO1_0)
	{ IMX_GPIO_NR(1, 0), "CON1_24" },
#endif

	/* CON1_25 */
#if defined(CONFIG_AIOTG_STD_CON1_25_GPIO1_1)
	{ IMX_GPIO_NR(1, 1), "CON1_25" },
#endif

	/* CON1_32 */
#if defined(CONFIG_AIOTG_STD_CON1_32_GPIO1_5)
	{ IMX_GPIO_NR(1, 5), "CON1_32" },
#endif

	/* CON1_33 */
#if defined(CONFIG_AIOTG_STD_CON1_33_GPIO1_4)
	{ IMX_GPIO_NR(1, 4), "CON1_33" },
#endif

	/* CON1_42 */
#if defined(CONFIG_AIOTG_STD_CON1_42_GPIO1_21)
	{ IMX_GPIO_NR(1, 21), "CON1_42" },
#endif

	/* CON1_43 */
#if defined(CONFIG_AIOTG_STD_CON1_43_GPIO1_20)
	{ IMX_GPIO_NR(1, 20), "CON1_43" },
#endif

	/* CON1_44 */
#if defined(CONFIG_AIOTG_STD_CON1_44_GPIO1_19)
	{ IMX_GPIO_NR(1, 19), "CON1_44" },
#endif

	/* CON1_45 */
#if defined(CONFIG_AIOTG_STD_CON1_45_GPIO2_19)
	{ IMX_GPIO_NR(2, 19), "CON1_45" },
#endif

	/* CON1_46 */
#if defined(CONFIG_AIOTG_STD_CON1_46_GPIO2_18)
	{ IMX_GPIO_NR(2, 18), "CON1_46" },
#endif

	/* CON1_47 */
#if defined(CONFIG_AIOTG_STD_CON1_47_GPIO2_17)
	{ IMX_GPIO_NR(2, 17), "CON1_47" },
#endif

	/* CON1_48 */
#if defined(CONFIG_AIOTG_STD_CON1_48_GPIO2_16)
	{ IMX_GPIO_NR(2, 16), "CON1_48" },
#endif

	/* CON1_49 */
#if defined(CONFIG_AIOTG_STD_CON1_49_GPIO2_15)
	{ IMX_GPIO_NR(2, 15), "CON1_49" },
#endif

	/* CON1_50 */
#if defined(CONFIG_AIOTG_STD_CON1_50_GPIO1_25)
	{ IMX_GPIO_NR(1, 25), "CON1_50" },
#endif

	/* CON1_51 */
#if defined(CONFIG_AIOTG_STD_CON1_51_GPIO1_23)
	{ IMX_GPIO_NR(1, 23), "CON1_51" },
#endif

	/* CON1_52 */
#if defined(CONFIG_AIOTG_STD_CON1_52_GPIO1_22)
	{ IMX_GPIO_NR(1, 22), "CON1_52" },
#endif

	/* CON1_53 */
#if defined(CONFIG_AIOTG_STD_CON1_53_GPIO1_24)
	{ IMX_GPIO_NR(1, 24), "CON1_53" },
#endif
};

static void armadillo_iotg_std_exitif_set_gpio(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(aiotg_extif_gpios); i++) {
		imx25_named_gpio_request(aiotg_extif_gpios[i].gpio,
					 aiotg_extif_gpios[i].name);
		gpio_direction_input(aiotg_extif_gpios[i].gpio);
		gpio_export(aiotg_extif_gpios[i].gpio, true);
	}
}

void __init armadillo_iotg_std_extif_init(void)
{
	pinctrl_register_mappings(armadillo_iotg_std_extif_uart_map,
				  ARRAY_SIZE(armadillo_iotg_std_extif_uart_map));

	pinctrl_register_mappings(armadillo_iotg_std_extif_i2c_map,
				  ARRAY_SIZE(armadillo_iotg_std_extif_i2c_map));

	pinctrl_register_mappings(armadillo_iotg_std_extif_spi_map,
				  ARRAY_SIZE(armadillo_iotg_std_extif_spi_map));

	pinctrl_register_mappings(armadillo_iotg_std_extif_pwm_map,
				  ARRAY_SIZE(armadillo_iotg_std_extif_pwm_map));

	pinctrl_register_mappings(armadillo_iotg_std_extif_can_map,
				  ARRAY_SIZE(armadillo_iotg_std_extif_can_map));

	pinctrl_register_mappings(armadillo_iotg_std_extif_w1_map,
				  ARRAY_SIZE(armadillo_iotg_std_extif_w1_map));

	pinctrl_register_mappings(armadillo_iotg_std_extif_gpio_map,
				  ARRAY_SIZE(armadillo_iotg_std_extif_gpio_map));

	if (IS_ENABLED(CONFIG_AIOTG_STD_UART1))
		imx25_add_imx_uart0(&uart_pdata);

	if (IS_ENABLED(CONFIG_AIOTG_STD_UART3))
		imx25_add_imx_uart2(&uart_pdata);

	if (IS_ENABLED(CONFIG_AIOTG_STD_UART4_KPP) ||
	    IS_ENABLED(CONFIG_AIOTG_STD_UART4_LCD))
		imx25_add_imx_uart3(&uart_pdata);

	if (IS_ENABLED(CONFIG_AIOTG_STD_UART5))
		imx25_add_imx_uart4(&uart_pdata);

	if (IS_ENABLED(CONFIG_AIOTG_STD_I2C2)) {
		imx25_add_imx_i2c1(&mx25_i2c1_data);
		i2c_register_board_info(1, armadillo_iotg_std_extif_i2c1,
					ARRAY_SIZE(armadillo_iotg_std_extif_i2c1));
	}

	if (IS_ENABLED(CONFIG_AIOTG_STD_I2C3_CON1_24_25) ||
	    IS_ENABLED(CONFIG_AIOTG_STD_I2C3_CON1_51_52)) {
		imx25_add_imx_i2c2(&mx25_i2c2_data);
		i2c_register_board_info(2, armadillo_iotg_std_extif_i2c2,
					ARRAY_SIZE(armadillo_iotg_std_extif_i2c2));
	}

	if (IS_ENABLED(CONFIG_AIOTG_STD_SPI2)) {
		imx25_add_spi_imx1(&spi1_pdata);
		spi_register_board_info(armadillo_iotg_std_spi1_board_info,
					ARRAY_SIZE(armadillo_iotg_std_spi1_board_info));
	}

	if (IS_ENABLED(CONFIG_AIOTG_STD_SPI3)) {
		imx25_add_spi_imx2(&spi2_pdata);
		spi_register_board_info(armadillo_iotg_std_spi2_board_info,
					ARRAY_SIZE(armadillo_iotg_std_spi2_board_info));
	}

	if (IS_ENABLED(CONFIG_AIOTG_STD_PWM1))
		imx25_add_mxc_pwm(0);

	if (IS_ENABLED(CONFIG_AIOTG_STD_PWM2))
		imx25_add_mxc_pwm(1);

	if (IS_ENABLED(CONFIG_AIOTG_STD_PWM3))
		imx25_add_mxc_pwm(2);

	if (IS_ENABLED(CONFIG_AIOTG_STD_PWM4))
		imx25_add_mxc_pwm(3);

	if (IS_ENABLED(CONFIG_AIOTG_STD_CAN1))
		imx25_add_flexcan0();

	if (IS_ENABLED(CONFIG_AIOTG_STD_CAN2))
		imx25_add_flexcan1();

	if (IS_ENABLED(CONFIG_AIOTG_STD_W1))
		imx25_add_mxc_w1();

	armadillo_iotg_std_exitif_set_gpio();
}
