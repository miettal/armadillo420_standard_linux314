
#ifndef _MX3_IOMUXC_H_
#define _MX3_IOMUXC_H_

/* MUX config value */
#define MUX_O_GPIO	(0x00)
#define MUX_O_FUNC	(0x10)
#define MUX_O_ALT1	(0x20)
#define MUX_O_ALT2	(0x30)
#define MUX_O_ALT3	(0x40)
#define MUX_O_ALT4	(0x50)
#define MUX_O_ALT5	(0x60)
#define MUX_O_ALT6	(0x70)

#define MUX_I_NO	(0x00)
#define MUX_I_GPIO	(0x01)
#define MUX_I_FUNC	(0x02)
#define MUX_I_ALT1	(0x04)
#define MUX_I_ALT2	(0x08)

/* PAD control value */
#define PAD_CTL_PKE_ENABLE	(0x100)
#define PAD_CTL_PUE_PUD		(0x080)
#define PAD_CTL_100K_PU		(0x020)

/* 
 * MUX number
 *    0- 7 bit: addr + 0
 *    8-15 bit: addr + 1
 *   16-23 bit: addr + 2
 *   24-31 bit: addr + 3
 */
#define MUX_CTL_ATA_DMACK	(0x0014)
	#define MUX_CTL_xSD1_CD	(MUX_CTL_ATA_DMACK)
#define MUX_CTL_SD1_DATA3	(0x0019)
#define MUX_CTL_SD1_DATA2	(0x001a)
#define MUX_CTL_SD1_DATA1	(0x001b)
#define MUX_CTL_SD1_DATA0	(0x001c)
#define MUX_CTL_SD1_CLK		(0x001d)
#define MUX_CTL_SD1_CMD		(0x001e)
#define MUX_CTL_CONTRAST	(0x0022)
#define MUX_CTL_USB_BYP		(0x0055)
	#define MUX_CTL_xCPU3	(MUX_CTL_USB_BYP)
#define MUX_CTL_USB_OC		(0x0056)
	#define MUX_CTL_xCPU2	(MUX_CTL_USB_OC)
#define MUX_CTL_USB_PWR		(0x0057)
	#define MUX_CTL_xCPU1	(MUX_CTL_USB_PWR)
#define MUX_CTL_BATT_LINE	(0x0070)
	#define MUX_CTL_xLED5	(MUX_CTL_BATT_LINE)
#define MUX_CTL_CTS2		(0x0071)
#define MUX_CTL_RTS2		(0x0072)
#define MUX_CTL_TXD2		(0x0073)
#define MUX_CTL_RXD2		(0x0074)
#define MUX_CTL_CTS1		(0x007e)
#define MUX_CTL_RTS1		(0x007f)
#define MUX_CTL_TXD1		(0x0080)
#define MUX_CTL_RXD1		(0x0081)
#define MUX_CTL_CSPI2_MOSI	(0x0088)
#define MUX_CTL_PC_POE		(0x00b4)
#define MUX_CTL_PC_RW		(0x00b5)
#define MUX_CTL_IOIS16		(0x00b6)
#define MUX_CTL_PC_RST		(0x00b7)
#define MUX_CTL_PC_BVD2		(0x00b8)
#define MUX_CTL_PC_BVD1		(0x00b9)
#define MUX_CTL_PC_VS2		(0x00ba)
#define MUX_CTL_PC_VS1		(0x00bb)
#define MUX_CTL_PC_PWRON	(0x00bc)
#define MUX_CTL_PC_READY	(0x00bd)
#define MUX_CTL_PC_WAIT		(0x00be)
#define MUX_CTL_PC_CD2		(0x00bf)
#define MUX_CTL_PC_CD1		(0x00c0)
#define MUX_CTL_CS3		(0x00e9)
#define MUX_CTL_SDBA0		(0x0114)
#define MUX_CTL_SDBA1		(0x0115)
#define MUX_CTL_SIMPD0		(0x0141)
	#define MUX_CTL_xLED4	(MUX_CTL_SIMPD0)
	#define MUX_CTL_xID4	(MUX_CTL_SIMPD0)
	#define MUX_CTL_xLED_STATUS	(MUX_CTL_SIMPD0)
#define MUX_CTL_SRX0		(0x0142)
	#define MUX_CTL_xLED3	(MUX_CTL_SRX0)
	#define MUX_CTL_xID3	(MUX_CTL_SRX0)
