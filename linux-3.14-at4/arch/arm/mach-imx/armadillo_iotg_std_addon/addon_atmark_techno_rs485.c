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
	MX25_PAD_GPIO_E__GPIO_E,
	MX25_PAD_LD9__UART4_TXD,
	NEW_PAD_CTRL(MX25_PAD_LD8__UART4_RXD, PAD_CTL_PUS_100K_UP),
};

static iomux_v3_cfg_t addon_pinctrl_pads_con2[] = {
	MX25_PAD_RTCK__GPIO_3_14,
	MX25_PAD_PWM__GPIO_1_26,
	MX25_PAD_UART1_TXD__UART1_TXD,
	NEW_PAD_CTRL(MX25_PAD_UART1_RXD__UART1_RXD, PAD_CTL_PUS_100K_UP),
};

static struct {
	iomux_v3_cfg_t *pads;
	size_t nr_pads;
	int uart_id;
} addon_data[NR_ADDON_INTERFACES] = {
	[ADDON_INTERFACE1] = {
		.pads		= addon_pinctrl_pads_con1,
		.nr_pads	= ARRAY_SIZE(addon_pinctrl_pads_con1),
		.uart_id	= 3,
	},
	[ADDON_INTERFACE2] = {
		.pads		= addon_pinctrl_pads_con2,
		.nr_pads	= ARRAY_SIZE(addon_pinctrl_pads_con2),
		.uart_id	= 0,
	},
};

static const struct imxuart_platform_data
addon_uart_pdata[NR_ADDON_INTERFACES] __initconst = {
	[ADDON_INTERFACE1] = {
		.rs485 = {
			.flags = SER_RS485_ENABLED | SER_RS485_RTS_ON_SEND,
		},
		.rx_gate_type = IMXUART_RX_GATE_GPIO,
		.rx_gate_gpio = IMX_GPIO_NR(1, 4),
		.rx_gate_active_low = 1,
		.rs485_tx_gate_type = IMXUART_RS485_TX_GATE_GPIO,
		.rs485_tx_gate_gpio = IMX_GPIO_NR(1, 5),
	},
	[ADDON_INTERFACE2] = {
		.rs485 = {
			.flags = SER_RS485_ENABLED | SER_RS485_RTS_ON_SEND,
		},
		.rx_gate_type = IMXUART_RX_GATE_GPIO,
		.rx_gate_gpio = IMX_GPIO_NR(1, 26),
		.rx_gate_active_low = 1,
		.rs485_tx_gate_type = IMXUART_RS485_TX_GATE_GPIO,
		.rs485_tx_gate_gpio = IMX_GPIO_NR(3, 14),
	},
};

int __init
addon_setup_atmark_techno_rs485(struct addon_device_descriptor *desc,
				enum addon_interface intf)
{
	mxc_iomux_v3_setup_multiple_pads(addon_data[intf].pads,
					 addon_data[intf].nr_pads);

	imx25_add_imx_uart(addon_data[intf].uart_id, &addon_uart_pdata[intf]);

	return 0;
}
