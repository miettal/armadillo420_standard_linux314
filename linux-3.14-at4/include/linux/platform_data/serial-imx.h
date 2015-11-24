/*
 * Copyright (C) 2008 by Sascha Hauer <kernel@pengutronix.de>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#ifndef ASMARM_ARCH_UART_H
#define ASMARM_ARCH_UART_H

#include <linux/serial.h>

#define IMXUART_HAVE_RTSCTS (1<<0)

enum imxuart_rx_gate_types {
	IMXUART_RX_GATE_NONE,
	IMXUART_RX_GATE_GPIO,
};

enum imxuart_rs485_tx_gate_types {
	IMXUART_RS485_TX_GATE_RTS,
	IMXUART_RS485_TX_GATE_NONE,
	IMXUART_RS485_TX_GATE_GPIO,
};

enum imxuart_rs485_duplex_types {
	IMXUART_RS485_DUPLEX_NONE,
	IMXUART_RS485_DUPLEX_GPIO,
};

struct imxuart_platform_data {
	int (*init)(struct platform_device *pdev);
	void (*exit)(struct platform_device *pdev);
	unsigned int flags;

	struct serial_rs485 rs485;

	enum imxuart_rx_gate_types rx_gate_type;
	unsigned int rx_gate_gpio;
	bool rx_gate_active_low;

	enum imxuart_rs485_tx_gate_types rs485_tx_gate_type;
	unsigned int rs485_tx_gate_gpio;
	enum imxuart_rs485_duplex_types rs485_duplex_type;
	unsigned int rs485_duplex_gpio; /* 0: half, 1: full */
};

#endif
