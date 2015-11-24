#include <autoconf.h>
#include <hermit.h>
#include <herrno.h>
#include <platform.h>
#include <machine.h>
#include <init.h>
#include <arch/system.h>
#include <arch/iomux.h>
#include <arch/clock.h>
#include <htypes.h>
#include <io.h>
#include <string.h>
#include <memdev.h>
#include <param.h>
#include <map.h>
#include <boost.h>
#include <flash.h>
#include <medium.h>
#include <led.h>
#include <i2c.h>
#include <i2c_mx25.h>
#include <pmic_mc34704.h>
#include <net/eth_util.h>
#include <net/mx25_fec.h>
#include <mx25_irq.h>
#include <mx25_wdt.h>
#include <mx25_esdhc.h>
#include <gpio_pca9538.h>
#include "board.h"

char target_name[256];
char *target_profile = target_name;

static struct board_private board_priv;
static struct memory_device mdev_param;

#define FLASH_ADDR(offset) (FLASH_START + (offset))
#define RAM_ADDR(offset) (DRAM_START + (offset))

#define BASE_BOARD_GEN_MASK (0x03)
#define CPU_BOARD_REV_MINOR_MASK (0xff << 0)
#define CPU_BOARD_REV_MAJOR_MASK (0xff << 8)
#define CPU_BOARD_REV_MASK (CPU_BOARD_REV_MAJOR_MASK | CPU_BOARD_REV_MINOR_MASK)

static struct iomux_info ids_pins[] = {
	{ MX25_PIN_A24, MUX_ALT5, MUX_ALT0, -1, -1 },
	{ MX25_PIN_A25, MUX_ALT5, MUX_ALT0, -1, -1 },
	{ },
};

static struct iomux_info jumper_pins[] = {
	{ MX25_PIN_NF_CE0, MUX_ALT5, MUX_ALT5, 0x0000, 0x0000 },
	{ },
};

static struct iomux_info led_pins[] = {
	/* Red */
	{ MX25_PIN_NFALE, MUX_ALT5, MUX_ALT5, -1, -1 },
	/* Green */
	{ MX25_PIN_NFCLE, MUX_ALT5, MUX_ALT5, -1, -1 },
	/* Yellow */
	{ MX25_PIN_BOOT_MODE0, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ },
};

static struct iomux_info sw_pins[] = {
	{ MX25_PIN_NFWP_B, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ },
};

static struct iomux_info wdt_pins[] = {
	{ MX25_PIN_CONTRAST, MUX_ALT7, MUX_ALT7, -1, -1 },
	{ MX25_PIN_POWER_FAIL, MUX_ALT5, MUX_ALT5, 0x0000, 0x0000 },
	{ },
};

static struct iomux_info gpio_pins[] = {
	{ MX25_PIN_CSPI1_SS0, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ MX25_PIN_CSPI1_RDY, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ MX25_PIN_CLKO, MUX_ALT5, MUX_ALT5, 0x0000, 0x0000 },
	{ MX25_PIN_EXT_ARMCLK, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ MX25_PIN_CSPI1_SS1, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ MX25_PIN_CSPI1_SCLK, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ MX25_PIN_CSI_D9, MUX_ALT5, MUX_ALT5, 0x00c0, 0x00c0 },
	{ MX25_PIN_VSTBY_REQ, MUX_ALT5, MUX_ALT5, 0x0080, 0x0080 },
	{ MX25_PIN_RTCK, MUX_ALT5, MUX_ALT5, 0x00c0, 0x00c0 },
	{ MX25_PIN_GPIO_C, MUX_ALT0, MUX_ALT0, 0x00c0, 0x00c0 },
	{ MX25_PIN_GPIO_D, MUX_ALT0, MUX_ALT0, 0x0080, 0x0080 },
	{ MX25_PIN_GPIO_A, MUX_ALT0, MUX_ALT0, 0x00c0, 0x00c0 },
	{ MX25_PIN_GPIO_B, MUX_ALT0, MUX_ALT0, 0x00c0, 0x00c0 },
	{ MX25_PIN_CSPI1_MOSI, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ MX25_PIN_CSPI1_MISO, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ MX25_PIN_KPP_COL0, MUX_ALT5, MUX_ALT5, 0x0080, 0x0080 },
	{ MX25_PIN_KPP_COL1, MUX_ALT5, MUX_ALT5, 0x0080, 0x0080 },
	{ MX25_PIN_KPP_COL2, MUX_ALT5, MUX_ALT5, 0x0080, 0x0080 },
	{ MX25_PIN_KPP_COL3, MUX_ALT5, MUX_ALT5, 0x0080, 0x0080 },
	{ MX25_PIN_LD0, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ MX25_PIN_LD1, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ MX25_PIN_LD2, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ MX25_PIN_LD3, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ MX25_PIN_LD4, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ MX25_PIN_LD5, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ MX25_PIN_LD6, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ MX25_PIN_LD7, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ MX25_PIN_HSYNC, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ MX25_PIN_VSYNC, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ MX25_PIN_LSCLK, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ MX25_PIN_OE_ACD, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ MX25_PIN_GPIO_E, MUX_ALT0, MUX_ALT0, 0x0080, 0x0080 },
	{ MX25_PIN_GPIO_F, MUX_ALT0, MUX_ALT0, 0x0080, 0x0080 },
	{ MX25_PIN_PWM, MUX_ALT5, MUX_ALT5, 0x0000, 0x0000 },
	{ },
};

