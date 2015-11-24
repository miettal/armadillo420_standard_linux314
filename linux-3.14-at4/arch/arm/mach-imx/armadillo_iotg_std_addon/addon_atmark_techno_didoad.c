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

#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/pinctrl/machine.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/fixed.h>

#include "armadillo_iotg_std_addon.h"
#include "../imx25-named_gpio.h"
#include "../iomux-mx25.h"
#include "../hardware.h"
#include "../devices-imx25.h"

static iomux_v3_cfg_t addon_pinctrl_pads_con1[] = {
	MX25_PAD_GPIO_A__GPIO_A,
	MX25_PAD_GPIO_B__GPIO_B,
	MX25_PAD_LD6__GPIO_1_20,
	NEW_PAD_CTRL(MX25_PAD_LD2__GPIO_2_17, PAD_CTL_PUS_100K_UP),
	NEW_PAD_CTRL(MX25_PAD_LD1__GPIO_2_16, PAD_CTL_PUS_100K_UP),

	NEW_PAD_CTRL(MX25_PAD_LD14__CSPI2_SCLK, PAD_CTL_PUS_100K_UP),
	NEW_PAD_CTRL(MX25_PAD_LD13__CSPI2_MISO, PAD_CTL_PUS_100K_UP),
	NEW_PAD_CTRL(MX25_PAD_LD12__CSPI2_MOSI, PAD_CTL_PUS_100K_UP),
	NEW_PAD_CTRL(MX25_PAD_OE_ACD__GPIO_1_25, PAD_CTL_PUS_100K_UP),
};

static iomux_v3_cfg_t addon_pinctrl_pads_con2[] = {
	MX25_PAD_GPIO_C__GPIO_C,
	MX25_PAD_GPIO_D__GPIO_D,
	MX25_PAD_KPP_COL2__GPIO_3_3,
	NEW_PAD_CTRL(MX25_PAD_KPP_ROW2__GPIO_2_31, PAD_CTL_PUS_100K_UP),
	NEW_PAD_CTRL(MX25_PAD_KPP_ROW1__GPIO_2_30, PAD_CTL_PUS_100K_UP),

	NEW_PAD_CTRL(MX25_PAD_CSI_D4__CSPI3_SCLK, PAD_CTL_PUS_100K_UP),
	NEW_PAD_CTRL(MX25_PAD_CSI_D3__CSPI3_MISO, PAD_CTL_PUS_100K_UP),
	NEW_PAD_CTRL(MX25_PAD_CSI_D2__CSPI3_MOSI, PAD_CTL_PUS_100K_UP),
	NEW_PAD_CTRL(MX25_PAD_CSI_D9__GPIO_4_21, PAD_CTL_PUS_100K_UP),
};

static struct spi_board_info addon_spi_board_info_con1[] __initdata = {
	{
		.modalias = "mcp3202",
		.max_speed_hz = 1000000,
		.bus_num = 1,
		.chip_select = 0,
	},
};

static struct spi_board_info addon_spi_board_info_con2[] __initdata = {
	{
		.modalias = "mcp3202",
		.max_speed_hz = 1000000,
		.bus_num = 2,
		.chip_select = 0,
	},
};

static struct regulator_consumer_supply fixed5v0_consumers_con1[] = {
	REGULATOR_SUPPLY("vref", "spi1.0"),
};

static struct regulator_consumer_supply fixed5v0_consumers_con2[] = {
	REGULATOR_SUPPLY("vref", "spi2.0"),
};

