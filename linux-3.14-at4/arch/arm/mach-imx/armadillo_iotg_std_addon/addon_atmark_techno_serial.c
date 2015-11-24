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
	MX25_PAD_GPIO_F__GPIO_F,
	MX25_PAD_LD11__UART4_CTS,
	NEW_PAD_CTRL(MX25_PAD_LD10__UART4_RTS, PAD_CTL_PUS_100K_UP),
	MX25_PAD_LD9__UART4_TXD,
	NEW_PAD_CTRL(MX25_PAD_LD8__UART4_RXD, PAD_CTL_PUS_100K_UP),
	NEW_PAD_CTRL(MX25_PAD_LD7__GPIO_1_21, PAD_CTL_PUS_100K_UP),
	MX25_PAD_LD6__GPIO_1_20,
};

static iomux_v3_cfg_t addon_pinctrl_pads_con2[] = {
	MX25_PAD_RTCK__GPIO_3_14,
	MX25_PAD_UART1_CTS__UART1_CTS,
	NEW_PAD_CTRL(MX25_PAD_UART1_RTS__UART1_RTS, PAD_CTL_PUS_100K_UP),
	MX25_PAD_UART1_TXD__UART1_TXD,
	NEW_PAD_CTRL(MX25_PAD_UART1_RXD__UART1_RXD, PAD_CTL_PUS_100K_UP),
	NEW_PAD_CTRL(MX25_PAD_KPP_COL3__GPIO_3_4, PAD_CTL_PUS_100K_UP),
	MX25_PAD_KPP_COL2__GPIO_3_3,
};

static struct {
	iomux_v3_cfg_t *pads;
	size_t nr_pads;
	int uart_id;
	struct addon_gpio xr3160_mode;
} addon_data[NR_ADDON_INTERFACES] = {
	[ADDON_INTERFACE1] = {
		.pads		= addon_pinctrl_pads_con1,
		.nr_pads	= ARRAY_SIZE(addon_pinctrl_pads_con1),
		.uart_id	= 3,
		.xr3160_mode	= ADDON_GPIO(IMX_GPIO_NR(1, 21),
					     "XR3160_MODE_CON1"),
	},
	[ADDON_INTERFACE2] = {
		.pads		= addon_pinctrl_pads_con2,
		.nr_pads	= ARRAY_SIZE(addon_pinctrl_pads_con2),
		.uart_id	= 0,
		.xr3160_mode	= ADDON_GPIO(IMX_GPIO_NR(3, 4),
					     "XR3160_MODE_CON2"),
	},
};

static struct imxuart_platform_data
addon_uart_pdata[NR_ADDON_INTERFACES] = {
	[ADDON_INTERFACE1] = {
		.flags = IMXUART_HAVE_RTSCTS,
		.rs485 = {
			.flags = SER_RS485_RTS_ON_SEND,
		},
		.rx_gate_type = IMXUART_RX_GATE_GPIO,
		.rx_gate_gpio = IMX_GPIO_NR(1, 20),
		.rs485_tx_gate_type = IMXUART_RS485_TX_GATE_RTS,
		.rs485_duplex_type = IMXUART_RS485_DUPLEX_GPIO,
		.rs485_duplex_gpio = IMX_GPIO_NR(1, 5),
	},
	[ADDON_INTERFACE2] = {
		.flags = IMXUART_HAVE_RTSCTS,
		.rs485 = {
			.flags = SER_RS485_RTS_ON_SEND,
		},
		.rx_gate_type = IMXUART_RX_GATE_GPIO,
		.rx_gate_gpio = IMX_GPIO_NR(3, 3),
		.rs485_tx_gate_type = IMXUART_RS485_TX_GATE_RTS,
		.rs485_duplex_type = IMXUART_RS485_DUPLEX_GPIO,
		.rs485_duplex_gpio = IMX_GPIO_NR(3, 14),
	},
};

int __init
addon_setup_atmark_techno_serial(struct addon_device_descriptor *desc,
				 enum addon_interface intf)
{
	mxc_iomux_v3_setup_multiple_pads(addon_data[intf].pads,
					 addon_data[intf].nr_pads);

	addon_gpio_request(addon_data[intf].xr3160_mode);
	addon_gpio_direction_input(addon_data[intf].xr3160_mode);
	addon_gpio_export(addon_data[intf].xr3160_mode, false);

	if (gpio_get_value(addon_data[intf].xr3160_mode.gpio))
		addon_uart_pdata[intf].rs485.flags |= SER_RS485_ENABLED;

	imx25_add_imx_uart(addon_data[intf].uart_id, &addon_uart_pdata[intf]);

	return 0;
}