static struct iomux_info uart1_pins[] = {
	{ MX25_PIN_UART1_RXD, MUX_ALT0, MUX_ALT5, -1, 0x0080 },
	{ MX25_PIN_UART1_TXD, MUX_ALT0, MUX_ALT5, -1, 0x0080 },
	{ MX25_PIN_UART1_RTS, MUX_ALT0, MUX_ALT5, -1, 0x00c0 },
	{ MX25_PIN_UART1_CTS, MUX_ALT0, MUX_ALT5, -1, 0x00c0 },
	{ MX25_PIN_IPP_UART1_RTS, -1, -1, 0x0001, 0x0001 },
	{ MX25_PIN_IPP_UART1_CTS, -1, -1, 0x0001, 0x0001 },
	{ },
};

static struct iomux_info uart2_pins[] = {
	{ MX25_PIN_UART2_RXD, MUX_ALT0, MUX_ALT0, -1, -1 },
	{ MX25_PIN_UART2_TXD, MUX_ALT0, MUX_ALT0, -1, -1 },
	{ MX25_PIN_UART2_RTS, MUX_ALT0, MUX_ALT0, 0x00e0, 0x00e0 },
	{ MX25_PIN_UART2_CTS, MUX_ALT0, MUX_ALT0, -1, -1 },
	{ MX25_PIN_BOOT_MODE1, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ },
};

static struct iomux_info uart3_pins[] = {
	{ MX25_PIN_KPP_ROW0, MUX_ALT1, MUX_ALT5, 0x00a0, 0x0080 },
	{ MX25_PIN_KPP_ROW1, MUX_ALT1, MUX_ALT5, 0x0000, 0x0080 },
	{ MX25_PIN_KPP_ROW2, MUX_ALT1, MUX_ALT5, -1, 0x00c0 },
	{ MX25_PIN_KPP_ROW3, MUX_ALT1, MUX_ALT5, 0x0000, 0x00c0 },
	{ MX25_PIN_IPP_UART3_RXD, -1, -1, 0x0001, 0x0000 },
	{ MX25_PIN_IPP_UART3_RTS, -1, -1, 0x0001, 0x0000 },
};

static struct iomux_info uart4_pins[] = {
	{ MX25_PIN_LD8, MUX_ALT2, MUX_ALT0, 0x00e0, -1 },
	{ MX25_PIN_LD9, MUX_ALT2, MUX_ALT0, 0x0000, -1 },
	{ MX25_PIN_LD10, MUX_ALT2, MUX_ALT0, 0x00e0, -1 },
	{ MX25_PIN_LD11, MUX_ALT2, MUX_ALT0, 0x0000, -1 },
	{ MX25_PIN_IPP_UART4_RXD, -1, -1, 0x0000, 0x0000 },
	{ MX25_PIN_IPP_UART4_RTS, -1, -1, 0x0000, 0x0000 },
	{ },
};

static struct iomux_info uart5_pins[] = {
	{ MX25_PIN_CSI_D2, MUX_ALT1, MUX_ALT5, 0x00e0, 0x00c0 },
	{ MX25_PIN_CSI_D3, MUX_ALT1, MUX_ALT5, 0x0000, 0x00c0 },
	{ MX25_PIN_CSI_D4, MUX_ALT1, MUX_ALT5, 0x01e0, 0x01c0 },
	{ MX25_PIN_CSI_D5, MUX_ALT1, MUX_ALT5, 0x0000, 0x00c0 },
	{ MX25_PIN_IPP_UART5_RXD, -1, -1, 0x0001, 0x0001},
	{ MX25_PIN_IPP_UART5_RTS, -1, -1, 0x0001, 0x0001},
	{ },
};