static struct {
	iomux_v3_cfg_t *pads;
	size_t nr_pads;
	int spi_id;
	struct spi_board_info *spi_boards;
	size_t nr_spi_boards;
	struct regulator_consumer_supply *supply;
	size_t nr_supply;
	int supply_id;
	const char *supply_name;
	struct addon_gpio adum1401_ve1;
	struct addon_gpio di1;
	struct addon_gpio di2;
	struct addon_gpio do1;
	struct addon_gpio do2;
} addon_data[NR_ADDON_INTERFACES] = {
	[ADDON_INTERFACE1] = {
		.pads		= addon_pinctrl_pads_con1,
		.nr_pads	= ARRAY_SIZE(addon_pinctrl_pads_con1),
		.spi_id		= 1,
		.spi_boards	= addon_spi_board_info_con1,
		.nr_spi_boards	= ARRAY_SIZE(addon_spi_board_info_con1),
		.supply		= fixed5v0_consumers_con1,
		.nr_supply	= ARRAY_SIZE(fixed5v0_consumers_con1),
		.supply_id	= 4,
		.supply_name	= "mcp3202 Vref CON1",
		.adum1401_ve1	= ADDON_GPIO(IMX_GPIO_NR(1, 20),
					     "ADUM1401_VE1_CON1"),
		.di1		= ADDON_GPIO(IMX_GPIO_NR(2, 16),
					     "DI1_CON1"),
		.di2		= ADDON_GPIO(IMX_GPIO_NR(2, 17),
					     "DI2_CON1"),
		.do1		= ADDON_GPIO(IMX_GPIO_NR(1, 0),
					     "DO1_CON1"),
		.do2		= ADDON_GPIO(IMX_GPIO_NR(1, 1),
					     "DO2_CON1"),
	},
	[ADDON_INTERFACE2] = {
		.pads		= addon_pinctrl_pads_con2,
		.nr_pads	= ARRAY_SIZE(addon_pinctrl_pads_con2),
		.spi_id		= 2,
		.spi_boards	= addon_spi_board_info_con2,
		.nr_spi_boards	= ARRAY_SIZE(addon_spi_board_info_con2),
		.supply		= fixed5v0_consumers_con2,
		.nr_supply	= ARRAY_SIZE(fixed5v0_consumers_con2),
		.supply_id	= 5,
		.supply_name	= "mcp3202 Vref CON2",
		.adum1401_ve1	= ADDON_GPIO(IMX_GPIO_NR(1, 20),
					     "ADUM1401_VE1_CON1"),
		.di1		= ADDON_GPIO(IMX_GPIO_NR(2, 16),
					     "DI1_CON1"),
		.di2		= ADDON_GPIO(IMX_GPIO_NR(2, 17),
					     "DI2_CON1"),
		.do1		= ADDON_GPIO(IMX_GPIO_NR(1, 0),
					     "DO1_CON1"),
		.do2		= ADDON_GPIO(IMX_GPIO_NR(1, 1),
					     "DO2_CON1"),
		.adum1401_ve1	= ADDON_GPIO(IMX_GPIO_NR(3, 3),
					     "ADUM1401_VE1_CON2"),
		.di1		= ADDON_GPIO(IMX_GPIO_NR(2, 30),
					     "DI1_CON2"),
		.di2		= ADDON_GPIO(IMX_GPIO_NR(2, 31),
					     "DI2_CON2"),
		.do1		= ADDON_GPIO(IMX_GPIO_NR(1, 2),
					     "DO1_CON2"),
		.do2		= ADDON_GPIO(IMX_GPIO_NR(1, 3),
					     "DO2_CON2"),
	},
};

static int spi1_cs[] = {
	IMX_GPIO_NR(1, 25),
};

static int spi2_cs[] = {
	IMX_GPIO_NR(4, 21),
};

static const struct spi_imx_master
addon_spi_pdata[NR_ADDON_INTERFACES] __initconst = {
	[ADDON_INTERFACE1] = {
		.chipselect	= spi1_cs,
		.num_chipselect	= ARRAY_SIZE(spi1_cs),
	},
	[ADDON_INTERFACE2] = {
		.chipselect	= spi2_cs,
		.num_chipselect	= ARRAY_SIZE(spi2_cs),
	},
};

int __init
addon_setup_atmark_techno_didoad(struct addon_device_descriptor *desc,
				 enum addon_interface intf)
{
	mxc_iomux_v3_setup_multiple_pads(addon_data[intf].pads,
					 addon_data[intf].nr_pads);

	imx25_add_spi_imx(addon_data[intf].spi_id, &addon_spi_pdata[intf]);
	spi_register_board_info(addon_data[intf].spi_boards,
				addon_data[intf].nr_spi_boards);

	regulator_register_always_on(addon_data[intf].supply_id,
				     addon_data[intf].supply_name,
				     addon_data[intf].supply,
				     addon_data[intf].nr_supply, 5000000);

	addon_gpio_request(addon_data[intf].adum1401_ve1);
	addon_gpio_request(addon_data[intf].di1);
	addon_gpio_request(addon_data[intf].di2);
	addon_gpio_request(addon_data[intf].do1);
	addon_gpio_request(addon_data[intf].do2);

	addon_gpio_direction_output(addon_data[intf].adum1401_ve1, 1);
	addon_gpio_direction_input(addon_data[intf].di1);
	addon_gpio_direction_input(addon_data[intf].di2);
	addon_gpio_direction_output(addon_data[intf].do1, 0);
	addon_gpio_direction_output(addon_data[intf].do2, 0);

	addon_gpio_export(addon_data[intf].adum1401_ve1, false);
	addon_gpio_export(addon_data[intf].di1, false);
	addon_gpio_export(addon_data[intf].di2, false);
	addon_gpio_export(addon_data[intf].do1, false);
	addon_gpio_export(addon_data[intf].do2, false);

	return 0;
}
