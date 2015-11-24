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

#ifndef __ASM_ARCH_MXC_IMX25_NAMED_GPIO_H__
#define __ASM_ARCH_MXC_IMX25_NAMED_GPIO_H__

#define IMX25_NR_BANKS_PER_GPIOCHIP	(4)
#define IMX25_NR_GPIOS_PER_BANK		(32)
#define IMX25_NR_GPIOS			(IMX25_NR_BANKS_PER_GPIOCHIP * \
					 IMX25_NR_GPIOS_PER_BANK)

extern int imx25_named_gpio_init(void);
extern int imx25_named_extgpio_init(unsigned ngpio);
extern int imx25_named_gpio_request(int gpio, const char *name);

#endif /* __ASM_ARCH_MXC_IMX25_NAMED_GPIO_H__ */