struct iomux_info fec_pins[] = {
	{ MX25_PIN_FEC_TX_CLK, MUX_ALT0, MUX_ALT0, 0x01e0, 0x01e0 },
	{ MX25_PIN_FEC_RX_DV, MUX_ALT0, MUX_ALT0, 0x0040, 0x0040 },
	{ MX25_PIN_FEC_RDATA0, MUX_ALT0, MUX_ALT0, 0x0040, 0x0040 },
	{ MX25_PIN_FEC_RDATA1, MUX_ALT0, MUX_ALT0, 0x0040, 0x0040 },
	{ MX25_PIN_FEC_TDATA0, MUX_ALT0, MUX_ALT0, -1, -1 },
	{ MX25_PIN_FEC_TDATA1, MUX_ALT0, MUX_ALT0, -1, -1 },
	{ MX25_PIN_FEC_TX_EN, MUX_ALT0, MUX_ALT0, -1, -1 },
	{ MX25_PIN_FEC_MDC, MUX_ALT0, MUX_ALT0, -1, -1 },
	{ MX25_PIN_FEC_MDIO, MUX_ALT0, MUX_ALT0, 0x0170, 0x0170 },
	{ },
};

struct iomux_info fec_phy_reset_pin[] = {
	{ MX25_PIN_VSTBY_ACK, MUX_ALT5, MUX_ALT5, 0x0000, 0x0000 },
	{ },
};

struct iomux_info fec_led_pin[] = {
	/* GPIO: LINK LED (UPLL_BYPCLK) */
	{ MX25_PIN_UPLL_BYPCLK, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ },
};

struct iomux_info i2c1_pins[] = {
	{ MX25_PIN_I2C1_CLK, SION(MUX_ALT0), MUX_ALT5, 0x0028, 0x0028 },
	{ MX25_PIN_I2C1_DAT, SION(MUX_ALT0), MUX_ALT5, 0x0028, 0x0028 },
	{ },
};

struct iomux_info mmcsd1_pins[] = {
	{ MX25_PIN_SD1_CMD, SION(MUX_ALT0), SION(MUX_ALT0), 0x0051, 0x0051 },
	{ MX25_PIN_SD1_CLK, SION(MUX_ALT0), MUX_ALT5, 0x0051, 0x0051 },
	{ MX25_PIN_SD1_DATA0, MUX_ALT0, MUX_ALT0, 0x0051, 0x0051 },
	{ MX25_PIN_SD1_DATA1, MUX_ALT0, MUX_ALT0, 0x0051, 0x0051 },
	{ MX25_PIN_SD1_DATA2, MUX_ALT0, MUX_ALT0, 0x0051, 0x0051 },
	{ MX25_PIN_SD1_DATA3, MUX_ALT0, MUX_ALT0, 0x0051, 0x0051 },
	{ },
};

struct iomux_info mmcsd1_cd_pin[] = {
	{ MX25_PIN_NFRB, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ },
};

struct iomux_info mmcsd1_pwren_pin[] = {
	{ MX25_PIN_NFRE_B, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ },
};

struct iomux_info mmcsd2_pins[] = {
	{ MX25_PIN_CSI_D6, SION(MUX_ALT2), SION(MUX_ALT2), 0x0001, 0x0001 },
	{ MX25_PIN_CSI_D7, SION(MUX_ALT2), MUX_ALT5, 0x0001, 0x0051 },
	{ MX25_PIN_CSI_MCLK, MUX_ALT2, MUX_ALT2, 0x0001, 0x0001 },
	{ MX25_PIN_CSI_VSYNC, MUX_ALT2, MUX_ALT2, 0x0001, 0x0001 },
	{ MX25_PIN_CSI_HSYNC, MUX_ALT2, MUX_ALT2, 0x0001, 0x0001 },
	{ MX25_PIN_CSI_PIXCLK, MUX_ALT2, MUX_ALT2, 0x0101, 0x0101 },
	{ MX25_PIN_ESDHC2_IPP_CARD_CLK, -1, -1, 0x0001, 0x0001 },
	{ MX25_PIN_ESDHC2_IPP_CMD, -1, -1, 0x0001, 0x0001 },
	{ MX25_PIN_ESDHC2_IPP_DAT0, -1, -1, 0x0001, 0x0001 },
	{ MX25_PIN_ESDHC2_IPP_DAT1, -1, -1, 0x0001, 0x0001 },
	{ MX25_PIN_ESDHC2_IPP_DAT2, -1, -1, 0x0001, 0x0001 },
	{ MX25_PIN_ESDHC2_IPP_DAT3, -1, -1, 0x0001, 0x0001 },
	{ },
};

