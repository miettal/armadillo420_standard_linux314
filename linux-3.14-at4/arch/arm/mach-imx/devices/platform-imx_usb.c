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

#define imx_imx_usb_data_entry_single(soc, _devid, _id, hs)	\
	{								\
		.devid = _devid,					\
		.id = _id,						\
		.iobase = soc ## _USB_ ## hs ## _BASE_ADDR,		\
		.irq = soc ## _INT_USB_ ## hs,				\
	}

#ifdef CONFIG_SOC_IMX25
const struct imx_imx_usb_data imx25_imx_usb_otg_data __initconst =
	imx_imx_usb_data_entry_single(MX25, "imx27-usb", 0, OTG);
const struct imx_imx_usb_data imx25_imx_usb_hs_data __initconst =
	imx_imx_usb_data_entry_single(MX25, "imx27-usb", 1, HS);
#endif /* ifdef CONFIG_SOC_IMX25 */

struct platform_device *__init imx_add_imx_usb(
		const struct imx_imx_usb_data *data,
		struct imx_usb_platform_data *pdata)
{
	struct resource res[] = {
		{
			.start = data->iobase,
			.end = data->iobase + SZ_512 - 1,
			.flags = IORESOURCE_MEM,
		}, {
			.start = data->irq,
			.end = data->irq,
			.flags = IORESOURCE_IRQ,
		},
	};

	pdata->data.index = data->id;
	return imx_add_platform_device_dmamask(data->devid, data->id,
			res, ARRAY_SIZE(res),
			pdata, sizeof(*pdata), DMA_BIT_MASK(32));
}