#define MUX_CTL_STX0		(0x0143)
	#define MUX_CTL_xLED2	(MUX_CTL_STX0)
	#define MUX_CTL_xID2	(MUX_CTL_STX0)
#define MUX_CTL_SVEN0		(0x0144)
	#define MUX_CTL_xLED1	(MUX_CTL_SVEN0)
	#define MUX_CTL_xID1	(MUX_CTL_SVEN0)
#define MUX_CTL_GPIO1_1		(0x014e)
#define MUX_CTL_GPIO1_0		(0x014f)
#define MUX_CTL_PWMO		(0x0150)
	#define MUX_CTL_xCF_PWREN	(MUX_CTL_PWMO)
#define MUX_CTL_COMPARE		(0x0152)
	#define MUX_CTL_xJP2	(MUX_CTL_COMPARE)
#define MUX_CTL_CAPTURE		(0x0153)
	#define MUX_CTL_xJP1	(MUX_CTL_CAPTURE)
#define MUX_CTL_		()

/*
 * PAD number
 *    0- 9 bit: addr + 0
 *   10-19 bit: addr + 1
 *   20-29 bit: addr + 2
 */
#define PAD_CTL_CSI_D5		(0x0231)
#define PAD_CTL_CSI_D4		(0x0232)
#define PAD_CTL_		()

/*
 * GPIO number
 *   GPIO1:  0 + port
 *   GPIO2: 32 + port
 *   GPIO3: 64 + port
 */
#define GPIO_CTL_CAPTURE	(7)
	#define GPIO_CTL_xJP1	(GPIO_CTL_CAPTURE)
#define GPIO_CTL_COMPARE	(8)
	#define GPIO_CTL_xJP2	(GPIO_CTL_COMPARE)
#define GPIO_CTL_PWMO		(9)
	#define GPIO_CTL_xCF_PWREN	(GPIO_CTL_PWMO)
#define GPIO_CTL_USB_PWR	(29)
	#define GPIO_CTL_xCPU1	(GPIO_CTL_USB_PWR)
#define GPIO_CTL_USB_OC		(30)
	#define GPIO_CTL_xCPU2	(GPIO_CTL_USB_OC)
#define GPIO_CTL_USB_BYP	(31)
	#define GPIO_CTL_xCPU3	(GPIO_CTL_USB_BYP)
#define GPIO_CTL_SVEN0		(32)
	#define GPIO_CTL_xLED1	(GPIO_CTL_SVEN0)
	#define GPIO_CTL_xID1	(GPIO_CTL_SVEN0)
#define GPIO_CTL_STX0		(33)
	#define GPIO_CTL_xLED2	(GPIO_CTL_STX0)
	#define GPIO_CTL_xID2	(GPIO_CTL_STX0)
#define GPIO_CTL_SRX0		(34)
	#define GPIO_CTL_xLED3	(GPIO_CTL_SRX0)
	#define GPIO_CTL_xID3	(GPIO_CTL_SRX0)
#define GPIO_CTL_SIMPD0		(35)
	#define GPIO_CTL_xLED4		(GPIO_CTL_SIMPD0)
	#define GPIO_CTL_xID4		(GPIO_CTL_SIMPD0)
	#define GPIO_CTL_xLED_STATUS	(GPIO_CTL_SIMPD0)
#define GPIO_CTL_BATT_LINE	(49)
	#define GPIO_CTL_xLED5	(GPIO_CTL_BATT_LINE)
#define GPIO_CTL_ATA_DMACK	(64 + 30)
	#define GPIO_CTL_xSD1_CD	(GPIO_CTL_ATA_DMACK)
#define GPIO_CTL_		()


#define MUX_PIN(pin)  (MUX_CTL_##pin)
#define PAD_PIN(pin)  (PAD_CTL_##pin)
#define GPIO_PIN(pin) (GPIO_CTL_##pin)

extern void mxc_set_mux(u32 mux, u32 config);
extern u32 mxc_get_mux(u32 mux);
extern void mxc_set_pad(u32 pad, u32 config);

extern int mxc_get_gpio(u32 gpio);
extern void mxc_set_gpio(u32 gpio, u32 direction, u32 data);
#define GPIO_INPUT	(0)
#define GPIO_OUTPUT	(1)
#define GPIO_LOW	(0)
#define GPIO_HIGH	(1)

#endif