struct iomux_info mmcsd2_cd_pin[] = {
	{ MX25_PIN_CSI_D8, MUX_ALT5, MUX_ALT5, 0x00e0, 0x00e0 },
	{ },
};

struct iomux_info mmcsd2_pwren_pin[] = {
	{ MX25_PIN_DE_B, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ },
};

static struct iomux_info mmcsd1_pwren_pin_revb[] = {
	{ MX25_PIN_VSTBY_REQ, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ },
};

static struct iomux_info usb_pwrsel_pin[] = {
	{ MX25_PIN_NFWE_B, MUX_ALT5, MUX_ALT5, -1, -1 },
	{ },
};

static struct iomux_info i2c_gpio_pins[] = {	
	{ MX25_PIN_CSPI1_SS1, MUX_ALT5, MUX_ALT5, 0x00a0, 0x00a0 },	/* GPIO_SDA */
	{ MX25_PIN_CSPI1_SCLK, MUX_ALT5, MUX_ALT5, 0x00a0, 0x00a0 },	/* GPIO_SCL */
	{ },
};

static struct iomux_info misc_pins[] = {
	{ MX25_PIN_A13, MUX_ALT0, MUX_ALT0, 0x0000, 0x0000 },
	{ MX25_PIN_A14, MUX_ALT0, MUX_ALT0, 0x0000, 0x0000 },
	{ MX25_PIN_A15, MUX_ALT0, MUX_ALT0, 0x0000, 0x0000 },
	{ MX25_PIN_ECB, MUX_ALT0, MUX_ALT0, 0x2100, 0x2100 },
	{ MX25_PIN_TDO, -1, -1, 0x0000, 0x0000 },
	{ MX25_PIN_CTL_GRP_DSE_NFC, -1, -1, 0x0000, 0x0000 },
	{ MX25_PIN_CTL_GRP_DVS_NFC, -1, -1, 0x2000, 0x2000 },
	{ },
};

int mx25_uart_post_setup(int uart)
{
	switch (uart) {
	case 2:
		mx25_iomux_config(uart2_pins, uart == 2);
		break;
	default:
		return -H_EINVAL;
	}

	return 0;
}

static void armadillo_iotg_std_setup_iomux(struct platform_info *pinfo)
{
	mx25_iomux_config(led_pins, 1);
	mx25_iomux_config(wdt_pins, 1);
	mx25_iomux_config(uart1_pins, 0);
	mx25_iomux_config(uart2_pins, 1);
	mx25_iomux_config(uart3_pins, 0);
	mx25_iomux_config(uart4_pins, 0);
	mx25_iomux_config(uart5_pins, 0);
	mx25_iomux_config(fec_pins, 1);
	mx25_iomux_config(fec_phy_reset_pin, 1);
	mx25_iomux_config(fec_led_pin, 1);
	mx25_iomux_config(mmcsd1_pins, 0);
	mx25_gpio_output(mmcsd1_pins[1].pin, 0); /* SDCLK: Low */
	mx25_iomux_config(mmcsd1_cd_pin, 1);
	mx25_gpio_output(mmcsd1_pwren_pin[0].pin, 1); /* SD Power: Disable */
	mx25_iomux_config(mmcsd1_pwren_pin, 1);
	mx25_iomux_config(mmcsd2_pins, 0);
	mx25_gpio_output(mmcsd2_pins[1].pin, 0); /* SDCLK: Low */
	mx25_iomux_config(mmcsd2_cd_pin, 1);
	mx25_gpio_output(mmcsd2_pwren_pin[0].pin, 0); /* SD Power: Disable */
	mx25_iomux_config(mmcsd1_pwren_pin, 1);
	mx25_iomux_config(usb_pwrsel_pin, 1);
	mx25_iomux_config(misc_pins, 1);
}

static void armadillo_iotg_std_setup_i2c(struct platform_info *pinfo)
{
	mx25_setup_i2c(1);
}

