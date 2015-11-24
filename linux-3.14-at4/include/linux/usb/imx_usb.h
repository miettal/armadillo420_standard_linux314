/*
 * Freescale i.MX driver platform data
 *
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

#ifndef __LINUX_USB_IMX_USB_H
#define __LINUX_USB_IMX_USB_H

#include <linux/usb/chipidea.h>

struct imx_usbmisc_data {
	int index;

	unsigned int disable_oc:1; /* over current detect disabled */
	unsigned int evdo:1; /* set external vbus divider option */
};

struct imx_usb_platform_data {
	enum usb_phy_interface phy_mode;
	enum usb_dr_mode dr_mode;

	struct imx_usbmisc_data data;
};

#endif /* __LINUX_USB_IMX_USB_H */
