/*
 * Copyright (C) 2015 Atmark Techno, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/pinctrl/machine.h>
#include <linux/platform_device.h>

#include "armadillo_iotg_std_addon.h"
#include "../imx25-named_gpio.h"
#include "../iomux-mx25.h"
#include "../hardware.h"
#include "../devices-imx25.h"

static iomux_v3_cfg_t addon_pinctrl_pads_con1[] = {
	MX25_PAD_LD11__UART4_CTS,
	NEW_PAD_CTRL(MX25_PAD_LD10__UART4_RTS, PAD_CTL_PUS_100K_DOWN),
	MX25_PAD_LD9__UART4_TXD,
	NEW_PAD_CTRL(MX25_PAD_LD8__UART4_RXD, PAD_CTL_PUS_100K_UP),
	MX25_PAD_LD3__GPIO_2_18,
	MX25_PAD_LD6__GPIO_1_20,
	MX25_PAD_LD7__GPIO_1_21,
};

static iomux_v3_cfg_t addon_pinctrl_pads_con2[] = {
	MX25_PAD_UART1_CTS__UART1_CTS,
	NEW_PAD_CTRL(MX25_PAD_UART1_RTS__UART1_RTS, PAD_CTL_PUS_100K_DOWN),
	MX25_PAD_UART1_TXD__UART1_TXD,
	NEW_PAD_CTRL(MX25_PAD_UART1_RXD__UART1_RXD, PAD_CTL_PUS_100K_UP),
	MX25_PAD_KPP_ROW3__GPIO_3_0,
	MX25_PAD_KPP_COL2__GPIO_3_3,
	MX25_PAD_KPP_COL3__GPIO_3_4,
};

static struct {
	iomux_v3_cfg_t *pads;
	size_t nr_pads;
	int uart_id;
	struct addon_gpio cmd_mldp;
	struct addon_gpio wake_sw;
	struct addon_gpio wake_hw;
} addon_data[NR_ADDON_INTERFACES] = {
	[ADDON_INTERFACE1] = {
		.pads		= addon_pinctrl_pads_con1,
		.nr_pads	= ARRAY_SIZE(addon_pinctrl_pads_con1),
		.uart_id	= 3,
		.cmd_mldp	= ADDON_GPIO(IMX_GPIO_NR(2, 18),
					     "RN4020_CMDMLDP_CON1"),
		.wake_sw	= ADDON_GPIO(IMX_GPIO_NR(1, 20),
					     "RN4020_WAKE_SW_CON1"),
		.wake_hw	= ADDON_GPIO(IMX_GPIO_NR(1, 21),
					     "RN4020_WAKE_HW_CON1"),
	},
	[ADDON_INTERFACE2] = {
		.pads		= addon_pinctrl_pads_con2,
		.nr_pads	= ARRAY_SIZE(addon_pinctrl_pads_con2),
		.uart_id	= 0,
		.cmd_mldp	= ADDON_GPIO(IMX_GPIO_NR(3, 0),
					     "RN4020_CMDMLDP_CON2"),
		.wake_sw	= ADDON_GPIO(IMX_GPIO_NR(3, 3),
					     "RN4020_WAKE_SW_CON2"),
		.wake_hw	= ADDON_GPIO(IMX_GPIO_NR(3, 4),
					     "RN4020_WAKE_HW_CON2"),
	},
};

static const struct imxuart_platform_data addon_uart_pdata __initconst = {
	.flags = IMXUART_HAVE_RTSCTS,
};

#define RN4020_COMMAND_MODE	(1)
#define RN4020_MLDP_MODE	(0)
#define RN4020_WAKE_SW_ASSERT	(1)
#define RN4020_WAKE_SW_DEASSERT	(0)
#define RN4020_WAKE_HW_ASSERT	(1)
#define RN4020_WAKE_HW_DEASSERT	(0)

int __init
addon_setup_atmark_techno_rn4020(struct addon_device_descriptor *desc,
			      enum addon_interface intf)
{
	mxc_iomux_v3_setup_multiple_pads(addon_data[intf].pads,
					 addon_data[intf].nr_pads);

	imx25_add_imx_uart(addon_data[intf].uart_id, &addon_uart_pdata);

	addon_gpio_request(addon_data[intf].cmd_mldp);
	addon_gpio_request(addon_data[intf].wake_sw);
	addon_gpio_request(addon_data[intf].wake_hw);

	addon_gpio_direction_output(addon_data[intf].wake_sw,
				    RN4020_WAKE_SW_ASSERT);
	addon_gpio_direction_output(addon_data[intf].wake_hw,
				    RN4020_WAKE_HW_DEASSERT);
	addon_gpio_direction_output(addon_data[intf].cmd_mldp,
				    RN4020_COMMAND_MODE);

	addon_gpio_export(addon_data[intf].wake_sw, false);
	addon_gpio_export(addon_data[intf].wake_hw, false);
	addon_gpio_export(addon_data[intf].cmd_mldp, false);

	return 0;
}