static void armadillo_iotg_std_setup_i2c_gpio(struct platform_info *pinfo)
{
	int i;

	mx25_iomux_config(i2c_gpio_pins, 0);

	for (i=0; ; i++) {
		if (!i2c_gpio_pins[i].pin)
			break;

		mx25_gpio_input(i2c_gpio_pins[i].pin);
	}

	pca9538_detect();
}

static void armadillo_iotg_std_setup_gpio(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;
	int i;

	mx25_iomux_config(gpio_pins, 1);

	for (i=0; ; i++) {
		if (!gpio_pins[i].pin)
			break;

		if (gpio_pins[i].pin == MX25_PIN_CLKO ||
		    gpio_pins[i].pin == MX25_PIN_EXT_ARMCLK ||
		    gpio_pins[i].pin == MX25_PIN_LD6)
			mx25_gpio_output(gpio_pins[i].pin, 0);
		else
			mx25_gpio_input(gpio_pins[i].pin);
	}

	pca9538_gpio_direction_input(0);	/* SW2_CN */
	pca9538_gpio_direction_input(1);	/* SW3_CN */
	pca9538_gpio_direction_output(2, 0);	/* LED4 */
	pca9538_gpio_direction_output(3, 0);	/* LED5 */
	pca9538_gpio_direction_output(4, 0);	/* SD/AWLAN_SEL */
	pca9538_gpio_direction_output(5, 0);	/* EXT_USB_SEL */

	switch (priv->base_board_gen) {
	case BASE_BOARD_GEN1:
		pca9538_gpio_direction_output(6, 1);	/* TH15 */
		pca9538_gpio_direction_output(7, 1);	/* TH25 */
		break;
	case BASE_BOARD_GEN2:
		pca9538_gpio_direction_output(6, 0);	/* PWR_ON_3G */
		pca9538_gpio_direction_input(7);	/* GPIO12_3G */
		break;
	default:
		break;
	}
}

static void armadillo_iotg_std_setup_private_data(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;
	u32 val;
	u8 addr = 0;
	struct i2c_device i2c_dev = {
		.port = 1,
		.devid = 0xa0,
		.wait = 5,
	};
	struct i2c_msg msgs[] = {
		[0] = { &addr, 1, I2C_M_WR },
		[1] = { NULL, 6, I2C_M_RD | I2C_REPSTART },
	};

	/* DRAM IDs */
	mx25_iomux_config(ids_pins, 1);
	val = read32(GPIO2_BASE_ADDR + GPIO(GDIR));
	val &= ~0x00000c00; /* Dir: input */
	write32(GPIO2_BASE_ADDR + GPIO(GDIR), val);
	priv->dram = (read32(GPIO2_BASE_ADDR + GPIO(PSR)) & 0x00000c00)>>10;
	mx25_iomux_config(ids_pins, 0);

	mx25_iomux_config(jumper_pins, 1);
	mx25_iomux_config(sw_pins, 1);
	val = read32(GPIO3_BASE_ADDR + GPIO(GDIR));
	val &= ~0x40400000;
	write32(GPIO3_BASE_ADDR + GPIO(GDIR), val);
	val = read32(GPIO3_BASE_ADDR + GPIO(PSR));
	priv->jumper = val & 0x00400000 ? 1 : 0;
	priv->tactsw = val & 0x40000000 ? 1 : 0;

	/* MAC1 Address */
	msgs[1].buf = priv->mac1;
	mx25_i2c_xfer(&i2c_dev, msgs, 2);

	/* USER MAC1 Address */
	addr = 64;
	msgs[1].buf = priv->umac1;
	mx25_i2c_xfer(&i2c_dev, msgs, 2);

	/* Board Type */
	addr = 32;
	msgs[1].buf = (u8 *)&priv->type;
	msgs[1].len = 2;
	mx25_i2c_xfer(&i2c_dev, msgs, 2);

	/* Hardware Info */
	addr = 34;
	msgs[1].buf = (u8 *)&priv->hardware;
	mx25_i2c_xfer(&i2c_dev, msgs, 2);

	/* Base Board Generation */
	priv->base_board_gen = pca9538_get_slave_addr() & BASE_BOARD_GEN_MASK;
	pinfo->system_rev = ((u32)priv->hardware & ~0xffu) |
			    ((u32)priv->base_board_gen << 16);

	pinfo->machine_nr = MACH_ARMADILLO410;
}

