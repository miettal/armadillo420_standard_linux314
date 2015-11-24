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

#ifndef __ASM_ARCH_ARMADILLO_IOTG_STD_ADD_ON_H__
#define __ASM_ARCH_ARMADILLO_IOTG_STD_ADD_ON_H__

#include <linux/kernel.h>

struct addon_device_descriptor {
	__be16	vendor_id;
	__be16	product_id;
	__be16	revision;
	__be32	serial_no;
	__u8	reserved[22];
	__u8	vendor_specific[96];
} __attribute__ ((packed));

/* EEPROM vendor ID */
#define ADDON_VENDOR_ID_ATMARK_TECHNO	(0x0001)
#define ADDON_VENDOR_ID_SATORI		(0x0005)

/* EEPROM Atmark Techno product ID */
#define ADDON_PRODUCT_ID_ATMARK_TECHNO_WI_SUN	(0x0001)
#define ADDON_PRODUCT_ID_ATMARK_TECHNO_EN_OCEAN	(0x0002)
#define ADDON_PRODUCT_ID_ATMARK_TECHNO_SERIAL	(0x0003)
#define ADDON_PRODUCT_ID_ATMARK_TECHNO_DIDOAD	(0x0004)
#define ADDON_PRODUCT_ID_ATMARK_TECHNO_RN4020	(0x0005)
#define ADDON_PRODUCT_ID_ATMARK_TECHNO_CAN	(0x0006)
#define ADDON_PRODUCT_ID_ATMARK_TECHNO_ZIGBEE	(0x0007)
#define ADDON_PRODUCT_ID_ATMARK_TECHNO_RS232C	(0x0008)
#define ADDON_PRODUCT_ID_ATMARK_TECHNO_RS485	(0x0009)

/* EEPROM Satori product ID */
#define ADDON_PRODUCT_ID_SATORI_B_ROUTE		(0x0001)
#define ADDON_PRODUCT_ID_SATORI_920M		(0x0002)
#define ADDON_PRODUCT_ID_SATORI_LOW_POWER	(0x0003)

enum addon_interface {
	ADDON_INTERFACE1 = 0,
	ADDON_INTERFACE2,
	NR_ADDON_INTERFACES,
};

struct addon_gpio {
	unsigned	gpio;
	const char	*label;
};

#define ADDON_GPIO(g, l) \
	{ .gpio = g, .label = l }

#define addon_gpio_request(g) \
	imx25_named_gpio_request(g.gpio, g.label)

#define addon_gpio_direction_output(g, v) \
	gpio_direction_output(g.gpio, v)

#define addon_gpio_direction_input(g) \
	gpio_direction_input(g.gpio)

#define addon_gpio_export(g, d) \
	gpio_export(g.gpio, d)

int __init addon_setup_atmark_techno_wi_sun(struct addon_device_descriptor *desc,
					    enum addon_interface intf);
int __init addon_setup_atmark_techno_en_ocean(struct addon_device_descriptor *desc,
					      enum addon_interface intf);
int __init addon_setup_atmark_techno_serial(struct addon_device_descriptor *desc,
					    enum addon_interface intf);
int __init addon_setup_atmark_techno_didoad(struct addon_device_descriptor *desc,
					    enum addon_interface intf);
int __init addon_setup_atmark_techno_rn4020(struct addon_device_descriptor *desc,
					 enum addon_interface intf);
int __init addon_setup_atmark_techno_rs232c(struct addon_device_descriptor *desc,
					    enum addon_interface intf);
int __init addon_setup_atmark_techno_rs485(struct addon_device_descriptor *desc,
					   enum addon_interface intf);
int __init addon_setup_satori_wireless(struct addon_device_descriptor *desc,
				       enum addon_interface intf);

#endif
