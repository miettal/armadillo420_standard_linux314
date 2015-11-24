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

#define imx_usbmisc_imx_data_entry_single(soc, _devid)		\
	{							\
		.devid = _devid,				\
		.iobase = soc ## _USB_USBCTRL_BASE_ADDR,	\
	}

#ifdef CONFIG_SOC_IMX25
const struct imx_usbmisc_imx_data imx25_usbmisc_imx_data __initconst =
	imx_usbmisc_imx_data_entry_single(MX25, "imx25-usbmisc");
#endif /* ifdef CONFIG_SOC_IMX25 */

struct platform_device *__init imx_add_usbmisc_imx(
		const struct imx_usbmisc_imx_data *data)
{
	struct resource res[] = {
		{
			.start = data->iobase,
			.end = data->iobase + SZ_16 - 1,
			.flags = IORESOURCE_MEM,
		},
	};

	return platform_device_register_simple(data->devid, -1,
			res, ARRAY_SIZE(res));
}