static void armadillo_iotg_std_setup_watchdog(struct platform_info *pinfo)
{
#if defined(CONFIG_WDT_MX25)
	u32 val;

	val = read32(GPIO3_BASE_ADDR + GPIO(DR));
	val &= ~0x00080000;
	write32(GPIO3_BASE_ADDR + GPIO(DR), val);

	val = read32(GPIO3_BASE_ADDR + GPIO(GDIR));
	val |= 0x00080000;
	write32(GPIO3_BASE_ADDR + GPIO(GDIR), val);

	mx25_irq_init();
#endif
}

static void armadillo_iotg_std_setup_pmic(struct platform_info *pinfo)
{
	mc34704_write(2, 0x9);
}

static void armadillo_iotg_std_setup_ethernet(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;
	u8 *mac;

	if (is_valid_mac(priv->umac1))
		mac = priv->umac1;
	else
		mac = priv->mac1;

	/* Set MAC Address at FEC Physical address registers */
	write32(FEC_BASE_ADDR + FEC_PALR,
		((mac[0] << 24) | (mac[1] << 16) | (mac[2] << 8) | mac[3]));
	write32(FEC_BASE_ADDR + FEC_PAUR,
		((mac[4] << 24) | (mac[5] << 16)));

	pinfo->net_dev->base_addr = FEC_BASE_ADDR;

	mx25_iomux_config(fec_pins, 1);
	mx25_iomux_config(fec_phy_reset_pin, 1);
	mx25_iomux_config(fec_led_pin, 1);
	mx25_ahb_clock_enable(7);
	mx25_ipg_clock_enable(15);

	mx25_ahb_clock_enable(10);
	mx25_ipg_clock_enable(38);
}

static void armadillo_iotg_std_get_mac_address(struct platform_info *pinfo, u8 *addr)
{
	struct board_private *priv = pinfo->private_data;
	u8 *mac;

	if (is_valid_mac(priv->umac1))
		mac = priv->umac1;
	else
		mac = priv->mac1;

	memcpy(addr, mac, 6);
}

static void armadillo_iotg_std_udelay(struct platform_info *pinfo, u32 usecs)
{
	u32 base = EPIT1_BASE_ADDR;

	mx25_per_clock_enable(1);
	mx25_ipg_clock_enable(10);

	write32(base + EPIT(CR), 0x0102041a);
	write32(base + EPIT(SR), 0x01);
	write32(base + EPIT(CMPR), 0xffffffff - usecs);

	write32(base + EPIT(CR), 0x0102041b);

	while (!(read32(base + EPIT(SR)) & 0x1));

	write32(base + EPIT(SR), 0x01);

	write32(base + EPIT(CR), 0x0102041a);
}

static void armadillo_iotg_std_led(struct platform_info *pinfo, u32 leds, int mode)
{
	u32 dir, dr;

	dir = read32(GPIO3_BASE_ADDR + GPIO(GDIR));
	dr = read32(GPIO3_BASE_ADDR + GPIO(DR));

	if (leds & LED_RED) {
		dir |= 0x10000000;
		if (mode)
			dr |= 0x10000000;
		else
			dr &= ~0x10000000;
	}
	if (leds & LED_GREEN) {
		dir |= 0x20000000;
		if (mode)
			dr |= 0x20000000;
		else
			dr &= ~0x20000000;
	}

	write32(GPIO3_BASE_ADDR + GPIO(GDIR), dir);
	write32(GPIO3_BASE_ADDR + GPIO(DR), dr);
}

static void armadillo_iotg_std_led_on(struct platform_info *pinfo, u32 leds)
{
	armadillo_iotg_std_led(pinfo, leds, 1);
}

static void armadillo_iotg_std_led_off(struct platform_info *pinfo, u32 leds)
{
	armadillo_iotg_std_led(pinfo, leds, 0);
}

static struct page_table pt_list[] = {
	/* Physical,  Virtual,    Size,       Option */
	/*-------------------------------------------*/
	/* SDRAM */
	{0x80000000, 0x80000000, 0x00b00000, 0xc0e},
	{0x80b00000, 0x80b00000, 0x00100000, 0xc02}, /* for Ethernet */
	{0x80c00000, 0x80c00000, 0x00300000, 0xc0e},
	{0x80f00000, 0xfff00000, 0x00100000, 0xc0e}, /* for vector */
	{0x81000000, 0x81000000, 0x0f000000, 0xc0e},
	/* Flash */
	{0xa0000000, 0xa0000000, 0x08000000, 0xc02},
	/* Internal SRAM */
	{0x78000000, 0x78000000, 0x02000000, 0xc0e},
	/* Internal Register */
	{0x40000000, 0x40000000, 0x30000000, 0xc02},
	{0xb8000000, 0xb8000000, 0x01000000, 0xc02},
};

