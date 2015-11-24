/*
 * Copyright (C) 2015 Atmark Techno, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation.
 */
#include "devices-common.h"
#include "../mx25.h"

#define imx_imx_pinctrl_data_entry_single(soc, _size)	\
	{						\
		.iobase = soc ## _IOMUXC_BASE_ADDR,	\
		.iosize = _size,			\
	}

#ifdef CONFIG_SOC_IMX25
const struct imx_imx_pinctrl_data imx25_imx_pinctrl_data __initconst =
	imx_imx_pinctrl_data_entry_single(MX25, SZ_4K);
#endif /* ifdef CONFIG_SOC_IMX25 */

struct platform_device __init __maybe_unused *imx_add_imx_pinctrl(char *name,
	const struct imx_imx_pinctrl_data *data)
{
	struct resource res[] = {
		{
			.start = data->iobase,
			.end = data->iobase + data->iosize - 1,
			.flags = IORESOURCE_MEM,
		},
	};

	return platform_device_register_simple(name, 0, res, ARRAY_SIZE(res));
}
