/*
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

#include <linux/types.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>

#include "common.h"
#include "devices-imx25.h"
#include "hardware.h"
#include "iomux-mx25.h"
#include "mx25.h"
#include "imx25-named_gpio.h"

#define GPIO_TO_BANK(gpio)	(gpio / IMX25_NR_GPIOS_PER_BANK)
#define GPIO_TO_OFFSET(gpio)	(gpio - (IMX25_NR_GPIOS_PER_BANK * \
					 GPIO_TO_BANK(gpio)))

#define MAX_NR_EXTGPIOS (32)

static unsigned nr_exgpios;

static const char *gpio1_names[IMX25_NR_GPIOS_PER_BANK];
static const char *gpio2_names[IMX25_NR_GPIOS_PER_BANK];
static const char *gpio3_names[IMX25_NR_GPIOS_PER_BANK];
static const char *gpio4_names[IMX25_NR_GPIOS_PER_BANK];
static const char *extgpio_names[MAX_NR_EXTGPIOS];

static const char *(*gpio_names[])[] = {
	&gpio1_names,
	&gpio2_names,
	&gpio3_names,
	&gpio4_names,
	&extgpio_names,
};

int imx25_named_gpio_init(void)
{
	struct gpio_chip *chip;
	int i;

	for (i = 0; i < IMX25_NR_BANKS_PER_GPIOCHIP; i++) {
		chip = gpio_to_chip(IMX_GPIO_NR(i + 1, 0));
		chip->names = *gpio_names[i];
	}

	return 0;
}

int imx25_named_extgpio_init(unsigned ngpio)
{
	struct gpio_chip *chip;

	if (ngpio > MAX_NR_EXTGPIOS)
		return -ERANGE;

	nr_exgpios = ngpio;

	chip = gpio_to_chip(IMX25_NR_GPIOS);
	chip->names = *gpio_names[IMX25_NR_BANKS_PER_GPIOCHIP];

	return 0;
}

int imx25_named_gpio_request(int gpio, const char *name)
{
	int bank, offset;

	if (gpio >= (IMX25_NR_GPIOS + nr_exgpios))
		return -ERANGE;

	if (gpio < IMX25_NR_GPIOS) {
		bank = GPIO_TO_BANK(gpio);
		offset = GPIO_TO_OFFSET(gpio);
	} else {
		bank = IMX25_NR_BANKS_PER_GPIOCHIP;
		offset = gpio - IMX25_NR_GPIOS;
	}

	(*gpio_names[bank])[offset] = name;

	return gpio_request(gpio, name);
}