static int armadillo_iotg_std_get_mmu_page_tables(struct platform_info *pinfo,
					    u32 mode, struct page_table **pt,
					    int *nr_pt)
{
	*pt = (struct page_table *)&pt_list;
	*nr_pt = ARRAY_SIZE(pt_list);
	return 0;
}

static void armadillo_iotg_std_display_info(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;

	hprintf(" Board Type: %p\n"
		"Hardware ID: %p\n"
		"    DRAM ID: %p\n"
		"     Jumper: %p\n"
		"    Tact-SW: %p\n",
		priv->type, priv->hardware, priv->dram,
		priv->jumper, priv->tactsw);
	hprintf(" ORIG MAC-1: ");
	print_mac(priv->mac1);
	if (is_valid_mac(priv->umac1)) {
		hprintf(" USER MAC-1: ");
		print_mac(priv->umac1);
	}

	hprintf("Base Board Gen: %p\n", priv->base_board_gen);
}

static int armadillo_iotg_std_is_autoboot(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;
	int autoboot = priv->jumper;
#if defined(CONFIG_AUTOBOOT_CANCEL_BY_TACTSW)
	autoboot &= priv->tactsw;
#endif
	return autoboot;
}

static void armadillo_iotg_std_setup_flash(struct platform_info *pinfo)
{
	int val;

	flash_initialize(FLASH_TYPE_INTEL, FLASH_START);

	val = flash_get_size(FLASH_START);
	pinfo->map->flash.size = 1 << val;
	hsprintf(pinfo->default_mtdparts,
		 "armadillo_iotg_std-nor:%p(all)ro,0x20000@0(bootloader)ro,"
		 "0x400000(kernel),%p(userland),-(config)",
		 pinfo->map->flash.size, pinfo->map->flash.size - 0x520000);
}

static void armadillo_iotg_std_setup_map(struct platform_info *pinfo)
{
	struct board_private *priv = pinfo->private_data;
	size_t size;

	switch (priv->dram) {
	case 0x00: /* 32MB */
		size = 0x02000000;
		break;
	case 0x01: /* 64MB */
		size = 0x04000000;
/*** update memory map: RAM = 64MB, (FLA = 16MB) ****************/
/*	.initrd		= { RAM_ADDR(0x01000000), 0x02000000 }, */
/*	.free		= { RAM_ADDR(0x03000000), 0x01000000 }, */
/****************************************************************/
		pinfo->map->initrd.size = 0x02000000;
		pinfo->map->free.base = RAM_ADDR(0x03000000);
		pinfo->map->free.size = 0x01000000;
		break;
	case 0x03: /* 256MB */
		size = 0x10000000;
/*** update memory map: RAM = 256MB, (FLA = 64MB) ***************/
/*	.initrd		= { RAM_ADDR(0x01000000), 0x0b000000 }, */
/*	.free		= { RAM_ADDR(0x0c000000), 0x04000000 }, */
/****************************************************************/
		pinfo->map->initrd.size = 0x0b000000;
		pinfo->map->free.base = RAM_ADDR(0x0c000000);
		pinfo->map->free.size = 0x04000000;
		break;
	case 0x02: /* 128MB */
	default:
		size = 0x08000000;
		break;
	}
	pinfo->map->ram.size = size;
	hsprintf(pinfo->default_ramparts, "%p(dram-1)", size);
}

static void update_target_profile(void)
{
	char *ptr = target_name;

	ptr += hsprintf(ptr, "%s", CONFIG_PLATFORM_NAME);
	if (strlen(CONFIG_PROFILE_NAME))
		hsprintf(ptr, "/%s", CONFIG_PROFILE_NAME);
}

static void armadillo_iotg_std_setup_console(struct platform_info *pinfo)
{
	char console[128];
	int ret;
	int autoboot = armadillo_iotg_std_is_autoboot(pinfo);

#if defined(CONFIG_SYSTEM_ENTRYPOINT_FLASH) && defined(CONFIG_CMD_SETENV)
	ret = get_param_value("console", console, 128);
	if (ret == 0 && strcmp(console, "none") == 0)
		if (!autoboot)
			ret = 1;
	if (ret)
#endif
		hsprintf(console, "%s", CONFIG_DEFAULT_CONSOLE);

	/* we must set up to STANDARD_CONSOLE to match our hardware.
	   FIXME: this is redundant and just using a side effect of
	   the function to initialize it */
	change_console(CONFIG_STANDARD_CONSOLE);
	ret = change_console(console);
	if (ret && !autoboot)
		change_console(CONFIG_STANDARD_CONSOLE);
}

