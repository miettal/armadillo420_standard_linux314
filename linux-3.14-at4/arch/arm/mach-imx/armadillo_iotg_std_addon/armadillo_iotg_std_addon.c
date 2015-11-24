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
#include <linux/i2c.h>

#include "armadillo_iotg_std_addon.h"
#include "../iomux-mx25.h"

/* EEPROM I2C slave address */
#define ADDON_EEPROM_ADDR1	(0x50) /* CON1(Ext.I/F1) */
#define ADDON_EEPROM_ADDR2	(0x51) /* CON2(Ext.I/F2) */

struct addon_vendor_name
{
        u16 vendor;
        const char *name;
};
#define VENDOR_NAME(v, n) { ADDON_VENDOR_ID_##v, n }

static struct addon_vendor_name vendor_names[] = {
	VENDOR_NAME(ATMARK_TECHNO, "Atmark Techno"),
	VENDOR_NAME(SATORI, "Satori"),
};

static const char *unknownvendorname = "Unknown Vendor";

struct addon_product_name
{
        u16 vendor;
        u16 product;
        const char *name;
};
#define PRODUCT_NAME(v, p, n) { ADDON_VENDOR_ID_##v, \
				ADDON_PRODUCT_ID_##v##_##p, n }

static struct addon_product_name product_names[] = {
	PRODUCT_NAME(ATMARK_TECHNO, WI_SUN, "Wi-SUN"),
	PRODUCT_NAME(ATMARK_TECHNO, EN_OCEAN, "EnOcean"),
	PRODUCT_NAME(ATMARK_TECHNO, SERIAL, "RS485/RS422/RS232C"),
	PRODUCT_NAME(ATMARK_TECHNO, DIDOAD, "DI/DO/AD"),
	PRODUCT_NAME(ATMARK_TECHNO, RN4020, "RN4020"),
	PRODUCT_NAME(ATMARK_TECHNO, CAN, "Can"),
	PRODUCT_NAME(ATMARK_TECHNO, ZIGBEE, "ZigBee"),
	PRODUCT_NAME(ATMARK_TECHNO, RS232C, "RS232C"),
	PRODUCT_NAME(ATMARK_TECHNO, RS485, "RS485"),
	PRODUCT_NAME(SATORI, B_ROUTE, "B_ROUTE"),
	PRODUCT_NAME(SATORI, 920M, "920M"),
	PRODUCT_NAME(SATORI, LOW_POWER, "LOW_POWER"),
};

static const char *unknownproductname = "Unknown Product";

