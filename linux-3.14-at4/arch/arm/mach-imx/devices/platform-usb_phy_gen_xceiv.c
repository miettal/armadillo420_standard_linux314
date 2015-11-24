/*
 * Copyright (C) 2015 Atmark Techno, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation.
 */

#include <linux/dma-mapping.h>

#include "../hardware.h"
#include "devices-common.h"

#define imx_usb_phy_gen_xceiv_data_entry_single(_id)	\
	{						\
		.id = _id,				\
	}

#ifdef CONFIG_SOC_IMX25
const struct imx_usb_phy_gen_xceiv_data imx25_usb_phy_gen_xceiv_otg_data __initconst =
	imx_usb_phy_gen_xceiv_data_entry_single(0);
const struct imx_usb_phy_gen_xceiv_data imx25_usb_phy_gen_xceiv_hs_data __initconst =
	imx_usb_phy_gen_xceiv_data_entry_single(1);
#endif /* ifdef CONFIG_SOC_IMX25 */

struct platform_device *__init imx_add_usb_phy_gen_xceiv(
		const struct imx_usb_phy_gen_xceiv_data *data)
{
	return platform_device_register_simple("usb_phy_gen_xceiv", data->id,
			NULL, 0);
}