static void armadillo_iotg_std_rev_fixup(struct platform_info *pinfo)
{
	/* the default setting is for Rev.C. So, there is nothing to
	 * do here if the board revision is rev.c or later.
	 * system_rev:
	 *   Rev.B 0x02xx
	 *   Rev.C 0x03xx
	 */
	if ((pinfo->system_rev & CPU_BOARD_REV_MASK) < 0x0300) {
		memcpy(&mmcsd1_pwren_pin[0], &mmcsd1_pwren_pin_revb[0],
		       sizeof(struct iomux_info));
	}
}

static void armadillo_iotg_std_prepare_normal_boot(struct platform_info *pinfo)
{
}

static struct memory_map armadillo_iotg_std_memory_map = {
	.flash		= { FLASH_ADDR(0x00000000), 0},
	.param		= { FLASH_ADDR(0x00018000), 0x00008000 },

	/* default memory map: RAM = 128MB, (FLA = 32MB) */
	.ram		= { RAM_ADDR(0x00000000), 0 },
	.boot_param	= { RAM_ADDR(0x00000100), 0x00000f00 },
	.mmu_table	= { RAM_ADDR(0x00004000), 0x00004000 },
	.kernel		= { RAM_ADDR(0x00008000), 0x007f8000 },
/*	.hermit		= { RAM_ADDR(0x00800000), 0x00300000 }, */
/*	.fec_desc	= { RAM_ADDR(0x00b00000), 0x00100000 }, */
	.gunzip		= { RAM_ADDR(0x00c00000), 0x00100000 },
/*	.stack(irq)	= { RAM_ADDR(0x00d00000), 0x00100000 }, */
/*	.stack(svc)	= { RAM_ADDR(0x00e00000), 0x00100000 }, */
/*	.vector		= { RAM_ADDR(0x00f00000), 0x00100000 }, */
	.initrd		= { RAM_ADDR(0x01000000), 0x05000000 },
	.free		= { RAM_ADDR(0x06000000), 0x02000000 },
};

static void armadillo_iotg_std_init(void)
{
	struct platform_info *pinfo = &platform_info;

	armadillo_iotg_std_setup_i2c(pinfo);
	armadillo_iotg_std_setup_i2c_gpio(pinfo);
	armadillo_iotg_std_setup_private_data(pinfo);

	armadillo_iotg_std_rev_fixup(pinfo);

	armadillo_iotg_std_setup_iomux(pinfo);
	armadillo_iotg_std_setup_gpio(pinfo);
	armadillo_iotg_std_setup_flash(pinfo);
	armadillo_iotg_std_setup_map(pinfo);
	armadillo_iotg_std_setup_watchdog(pinfo);
	armadillo_iotg_std_setup_ethernet(pinfo);
	armadillo_iotg_std_setup_pmic(pinfo);

	update_target_profile();

	memdev_add(flash, &mdev_param,
		   "hermit/param", FLASH_ADDR(0x00018000), 0x00008000);
}
arch_initcall(armadillo_iotg_std_init);

static void armadillo_iotg_std_late_init(void)
{
	struct platform_info *pinfo = &platform_info;
	armadillo_iotg_std_setup_console(pinfo);
}
latearch_initcall(armadillo_iotg_std_late_init);

struct platform_info platform_info = {
	.prepare_normal_boot = armadillo_iotg_std_prepare_normal_boot,
	.is_autoboot = armadillo_iotg_std_is_autoboot,
	.led_on = armadillo_iotg_std_led_on,
	.led_off = armadillo_iotg_std_led_off,
	.udelay = armadillo_iotg_std_udelay,
	.get_mmu_page_tables = armadillo_iotg_std_get_mmu_page_tables,
	.display_info = armadillo_iotg_std_display_info,
	.get_mac_address = armadillo_iotg_std_get_mac_address,

	.private_data = &board_priv,
	.map = &armadillo_iotg_std_memory_map,

	.net_dev = &mx25_fec,
	.mmcsd_dev = &mx25_mmcsd,
};