static iomux_v3_cfg_t addon_pinctrl_pads_defaut[] = {
	/* CON1_3, CON2_24 */
	NEW_PAD_CTRL(MX25_PAD_GPIO_C__GPIO_C, PAD_CTL_PUS_100K_DOWN),
	/* CON1_4, CON2_25 */
	NEW_PAD_CTRL(MX25_PAD_GPIO_D__GPIO_D, PAD_CTL_PUS_100K_DOWN),
	/* CON1_5, CON2_33 */
	MX25_PAD_PWM__GPIO_1_26,
	/* CON1_6, CON2_32 */
	NEW_PAD_CTRL(MX25_PAD_RTCK__GPIO_3_14, PAD_CTL_PUS_100K_DOWN),
	/* CON1_7, CON2_41 */
	NEW_PAD_CTRL(MX25_PAD_UART1_RXD__GPIO_4_22, PAD_CTL_PUS_100K_DOWN),
	/* CON1_8, CON2_40 */
	NEW_PAD_CTRL(MX25_PAD_UART1_TXD__GPIO_4_23, PAD_CTL_PUS_100K_DOWN),
	/* CON1_9, CON2_7, CON2_39 */
	NEW_PAD_CTRL(MX25_PAD_UART1_RTS__GPIO_4_24, PAD_CTL_PUS_100K_DOWN),
	/* CON1_10, CON2_8, CON2_38 */
	NEW_PAD_CTRL(MX25_PAD_UART1_CTS__GPIO_4_25, PAD_CTL_PUS_100K_DOWN),
	/* CON1_11, CON2_50 */
	NEW_PAD_CTRL(MX25_PAD_CSI_D9__GPIO_4_21, PAD_CTL_PUS_100K_DOWN),
	/* CON1_12, CON2_16, CON2_37 */
	NEW_PAD_CTRL(MX25_PAD_CSI_D2__GPIO_1_27, PAD_CTL_PUS_100K_DOWN),
	/* CON1_13, CON2_17, CON2_36 */
	NEW_PAD_CTRL(MX25_PAD_CSI_D3__GPIO_1_28, PAD_CTL_PUS_100K_DOWN),
	/* CON1_14, CON2_12, CON2_18, CON2_35 */
	NEW_PAD_CTRL(MX25_PAD_CSI_D4__GPIO_1_29, PAD_CTL_PUS_100K_DOWN),
	/* CON1_15, CON2_13, CON2_19, CON2_34 */
	NEW_PAD_CTRL(MX25_PAD_CSI_D5__GPIO_1_30, PAD_CTL_PUS_100K_DOWN),
	/* CON1_16, CON2_49 */
	NEW_PAD_CTRL(MX25_PAD_KPP_ROW0__GPIO_2_29, PAD_CTL_PUS_100K_DOWN),
	/* CON1_17, CON2_48 */
	NEW_PAD_CTRL(MX25_PAD_KPP_ROW1__GPIO_2_30, PAD_CTL_PUS_100K_DOWN),
	/* CON1_18, CON2_47 */
	NEW_PAD_CTRL(MX25_PAD_KPP_ROW2__GPIO_2_31, PAD_CTL_PUS_100K_DOWN),
	/* CON1_19, CON2_46 */
	NEW_PAD_CTRL(MX25_PAD_KPP_ROW3__GPIO_3_0, PAD_CTL_PUS_100K_DOWN),
	/* CON1_22, CON2_22, CON2_43 */
	NEW_PAD_CTRL(MX25_PAD_KPP_COL2__GPIO_3_3, PAD_CTL_PUS_100K_DOWN),
	/* CON1_23, CON2_23, CON2_42 */
	NEW_PAD_CTRL(MX25_PAD_KPP_COL3__GPIO_3_4, PAD_CTL_PUS_100K_DOWN),
	/* CON1_24 */
	NEW_PAD_CTRL(MX25_PAD_GPIO_A__GPIO_A, PAD_CTL_PUS_100K_DOWN),
	/* CON1_25 */
	NEW_PAD_CTRL(MX25_PAD_GPIO_B__GPIO_B, PAD_CTL_PUS_100K_DOWN),
	/* CON1_32 */
	MX25_PAD_GPIO_F__GPIO_F,
	/* CON1_33 */
	MX25_PAD_GPIO_E__GPIO_E,
	/* CON1_42 */
	MX25_PAD_LD7__GPIO_1_21,
	/* CON1_43 */
	MX25_PAD_LD6__GPIO_1_20,
	/* CON1_44 */
	MX25_PAD_LD5__GPIO_1_19,
	/* CON1_45 */
	MX25_PAD_LD4__GPIO_2_19,
	/* CON1_46 */
	MX25_PAD_LD3__GPIO_2_18,
	/* CON1_47 */
	MX25_PAD_LD2__GPIO_2_17,
	/* CON1_48 */
	MX25_PAD_LD1__GPIO_2_16,
	/* CON1_49 */
	MX25_PAD_LD0__GPIO_2_15,
	/* CON1_50 */
	MX25_PAD_OE_ACD__GPIO_1_25,
	/* CON1_51 */
	MX25_PAD_VSYNC__GPIO_1_23,
	/* CON1_52 */
	MX25_PAD_HSYNC__GPIO_1_22,
	/* CON1_53 */
	MX25_PAD_LSCLK__GPIO_1_24,
};

static const char *addon_get_vendor_name(u16 vendor)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(vendor_names); i++)
                if (vendor_names[i].vendor == vendor) 
                        return vendor_names[i].name;

        return unknownvendorname;
}

static const char *addon_get_product_name(u16 vendor, u16 product)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(product_names); i++) {
                if ((product_names[i].vendor == vendor) &&
		    (product_names[i].product == product)) 
                        return product_names[i].name;
	}

        return unknownproductname;
}

