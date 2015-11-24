/*
 * Copyright (C) 2015 Atmark Techno, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation.
 */
#include "devices-common.h"

struct platform_device *__init imx_add_gpio_keys_polled(
		const struct gpio_keys_platform_data *pdata)
{
	return imx_add_platform_device("gpio-keys-polled", -1, NULL,
		 0, pdata, sizeof(*pdata));
}