static int addon_get_descriptor(struct addon_device_descriptor *desc, u16 addr)
{
	struct i2c_adapter *adap;
	union i2c_smbus_data data;
	unsigned char *p = (unsigned char *)desc;
	int i;
	int ret;

	adap = i2c_get_adapter(4);
	if (!adap) {
		pr_err("failed to get i2c adapter\n");
		return -EINVAL;
	}

	for (i = 0; i < sizeof(struct addon_device_descriptor); i++) {
		ret = i2c_smbus_xfer(adap, addr, 0, I2C_SMBUS_READ,
				     i, I2C_SMBUS_BYTE_DATA, &data);
		if (ret)
			goto out;
		*(p + i) = data.byte;
	}

out:
	i2c_put_adapter(adap);

	return ret;
}

/*
 * Setup
 */
static void __init addon_setup(struct addon_device_descriptor *desc,
			       enum addon_interface intf)
{
	u16 vendor_id = be16_to_cpu(desc->vendor_id);
	u16 product_id = be16_to_cpu(desc->product_id);
	u16 revision = be16_to_cpu(desc->revision);
	u32 serial_no = be32_to_cpu(desc->serial_no);
	int ret = -ENODEV;

	pr_info("%s %s board detected at CON%d(Rev %d, SerialNumber=%d).\n",
		addon_get_vendor_name(vendor_id),
		addon_get_product_name(vendor_id, product_id),
		intf + 1, revision, serial_no);

	switch (vendor_id) {
	case ADDON_VENDOR_ID_ATMARK_TECHNO:
		switch (product_id) {
		case ADDON_PRODUCT_ID_ATMARK_TECHNO_WI_SUN:
			ret = addon_setup_atmark_techno_wi_sun(desc, intf);
			break;
		case ADDON_PRODUCT_ID_ATMARK_TECHNO_EN_OCEAN:
			ret = addon_setup_atmark_techno_en_ocean(desc, intf);
			break;
		case ADDON_PRODUCT_ID_ATMARK_TECHNO_SERIAL:
			ret = addon_setup_atmark_techno_serial(desc, intf);
			break;
		case ADDON_PRODUCT_ID_ATMARK_TECHNO_DIDOAD:
			ret = addon_setup_atmark_techno_didoad(desc, intf);
			break;
		case ADDON_PRODUCT_ID_ATMARK_TECHNO_RN4020:
			ret = addon_setup_atmark_techno_rn4020(desc, intf);
			break;
		case ADDON_PRODUCT_ID_ATMARK_TECHNO_CAN:
			break;
		case ADDON_PRODUCT_ID_ATMARK_TECHNO_ZIGBEE:
			break;
		case ADDON_PRODUCT_ID_ATMARK_TECHNO_RS232C:
			ret = addon_setup_atmark_techno_rs232c(desc, intf);
			break;
		case ADDON_PRODUCT_ID_ATMARK_TECHNO_RS485:
			ret = addon_setup_atmark_techno_rs485(desc, intf);
			break;
		default:
			break;
		}
		break;

	case ADDON_VENDOR_ID_SATORI:
		switch (product_id) {
		case ADDON_PRODUCT_ID_SATORI_B_ROUTE:
		case ADDON_PRODUCT_ID_SATORI_920M:
		case ADDON_PRODUCT_ID_SATORI_LOW_POWER:
			ret = addon_setup_satori_wireless(desc, intf);
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}

	if (ret) {
		pr_err("Failed to initialize Add-On.I/F at CON%d.\n",
		       intf + 1);
	}
}

static int __init armadillo_iotg_std_addon_init(void)
{
	struct addon_device_descriptor desc;
	int ret;

	mxc_iomux_v3_setup_multiple_pads(addon_pinctrl_pads_defaut,
					 ARRAY_SIZE(addon_pinctrl_pads_defaut));

	ret = addon_get_descriptor(&desc, ADDON_EEPROM_ADDR1);
	if (!ret)
		addon_setup(&desc, ADDON_INTERFACE1);
	else
		pr_info("No add-on expansion board detected at CON1.\n");

	ret = addon_get_descriptor(&desc, ADDON_EEPROM_ADDR2);
	if (!ret)
		addon_setup(&desc, ADDON_INTERFACE2);
	else
		pr_info("No add-on expansion board detected at CON2.\n");

	return 0;
}
subsys_initcall_sync(armadillo_iotg_std_addon_init);
