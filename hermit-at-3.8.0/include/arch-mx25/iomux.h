#ifndef _HERMIT_ARCH_MX25_IOMUX_H_
#define _HERMIT_ARCH_MX25_IOMUX_H_

#include "mx25_pins.h"

#define IOMUXC_GPR1				(IOMUXC_BASE_ADDR + 0x0000)
#define IOMUXC_OBSERVE_INT_MUX			(IOMUXC_BASE_ADDR + 0x0004)
#define IOMUXC_SW_MUX_CTL_PAD_A10		(IOMUXC_BASE_ADDR + 0x0008)
#define IOMUXC_SW_MUX_CTL_PAD_A13		(IOMUXC_BASE_ADDR + 0x000c)
#define IOMUXC_SW_MUX_CTL_PAD_A14		(IOMUXC_BASE_ADDR + 0x0010)
#define IOMUXC_SW_MUX_CTL_PAD_A15		(IOMUXC_BASE_ADDR + 0x0014)
#define IOMUXC_SW_MUX_CTL_PAD_A16		(IOMUXC_BASE_ADDR + 0x0018)
#define IOMUXC_SW_MUX_CTL_PAD_A17		(IOMUXC_BASE_ADDR + 0x001c)
#define IOMUXC_SW_MUX_CTL_PAD_A18		(IOMUXC_BASE_ADDR + 0x0020)
#define IOMUXC_SW_MUX_CTL_PAD_A19		(IOMUXC_BASE_ADDR + 0x0024)
#define IOMUXC_SW_MUX_CTL_PAD_A20		(IOMUXC_BASE_ADDR + 0x0028)
#define IOMUXC_SW_MUX_CTL_PAD_A21		(IOMUXC_BASE_ADDR + 0x002c)
#define IOMUXC_SW_MUX_CTL_PAD_A22		(IOMUXC_BASE_ADDR + 0x0030)
#define IOMUXC_SW_MUX_CTL_PAD_A23		(IOMUXC_BASE_ADDR + 0x0034)
#define IOMUXC_SW_MUX_CTL_PAD_A24		(IOMUXC_BASE_ADDR + 0x0038)
#define IOMUXC_SW_MUX_CTL_PAD_A25		(IOMUXC_BASE_ADDR + 0x003c)
#define IOMUXC_SW_MUX_CTL_PAD_EB0		(IOMUXC_BASE_ADDR + 0x0040)
#define IOMUXC_SW_MUX_CTL_PAD_EB1		(IOMUXC_BASE_ADDR + 0x0044)
#define IOMUXC_SW_MUX_CTL_PAD_OE		(IOMUXC_BASE_ADDR + 0x0048)
#define IOMUXC_SW_MUX_CTL_PAD_CS0		(IOMUXC_BASE_ADDR + 0x004c)
#define IOMUXC_SW_MUX_CTL_PAD_CS1		(IOMUXC_BASE_ADDR + 0x0050)
#define IOMUXC_SW_MUX_CTL_PAD_CS4		(IOMUXC_BASE_ADDR + 0x0054)
#define IOMUXC_SW_MUX_CTL_PAD_CS5		(IOMUXC_BASE_ADDR + 0x0058)
#define IOMUXC_SW_MUX_CTL_PAD_NF_CE0		(IOMUXC_BASE_ADDR + 0x005c)
#define IOMUXC_SW_MUX_CTL_PAD_ECB		(IOMUXC_BASE_ADDR + 0x0060)
#define IOMUXC_SW_MUX_CTL_PAD_LBA		(IOMUXC_BASE_ADDR + 0x0064)
#define IOMUXC_SW_MUX_CTL_PAD_BCLK		(IOMUXC_BASE_ADDR + 0x0068)
#define IOMUXC_SW_MUX_CTL_PAD_RW		(IOMUXC_BASE_ADDR + 0x006c)
#define IOMUXC_SW_MUX_CTL_PAD_NFWE_B		(IOMUXC_BASE_ADDR + 0x0070)
#define IOMUXC_SW_MUX_CTL_PAD_NFRE_B		(IOMUXC_BASE_ADDR + 0x0074)
#define IOMUXC_SW_MUX_CTL_PAD_NFALE		(IOMUXC_BASE_ADDR + 0x0078)
#define IOMUXC_SW_MUX_CTL_PAD_NFCLE		(IOMUXC_BASE_ADDR + 0x007c)
#define IOMUXC_SW_MUX_CTL_PAD_NFWP_B		(IOMUXC_BASE_ADDR + 0x0080)
#define IOMUXC_SW_MUX_CTL_PAD_NFRB		(IOMUXC_BASE_ADDR + 0x0084)
#define IOMUXC_SW_MUX_CTL_PAD_D15		(IOMUXC_BASE_ADDR + 0x0088)
#define IOMUXC_SW_MUX_CTL_PAD_D14		(IOMUXC_BASE_ADDR + 0x008c)
#define IOMUXC_SW_MUX_CTL_PAD_D13		(IOMUXC_BASE_ADDR + 0x0090)
#define IOMUXC_SW_MUX_CTL_PAD_D12		(IOMUXC_BASE_ADDR + 0x0094)
#define IOMUXC_SW_MUX_CTL_PAD_D11		(IOMUXC_BASE_ADDR + 0x0098)
#define IOMUXC_SW_MUX_CTL_PAD_D10		(IOMUXC_BASE_ADDR + 0x009c)
#define IOMUXC_SW_MUX_CTL_PAD_D9		(IOMUXC_BASE_ADDR + 0x00a0)
#define IOMUXC_SW_MUX_CTL_PAD_D8		(IOMUXC_BASE_ADDR + 0x00a4)
#define IOMUXC_SW_MUX_CTL_PAD_D7		(IOMUXC_BASE_ADDR + 0x00a8)
#define IOMUXC_SW_MUX_CTL_PAD_D6		(IOMUXC_BASE_ADDR + 0x00ac)
#define IOMUXC_SW_MUX_CTL_PAD_D5		(IOMUXC_BASE_ADDR + 0x00b0)
#define IOMUXC_SW_MUX_CTL_PAD_D4		(IOMUXC_BASE_ADDR + 0x00b4)
#define IOMUXC_SW_MUX_CTL_PAD_D3		(IOMUXC_BASE_ADDR + 0x00b8)
#define IOMUXC_SW_MUX_CTL_PAD_D2		(IOMUXC_BASE_ADDR + 0x00bc)
#define IOMUXC_SW_MUX_CTL_PAD_D1		(IOMUXC_BASE_ADDR + 0x00c0)
#define IOMUXC_SW_MUX_CTL_PAD_D0		(IOMUXC_BASE_ADDR + 0x00c4)
#define IOMUXC_SW_MUX_CTL_PAD_LD0		(IOMUXC_BASE_ADDR + 0x00c8)
#define IOMUXC_SW_MUX_CTL_PAD_LD1		(IOMUXC_BASE_ADDR + 0x00cc)
#define IOMUXC_SW_MUX_CTL_PAD_LD2		(IOMUXC_BASE_ADDR + 0x00d0)
#define IOMUXC_SW_MUX_CTL_PAD_LD3		(IOMUXC_BASE_ADDR + 0x00d4)
#define IOMUXC_SW_MUX_CTL_PAD_LD4		(IOMUXC_BASE_ADDR + 0x00d8)
#define IOMUXC_SW_MUX_CTL_PAD_LD5		(IOMUXC_BASE_ADDR + 0x00dc)
#define IOMUXC_SW_MUX_CTL_PAD_LD6		(IOMUXC_BASE_ADDR + 0x00e0)
#define IOMUXC_SW_MUX_CTL_PAD_LD7		(IOMUXC_BASE_ADDR + 0x00e4)
#define IOMUXC_SW_MUX_CTL_PAD_LD8		(IOMUXC_BASE_ADDR + 0x00e8)
#define IOMUXC_SW_MUX_CTL_PAD_LD9		(IOMUXC_BASE_ADDR + 0x00ec)
#define IOMUXC_SW_MUX_CTL_PAD_LD10		(IOMUXC_BASE_ADDR + 0x00f0)
#define IOMUXC_SW_MUX_CTL_PAD_LD11		(IOMUXC_BASE_ADDR + 0x00f4)
#define IOMUXC_SW_MUX_CTL_PAD_LD12		(IOMUXC_BASE_ADDR + 0x00f8)
#define IOMUXC_SW_MUX_CTL_PAD_LD13		(IOMUXC_BASE_ADDR + 0x00fc)
#define IOMUXC_SW_MUX_CTL_PAD_LD14		(IOMUXC_BASE_ADDR + 0x0100)
#define IOMUXC_SW_MUX_CTL_PAD_LD15		(IOMUXC_BASE_ADDR + 0x0104)
#define IOMUXC_SW_MUX_CTL_PAD_HSYNC		(IOMUXC_BASE_ADDR + 0x0108)
#define IOMUXC_SW_MUX_CTL_PAD_VSYNC		(IOMUXC_BASE_ADDR + 0x010c)
#define IOMUXC_SW_MUX_CTL_PAD_LSCLK		(IOMUXC_BASE_ADDR + 0x0110)
#define IOMUXC_SW_MUX_CTL_PAD_OE_ACD		(IOMUXC_BASE_ADDR + 0x0114)
#define IOMUXC_SW_MUX_CTL_PAD_CONTRAST		(IOMUXC_BASE_ADDR + 0x0118)
#define IOMUXC_SW_MUX_CTL_PAD_PWM		(IOMUXC_BASE_ADDR + 0x011c)
#define IOMUXC_SW_MUX_CTL_PAD_CSI_D2		(IOMUXC_BASE_ADDR + 0x0120)
#define IOMUXC_SW_MUX_CTL_PAD_CSI_D3		(IOMUXC_BASE_ADDR + 0x0124)
#define IOMUXC_SW_MUX_CTL_PAD_CSI_D4		(IOMUXC_BASE_ADDR + 0x0128)
#define IOMUXC_SW_MUX_CTL_PAD_CSI_D5		(IOMUXC_BASE_ADDR + 0x012c)
#define IOMUXC_SW_MUX_CTL_PAD_CSI_D6		(IOMUXC_BASE_ADDR + 0x0130)
#define IOMUXC_SW_MUX_CTL_PAD_CSI_D7		(IOMUXC_BASE_ADDR + 0x0134)
#define IOMUXC_SW_MUX_CTL_PAD_CSI_D8		(IOMUXC_BASE_ADDR + 0x0138)
#define IOMUXC_SW_MUX_CTL_PAD_CSI_D9		(IOMUXC_BASE_ADDR + 0x013c)
#define IOMUXC_SW_MUX_CTL_PAD_CSI_MCLK		(IOMUXC_BASE_ADDR + 0x0140)
#define IOMUXC_SW_MUX_CTL_PAD_CSI_VSYNC		(IOMUXC_BASE_ADDR + 0x0144)
#define IOMUXC_SW_MUX_CTL_PAD_CSI_HSYNC		(IOMUXC_BASE_ADDR + 0x0148)
#define IOMUXC_SW_MUX_CTL_PAD_CSI_PIXCLK	(IOMUXC_BASE_ADDR + 0x014c)
#define IOMUXC_SW_MUX_CTL_PAD_I2C1_CLK		(IOMUXC_BASE_ADDR + 0x0150)
#define IOMUXC_SW_MUX_CTL_PAD_I2C1_DAT		(IOMUXC_BASE_ADDR + 0x0154)
#define IOMUXC_SW_MUX_CTL_PAD_CSPI1_MOSI	(IOMUXC_BASE_ADDR + 0x0158)
#define IOMUXC_SW_MUX_CTL_PAD_CSPI1_MISO	(IOMUXC_BASE_ADDR + 0x015c)
#define IOMUXC_SW_MUX_CTL_PAD_CSPI1_SS0		(IOMUXC_BASE_ADDR + 0x0160)
#define IOMUXC_SW_MUX_CTL_PAD_CSPI1_SS1		(IOMUXC_BASE_ADDR + 0x0164)
#define IOMUXC_SW_MUX_CTL_PAD_CSPI1_SCLK	(IOMUXC_BASE_ADDR + 0x0168)
#define IOMUXC_SW_MUX_CTL_PAD_CSPI1_RDY		(IOMUXC_BASE_ADDR + 0x016c)
#define IOMUXC_SW_MUX_CTL_PAD_UART1_RXD		(IOMUXC_BASE_ADDR + 0x0170)
#define IOMUXC_SW_MUX_CTL_PAD_UART1_TXD		(IOMUXC_BASE_ADDR + 0x0174)
#define IOMUXC_SW_MUX_CTL_PAD_UART1_RTS		(IOMUXC_BASE_ADDR + 0x0178)
#define IOMUXC_SW_MUX_CTL_PAD_UART1_CTS		(IOMUXC_BASE_ADDR + 0x017c)
#define IOMUXC_SW_MUX_CTL_PAD_UART2_RXD		(IOMUXC_BASE_ADDR + 0x0180)
#define IOMUXC_SW_MUX_CTL_PAD_UART2_TXD		(IOMUXC_BASE_ADDR + 0x0184)
#define IOMUXC_SW_MUX_CTL_PAD_UART2_RTS		(IOMUXC_BASE_ADDR + 0x0188)
#define IOMUXC_SW_MUX_CTL_PAD_UART2_CTS		(IOMUXC_BASE_ADDR + 0x018c)
#define IOMUXC_SW_MUX_CTL_PAD_SD1_CMD		(IOMUXC_BASE_ADDR + 0x0190)
#define IOMUXC_SW_MUX_CTL_PAD_SD1_CLK		(IOMUXC_BASE_ADDR + 0x0194)
#define IOMUXC_SW_MUX_CTL_PAD_SD1_DATA0		(IOMUXC_BASE_ADDR + 0x0198)
#define IOMUXC_SW_MUX_CTL_PAD_SD1_DATA1		(IOMUXC_BASE_ADDR + 0x019c)
#define IOMUXC_SW_MUX_CTL_PAD_SD1_DATA2		(IOMUXC_BASE_ADDR + 0x01a0)
#define IOMUXC_SW_MUX_CTL_PAD_SD1_DATA3		(IOMUXC_BASE_ADDR + 0x01a4)
#define IOMUXC_SW_MUX_CTL_PAD_KPP_ROW0		(IOMUXC_BASE_ADDR + 0x01a8)
#define IOMUXC_SW_MUX_CTL_PAD_KPP_ROW1		(IOMUXC_BASE_ADDR + 0x01ac)
#define IOMUXC_SW_MUX_CTL_PAD_KPP_ROW2		(IOMUXC_BASE_ADDR + 0x01b0)
#define IOMUXC_SW_MUX_CTL_PAD_KPP_ROW3		(IOMUXC_BASE_ADDR + 0x01b4)
#define IOMUXC_SW_MUX_CTL_PAD_KPP_COL0		(IOMUXC_BASE_ADDR + 0x01b8)
#define IOMUXC_SW_MUX_CTL_PAD_KPP_COL1		(IOMUXC_BASE_ADDR + 0x01bc)
#define IOMUXC_SW_MUX_CTL_PAD_KPP_COL2		(IOMUXC_BASE_ADDR + 0x01c0)
#define IOMUXC_SW_MUX_CTL_PAD_KPP_COL3		(IOMUXC_BASE_ADDR + 0x01c4)
#define IOMUXC_SW_MUX_CTL_PAD_FEC_MDC		(IOMUXC_BASE_ADDR + 0x01c8)
#define IOMUXC_SW_MUX_CTL_PAD_FEC_MDIO		(IOMUXC_BASE_ADDR + 0x01cc)
#define IOMUXC_SW_MUX_CTL_PAD_FEC_TDATA0	(IOMUXC_BASE_ADDR + 0x01d0)
#define IOMUXC_SW_MUX_CTL_PAD_FEC_TDATA1	(IOMUXC_BASE_ADDR + 0x01d4)
#define IOMUXC_SW_MUX_CTL_PAD_FEC_TX_EN		(IOMUXC_BASE_ADDR + 0x01d8)
#define IOMUXC_SW_MUX_CTL_PAD_FEC_RDATA0	(IOMUXC_BASE_ADDR + 0x01dc)
#define IOMUXC_SW_MUX_CTL_PAD_FEC_RDATA1	(IOMUXC_BASE_ADDR + 0x01e0)
#define IOMUXC_SW_MUX_CTL_PAD_FEC_RX_DV		(IOMUXC_BASE_ADDR + 0x01e4)
#define IOMUXC_SW_MUX_CTL_PAD_FEC_TX_CLK	(IOMUXC_BASE_ADDR + 0x01e8)
#define IOMUXC_SW_MUX_CTL_PAD_RTCK		(IOMUXC_BASE_ADDR + 0x01ec)
#define IOMUXC_SW_MUX_CTL_PAD_DE_B		(IOMUXC_BASE_ADDR + 0x01f0)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_A		(IOMUXC_BASE_ADDR + 0x01f4)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_B		(IOMUXC_BASE_ADDR + 0x01f8)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_C		(IOMUXC_BASE_ADDR + 0x01fc)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_D		(IOMUXC_BASE_ADDR + 0x0200)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_E		(IOMUXC_BASE_ADDR + 0x0204)
#define IOMUXC_SW_MUX_CTL_PAD_GPIO_F		(IOMUXC_BASE_ADDR + 0x0208)
#define IOMUXC_SW_MUX_CTL_PAD_EXT_ARMCLK	(IOMUXC_BASE_ADDR + 0x020c)
#define IOMUXC_SW_MUX_CTL_PAD_UPLL_BYPCLK	(IOMUXC_BASE_ADDR + 0x0210)
#define IOMUXC_SW_MUX_CTL_PAD_VSTBY_REQ		(IOMUXC_BASE_ADDR + 0x0214)
#define IOMUXC_SW_MUX_CTL_PAD_VSTBY_ACK		(IOMUXC_BASE_ADDR + 0x0218)
#define IOMUXC_SW_MUX_CTL_PAD_POWER_FAIL	(IOMUXC_BASE_ADDR + 0x021c)
#define IOMUXC_SW_MUX_CTL_PAD_CLKO		(IOMUXC_BASE_ADDR + 0x0220)
#define IOMUXC_SW_MUX_CTL_PAD_BOOT_MODE0	(IOMUXC_BASE_ADDR + 0x0224)
#define IOMUXC_SW_MUX_CTL_PAD_BOOT_MODE1	(IOMUXC_BASE_ADDR + 0x0228)
#define IOMUXC_SW_PAD_CTL_PAD_A13		(IOMUXC_BASE_ADDR + 0x022c)
#define IOMUXC_SW_PAD_CTL_PAD_A14		(IOMUXC_BASE_ADDR + 0x0230)
#define IOMUXC_SW_PAD_CTL_PAD_A15		(IOMUXC_BASE_ADDR + 0x0234)
#define IOMUXC_SW_PAD_CTL_PAD_A17		(IOMUXC_BASE_ADDR + 0x0238)
#define IOMUXC_SW_PAD_CTL_PAD_A18		(IOMUXC_BASE_ADDR + 0x023c)
#define IOMUXC_SW_PAD_CTL_PAD_A19		(IOMUXC_BASE_ADDR + 0x0240)
#define IOMUXC_SW_PAD_CTL_PAD_A20		(IOMUXC_BASE_ADDR + 0x0244)
#define IOMUXC_SW_PAD_CTL_PAD_A21		(IOMUXC_BASE_ADDR + 0x0248)
#define IOMUXC_SW_PAD_CTL_PAD_A23		(IOMUXC_BASE_ADDR + 0x024c)
#define IOMUXC_SW_PAD_CTL_PAD_A24		(IOMUXC_BASE_ADDR + 0x0250)
#define IOMUXC_SW_PAD_CTL_PAD_A25		(IOMUXC_BASE_ADDR + 0x0254)
#define IOMUXC_SW_PAD_CTL_PAD_EB0		(IOMUXC_BASE_ADDR + 0x0258)
#define IOMUXC_SW_PAD_CTL_PAD_EB1		(IOMUXC_BASE_ADDR + 0x025c)
#define IOMUXC_SW_PAD_CTL_PAD_OE		(IOMUXC_BASE_ADDR + 0x0260)
#define IOMUXC_SW_PAD_CTL_PAD_CS4		(IOMUXC_BASE_ADDR + 0x0264)
#define IOMUXC_SW_PAD_CTL_PAD_CS5		(IOMUXC_BASE_ADDR + 0x0268)
#define IOMUXC_SW_PAD_CTL_PAD_NF_CE0		(IOMUXC_BASE_ADDR + 0x026c)
#define IOMUXC_SW_PAD_CTL_PAD_ECB		(IOMUXC_BASE_ADDR + 0x0270)
#define IOMUXC_SW_PAD_CTL_PAD_LBA		(IOMUXC_BASE_ADDR + 0x0274)
#define IOMUXC_SW_PAD_CTL_PAD_RW		(IOMUXC_BASE_ADDR + 0x0278)
#define IOMUXC_SW_PAD_CTL_PAD_NFRB		(IOMUXC_BASE_ADDR + 0x027c)
#define IOMUXC_SW_PAD_CTL_PAD_D15		(IOMUXC_BASE_ADDR + 0x0280)
#define IOMUXC_SW_PAD_CTL_PAD_D14		(IOMUXC_BASE_ADDR + 0x0284)
#define IOMUXC_SW_PAD_CTL_PAD_D13		(IOMUXC_BASE_ADDR + 0x0288)
#define IOMUXC_SW_PAD_CTL_PAD_D12		(IOMUXC_BASE_ADDR + 0x028c)
#define IOMUXC_SW_PAD_CTL_PAD_D11		(IOMUXC_BASE_ADDR + 0x0290)
#define IOMUXC_SW_PAD_CTL_PAD_D10		(IOMUXC_BASE_ADDR + 0x0294)
#define IOMUXC_SW_PAD_CTL_PAD_D9		(IOMUXC_BASE_ADDR + 0x0298)
#define IOMUXC_SW_PAD_CTL_PAD_D8		(IOMUXC_BASE_ADDR + 0x029c)
#define IOMUXC_SW_PAD_CTL_PAD_D7		(IOMUXC_BASE_ADDR + 0x02a0)
#define IOMUXC_SW_PAD_CTL_PAD_D6		(IOMUXC_BASE_ADDR + 0x02a4)
#define IOMUXC_SW_PAD_CTL_PAD_D5		(IOMUXC_BASE_ADDR + 0x02a8)
#define IOMUXC_SW_PAD_CTL_PAD_D4		(IOMUXC_BASE_ADDR + 0x02ac)
#define IOMUXC_SW_PAD_CTL_PAD_D3		(IOMUXC_BASE_ADDR + 0x02b0)
#define IOMUXC_SW_PAD_CTL_PAD_D2		(IOMUXC_BASE_ADDR + 0x02b4)
#define IOMUXC_SW_PAD_CTL_PAD_D1		(IOMUXC_BASE_ADDR + 0x02b8)
#define IOMUXC_SW_PAD_CTL_PAD_D0		(IOMUXC_BASE_ADDR + 0x02bc)
#define IOMUXC_SW_PAD_CTL_PAD_LD0		(IOMUXC_BASE_ADDR + 0x02c0)
#define IOMUXC_SW_PAD_CTL_PAD_LD1		(IOMUXC_BASE_ADDR + 0x02c4)
#define IOMUXC_SW_PAD_CTL_PAD_LD2		(IOMUXC_BASE_ADDR + 0x02c8)
#define IOMUXC_SW_PAD_CTL_PAD_LD3		(IOMUXC_BASE_ADDR + 0x02cc)
#define IOMUXC_SW_PAD_CTL_PAD_LD4		(IOMUXC_BASE_ADDR + 0x02d0)
#define IOMUXC_SW_PAD_CTL_PAD_LD5		(IOMUXC_BASE_ADDR + 0x02d4)
#define IOMUXC_SW_PAD_CTL_PAD_LD6		(IOMUXC_BASE_ADDR + 0x02d8)
#define IOMUXC_SW_PAD_CTL_PAD_LD7		(IOMUXC_BASE_ADDR + 0x02dc)
#define IOMUXC_SW_PAD_CTL_PAD_LD8		(IOMUXC_BASE_ADDR + 0x02e0)
#define IOMUXC_SW_PAD_CTL_PAD_LD9		(IOMUXC_BASE_ADDR + 0x02e4)
#define IOMUXC_SW_PAD_CTL_PAD_LD10		(IOMUXC_BASE_ADDR + 0x02e8)
#define IOMUXC_SW_PAD_CTL_PAD_LD11		(IOMUXC_BASE_ADDR + 0x02ec)
#define IOMUXC_SW_PAD_CTL_PAD_LD12		(IOMUXC_BASE_ADDR + 0x02f0)
#define IOMUXC_SW_PAD_CTL_PAD_LD13		(IOMUXC_BASE_ADDR + 0x02f4)
#define IOMUXC_SW_PAD_CTL_PAD_LD14		(IOMUXC_BASE_ADDR + 0x02f8)
#define IOMUXC_SW_PAD_CTL_PAD_LD15		(IOMUXC_BASE_ADDR + 0x02fc)
#define IOMUXC_SW_PAD_CTL_PAD_HSYNC		(IOMUXC_BASE_ADDR + 0x0300)
#define IOMUXC_SW_PAD_CTL_PAD_VSYNC		(IOMUXC_BASE_ADDR + 0x0304)
#define IOMUXC_SW_PAD_CTL_PAD_LSCLK		(IOMUXC_BASE_ADDR + 0x0308)
#define IOMUXC_SW_PAD_CTL_PAD_OE_ACD		(IOMUXC_BASE_ADDR + 0x030c)
#define IOMUXC_SW_PAD_CTL_PAD_CONTRAST		(IOMUXC_BASE_ADDR + 0x0310)
#define IOMUXC_SW_PAD_CTL_PAD_PWM		(IOMUXC_BASE_ADDR + 0x0314)
#define IOMUXC_SW_PAD_CTL_PAD_CSI_D2		(IOMUXC_BASE_ADDR + 0x0318)
#define IOMUXC_SW_PAD_CTL_PAD_CSI_D3		(IOMUXC_BASE_ADDR + 0x031c)
#define IOMUXC_SW_PAD_CTL_PAD_CSI_D4		(IOMUXC_BASE_ADDR + 0x0320)
#define IOMUXC_SW_PAD_CTL_PAD_CSI_D5		(IOMUXC_BASE_ADDR + 0x0324)
#define IOMUXC_SW_PAD_CTL_PAD_CSI_D6		(IOMUXC_BASE_ADDR + 0x0328)
#define IOMUXC_SW_PAD_CTL_PAD_CSI_D7		(IOMUXC_BASE_ADDR + 0x032c)
#define IOMUXC_SW_PAD_CTL_PAD_CSI_D8		(IOMUXC_BASE_ADDR + 0x0330)
#define IOMUXC_SW_PAD_CTL_PAD_CSI_D9		(IOMUXC_BASE_ADDR + 0x0334)
#define IOMUXC_SW_PAD_CTL_PAD_CSI_MCLK		(IOMUXC_BASE_ADDR + 0x0338)
#define IOMUXC_SW_PAD_CTL_PAD_CSI_VSYNC		(IOMUXC_BASE_ADDR + 0x033c)
#define IOMUXC_SW_PAD_CTL_PAD_CSI_HSYNC		(IOMUXC_BASE_ADDR + 0x0340)
#define IOMUXC_SW_PAD_CTL_PAD_CSI_PIXCLK	(IOMUXC_BASE_ADDR + 0x0344)
#define IOMUXC_SW_PAD_CTL_PAD_I2C1_CLK		(IOMUXC_BASE_ADDR + 0x0348)
#define IOMUXC_SW_PAD_CTL_PAD_I2C1_DAT		(IOMUXC_BASE_ADDR + 0x034c)
#define IOMUXC_SW_PAD_CTL_PAD_CSPI1_MOSI	(IOMUXC_BASE_ADDR + 0x0350)
#define IOMUXC_SW_PAD_CTL_PAD_CSPI1_MISO	(IOMUXC_BASE_ADDR + 0x0354)
#define IOMUXC_SW_PAD_CTL_PAD_CSPI1_SS0		(IOMUXC_BASE_ADDR + 0x0358)
#define IOMUXC_SW_PAD_CTL_PAD_CSPI1_SS1		(IOMUXC_BASE_ADDR + 0x035c)
#define IOMUXC_SW_PAD_CTL_PAD_CSPI1_SCLK	(IOMUXC_BASE_ADDR + 0x0360)
#define IOMUXC_SW_PAD_CTL_PAD_CSPI1_RDY		(IOMUXC_BASE_ADDR + 0x0364)
#define IOMUXC_SW_PAD_CTL_PAD_UART1_RXD		(IOMUXC_BASE_ADDR + 0x0368)
#define IOMUXC_SW_PAD_CTL_PAD_UART1_TXD		(IOMUXC_BASE_ADDR + 0x036c)
#define IOMUXC_SW_PAD_CTL_PAD_UART1_RTS		(IOMUXC_BASE_ADDR + 0x0370)
#define IOMUXC_SW_PAD_CTL_PAD_UART1_CTS		(IOMUXC_BASE_ADDR + 0x0374)
#define IOMUXC_SW_PAD_CTL_PAD_UART2_RXD		(IOMUXC_BASE_ADDR + 0x0378)
#define IOMUXC_SW_PAD_CTL_PAD_UART2_TXD		(IOMUXC_BASE_ADDR + 0x037c)
#define IOMUXC_SW_PAD_CTL_PAD_UART2_RTS		(IOMUXC_BASE_ADDR + 0x0380)
#define IOMUXC_SW_PAD_CTL_PAD_UART2_CTS		(IOMUXC_BASE_ADDR + 0x0384)
#define IOMUXC_SW_PAD_CTL_PAD_SD1_CMD		(IOMUXC_BASE_ADDR + 0x0388)
#define IOMUXC_SW_PAD_CTL_PAD_SD1_CLK		(IOMUXC_BASE_ADDR + 0x038c)
#define IOMUXC_SW_PAD_CTL_PAD_SD1_DATA0		(IOMUXC_BASE_ADDR + 0x0390)
#define IOMUXC_SW_PAD_CTL_PAD_SD1_DATA1		(IOMUXC_BASE_ADDR + 0x0394)
#define IOMUXC_SW_PAD_CTL_PAD_SD1_DATA2		(IOMUXC_BASE_ADDR + 0x0398)
#define IOMUXC_SW_PAD_CTL_PAD_SD1_DATA3		(IOMUXC_BASE_ADDR + 0x039c)
#define IOMUXC_SW_PAD_CTL_PAD_KPP_ROW0		(IOMUXC_BASE_ADDR + 0x03a0)
#define IOMUXC_SW_PAD_CTL_PAD_KPP_ROW1		(IOMUXC_BASE_ADDR + 0x03a4)
#define IOMUXC_SW_PAD_CTL_PAD_KPP_ROW2		(IOMUXC_BASE_ADDR + 0x03a8)
#define IOMUXC_SW_PAD_CTL_PAD_KPP_ROW3		(IOMUXC_BASE_ADDR + 0x03ac)
#define IOMUXC_SW_PAD_CTL_PAD_KPP_COL0		(IOMUXC_BASE_ADDR + 0x03b0)
#define IOMUXC_SW_PAD_CTL_PAD_KPP_COL1		(IOMUXC_BASE_ADDR + 0x03b4)
#define IOMUXC_SW_PAD_CTL_PAD_KPP_COL2		(IOMUXC_BASE_ADDR + 0x03b8)
#define IOMUXC_SW_PAD_CTL_PAD_KPP_COL3		(IOMUXC_BASE_ADDR + 0x03bc)
#define IOMUXC_SW_PAD_CTL_PAD_FEC_MDC		(IOMUXC_BASE_ADDR + 0x03c0)
#define IOMUXC_SW_PAD_CTL_PAD_FEC_MDIO		(IOMUXC_BASE_ADDR + 0x03c4)
#define IOMUXC_SW_PAD_CTL_PAD_FEC_TDATA0	(IOMUXC_BASE_ADDR + 0x03c8)
#define IOMUXC_SW_PAD_CTL_PAD_FEC_TDATA1	(IOMUXC_BASE_ADDR + 0x03cc)
#define IOMUXC_SW_PAD_CTL_PAD_FEC_TX_EN		(IOMUXC_BASE_ADDR + 0x03d0)
#define IOMUXC_SW_PAD_CTL_PAD_FEC_RDATA0	(IOMUXC_BASE_ADDR + 0x03d4)
#define IOMUXC_SW_PAD_CTL_PAD_FEC_RDATA1	(IOMUXC_BASE_ADDR + 0x03d8)
#define IOMUXC_SW_PAD_CTL_PAD_FEC_RX_DV		(IOMUXC_BASE_ADDR + 0x03dc)
#define IOMUXC_SW_PAD_CTL_PAD_FEC_TX_CLK	(IOMUXC_BASE_ADDR + 0x03e0)
#define IOMUXC_SW_PAD_CTL_PAD_RTCK		(IOMUXC_BASE_ADDR + 0x03e4)
#define IOMUXC_SW_PAD_CTL_PAD_TDO		(IOMUXC_BASE_ADDR + 0x03e8)
#define IOMUXC_SW_PAD_CTL_PAD_DE_B		(IOMUXC_BASE_ADDR + 0x03ec)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_A		(IOMUXC_BASE_ADDR + 0x03f0)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_B		(IOMUXC_BASE_ADDR + 0x03f4)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_C		(IOMUXC_BASE_ADDR + 0x03f8)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_D		(IOMUXC_BASE_ADDR + 0x03fc)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_E		(IOMUXC_BASE_ADDR + 0x0400)
#define IOMUXC_SW_PAD_CTL_PAD_GPIO_F		(IOMUXC_BASE_ADDR + 0x0404)
#define IOMUXC_SW_PAD_CTL_PAD_VSTBY_REQ		(IOMUXC_BASE_ADDR + 0x0408)
#define IOMUXC_SW_PAD_CTL_PAD_VSTBY_ACK		(IOMUXC_BASE_ADDR + 0x040c)
#define IOMUXC_SW_PAD_CTL_PAD_POWER_FAIL	(IOMUXC_BASE_ADDR + 0x0410)
#define IOMUXC_SW_PAD_CTL_PAD_CLKO		(IOMUXC_BASE_ADDR + 0x0414)
#define IOMUXC_SW_PAD_CTL_GRP_DVS_MISC		(IOMUXC_BASE_ADDR + 0x0418)
#define IOMUXC_SW_PAD_CTL_GRP_DSE_FEC		(IOMUXC_BASE_ADDR + 0x041c)
#define IOMUXC_SW_PAD_CTL_GRP_DVS_JTAG		(IOMUXC_BASE_ADDR + 0x0420)
#define IOMUXC_SW_PAD_CTL_GRP_DSE_NFC		(IOMUXC_BASE_ADDR + 0x0424)
#define IOMUXC_SW_PAD_CTL_GRP_DSE_CSI		(IOMUXC_BASE_ADDR + 0x0428)
#define IOMUXC_SW_PAD_CTL_GRP_DSE_WEIM		(IOMUXC_BASE_ADDR + 0x042c)
#define IOMUXC_SW_PAD_CTL_GRP_DSE_DDR		(IOMUXC_BASE_ADDR + 0x0430)
#define IOMUXC_SW_PAD_CTL_GRP_DVS_CCM		(IOMUXC_BASE_ADDR + 0x0434)
#define IOMUXC_SW_PAD_CTL_GRP_DSE_KPP		(IOMUXC_BASE_ADDR + 0x0438)
#define IOMUXC_SW_PAD_CTL_GRP_DSE_SDHC1		(IOMUXC_BASE_ADDR + 0x043c)
#define IOMUXC_SW_PAD_CTL_GRP_DSE_LCD		(IOMUXC_BASE_ADDR + 0x0440)
#define IOMUXC_SW_PAD_CTL_GRP_DSE_UART		(IOMUXC_BASE_ADDR + 0x0444)
#define IOMUXC_SW_PAD_CTL_GRP_DVS_NFC		(IOMUXC_BASE_ADDR + 0x0448)
#define IOMUXC_SW_PAD_CTL_GRP_DVS_CSI		(IOMUXC_BASE_ADDR + 0x044c)
#define IOMUXC_SW_PAD_CTL_GRP_DSE_CSPI1		(IOMUXC_BASE_ADDR + 0x0450)
#define IOMUXC_SW_PAD_CTL_GRP_DDRTYPE		(IOMUXC_BASE_ADDR + 0x0454)
#define IOMUXC_SW_PAD_CTL_GRP_DVS_SDHC1		(IOMUXC_BASE_ADDR + 0x0458)
#define IOMUXC_SW_PAD_CTL_GRP_DVS_LCD		(IOMUXC_BASE_ADDR + 0x045c)

#define IOMUXC_AUDMUX_P4_INPUT_DA_AMX_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x0460)
#define IOMUXC_AUDMUX_P4_INPUT_DB_AMX_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x0464)
#define IOMUXC_AUDMUX_P4_INPUT_RXCLK_AMX_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x0468)
#define IOMUXC_AUDMUX_P4_INPUT_RXFS_AMX_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x046c)
#define IOMUXC_AUDMUX_P4_INPUT_TXCLK_AMX_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x0470)
#define IOMUXC_AUDMUX_P4_INPUT_TXFS_AMX_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x0474)
#define IOMUXC_AUDMUX_P7_INPUT_DA_AMX_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x0478)
#define IOMUXC_AUDMUX_P7_INPUT_TXFS_AMX_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x047c)
#define IOMUXC_CAN1_IPP_IND_CANRX_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x0480)
#define IOMUXC_CAN2_IPP_IND_CANRX_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x0484)
#define IOMUXC_CSI_IPP_CSI_D_0_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x0488)
#define IOMUXC_CSI_IPP_CSI_D_1_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x048c)
#define IOMUXC_CSPI1_IPP_IND_SS3_B_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x0490)
#define IOMUXC_CSPI2_IPP_CSPI_CLK_IN_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x0494)
#define IOMUXC_CSPI2_IPP_IND_DATAREADY_B_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x0498)
#define IOMUXC_CSPI2_IPP_IND_MISO_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x049c)
#define IOMUXC_CSPI2_IPP_IND_MOSI_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x04a0)
#define IOMUXC_CSPI2_IPP_IND_SS0_B_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x04a4)
#define IOMUXC_CSPI2_IPP_IND_SS1_B_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x04a8)
#define IOMUXC_CSPI3_IPP_CSPI_CLK_IN_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x04ac)
#define IOMUXC_CSPI3_IPP_IND_DATAREADY_B_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x04b0)
#define IOMUXC_CSPI3_IPP_IND_MISO_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x04b4)
#define IOMUXC_CSPI3_IPP_IND_MOSI_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x04b8)
#define IOMUXC_CSPI3_IPP_IND_SS0_B_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x04bc)
#define IOMUXC_CSPI3_IPP_IND_SS1_B_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x04c0)
#define IOMUXC_CSPI3_IPP_IND_SS2_B_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x04c4)
#define IOMUXC_CSPI3_IPP_IND_SS3_B_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x04c8)
#define IOMUXC_ESDHC1_IPP_DAT4_IN_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x04cc)
#define IOMUXC_ESDHC1_IPP_DAT5_IN_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x04d0)
#define IOMUXC_ESDHC1_IPP_DAT6_IN_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x04d4)
#define IOMUXC_ESDHC1_IPP_DAT7_IN_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x04d8)
#define IOMUXC_ESDHC2_IPP_CARD_CLK_IN_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x04dc)
#define IOMUXC_ESDHC2_IPP_CMD_IN_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x04e0)
#define IOMUXC_ESDHC2_IPP_DAT0_IN_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x04e4)
#define IOMUXC_ESDHC2_IPP_DAT1_IN_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x04e8)
#define IOMUXC_ESDHC2_IPP_DAT2_IN_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x04ec)
#define IOMUXC_ESDHC2_IPP_DAT3_IN_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x04f0)
#define IOMUXC_ESDHC2_IPP_DAT4_IN_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x04f4)
#define IOMUXC_ESDHC2_IPP_DAT5_IN_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x04f8)
#define IOMUXC_ESDHC2_IPP_DAT6_IN_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x04fc)
#define IOMUXC_ESDHC2_IPP_DAT7_IN_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x0500)
#define IOMUXC_FEC_FEC_COL_SELECT_INPUT			(IOMUXC_BASE_ADDR + 0x0504)
#define IOMUXC_FEC_FEC_CRS_SELECT_INPUT			(IOMUXC_BASE_ADDR + 0x0508)
#define IOMUXC_FEC_FEC_RDATA_2_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x050c)
#define IOMUXC_FEC_FEC_RDATA_3_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x0510)
#define IOMUXC_FEC_FEC_RX_CLK_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x0514)
#define IOMUXC_FEC_FEC_RX_ER_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x0518)
#define IOMUXC_I2C2_IPP_SCL_IN_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x051c)
#define IOMUXC_I2C2_IPP_SDA_IN_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x0520)
#define IOMUXC_I2C3_IPP_SCL_IN_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x0524)
#define IOMUXC_I2C3_IPP_SDA_IN_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x0528)
#define IOMUXC_KPP_IPP_IND_COL_4_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x052c)
#define IOMUXC_KPP_IPP_IND_COL_5_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x0530)
#define IOMUXC_KPP_IPP_IND_COL_6_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x0534)
#define IOMUXC_KPP_IPP_IND_COL_7_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x0538)
#define IOMUXC_KPP_IPP_IND_ROW_4_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x053c)
#define IOMUXC_KPP_IPP_IND_ROW_5_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x0540)
#define IOMUXC_KPP_IPP_IND_ROW_6_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x0544)
#define IOMUXC_KPP_IPP_IND_ROW_7_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x0548)
#define IOMUXC_SIM1_PIN_SIM_RCVD1_IN_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x054c)
#define IOMUXC_SIM1_PIN_SIM_SIMPD1_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x0550)
#define IOMUXC_SIM1_SIM_RCVD1_IO_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x0554)
#define IOMUXC_SIM2_PIN_SIM_RCVD1_IN_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x0558)
#define IOMUXC_SIM2_PIN_SIM_SIMPD1_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x055c)
#define IOMUXC_SIM2_SIM_RCVD1_IO_SELECT_INPUT		(IOMUXC_BASE_ADDR + 0x0560)
#define IOMUXC_UART3_IPP_UART_RTS_B_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x0564)
#define IOMUXC_UART3_IPP_UART_RXD_MUX_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x0568)
#define IOMUXC_UART4_IPP_UART_RTS_B_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x056c)
#define IOMUXC_UART4_IPP_UART_RXD_MUX_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x0570)
#define IOMUXC_UART5_IPP_UART_RTS_B_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x0574)
#define IOMUXC_UART5_IPP_UART_RXD_MUX_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x0578)
#define IOMUXC_USB_TOP_IPP_IND_OTG_USB_OC_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x057c)
#define IOMUXC_USB_TOP_IPP_IND_UH2_USB_OC_SELECT_INPUT	(IOMUXC_BASE_ADDR + 0x0580)

#define IOMUXC_SW_MUX_CTL_PAD_SION_NORM    (0 << 4) /* Normal mode: IO is determined by the selected muxing mode. (default) */
#define IOMUXC_SW_MUX_CTL_PAD_SION_LB      (1 << 4) /* Loopback mode: Input feedthrough, loopback feature enabled. */
#define IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_0   (0)
#define IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_1   (1)
#define IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_2   (2)
#define IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_3   (3)
#define IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_4   (4)
#define IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_5   (5)
#define IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_6   (6)
#define IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_7   (7)
#define MUX_SION	(IOMUXC_SW_MUX_CTL_PAD_SION_LB)
#define MUX_ALT0	(IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_0)
#define MUX_ALT1	(IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_1)
#define MUX_ALT2	(IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_2)
#define MUX_ALT3	(IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_3)
#define MUX_ALT4	(IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_4)
#define MUX_ALT5	(IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_5)
#define MUX_ALT6	(IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_6)
#define MUX_ALT7	(IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_7)
#define SION(alt)	(IOMUXC_SW_MUX_CTL_PAD_SION_LB | alt)

#define IOMUXC_SW_PAD_CTL_PAD_DVS_3_3     (0)       /* Driver Voltage Select 3.3V */
#define IOMUXC_SW_PAD_CTL_PAD_DVS_1_8     (1 << 13) /* Driver Voltage Select 1.8V */
#define IOMUXC_SW_PAD_CTL_PAD_HYS         (1 << 8)  /* Hysteresis Enabled */
#define IOMUXC_SW_PAD_CTL_PAD_PKE         (1 << 7)  /* Pull/Keeper Enabled */
#define IOMUXC_SW_PAD_CTL_PAD_PUE_PULL    (1 << 6)  /* Keeper Disabled, Pull Enabled */
#define IOMUXC_SW_PAD_CTL_PAD_PUE_KEEPER  (0 << 6)  /* Keeper Enabled, Pull Disabled */
#define IOMUXC_SW_PAD_CTL_PAD_PUS_100K_PD (0 << 4)  /* 100K ohm Pull-down */
#define IOMUXC_SW_PAD_CTL_PAD_PUS_47K_PU  (1 << 4)  /* 47K ohm Pull-up */
#define IOMUXC_SW_PAD_CTL_PAD_PUS_100K_PU (2 << 4)  /* 100K ohm Pull-up */
#define IOMUXC_SW_PAD_CTL_PAD_PUS_22K_PU  (3 << 4)  /* 22K ohm Pull-up */
#define IOMUXC_SW_PAD_CTL_PAD_ODE         (1 << 3)  /* Open Drain Enabled */
#define IOMUXC_SW_PAD_CTL_PAD_DSE_NORM    (0 << 1)  /* Nominal or standard drive strength */
#define IOMUXC_SW_PAD_CTL_PAD_DSE_HIGH    (1 << 1)  /* High drive strength */
#define IOMUXC_SW_PAD_CTL_PAD_DSE_MAX     (2 << 1)  /* Max drive strength */
#define IOMUXC_SW_PAD_CTL_PAD_SRE         (1 << 0)  /* Fast Slew Rate Enabled */

#ifndef __ASSEMBLER__
#include <htypes.h>

struct iomux_info {
	int pin;
	u8 mux1;
	u8 mux2;
	u16 pad1;
	u16 pad2;
};

extern void mx25_iomux_config(struct iomux_info *iomux, int type);
extern void mx25_gpio_output(int pin, int mode);
extern void mx25_gpio_input(int pin);
extern int mx25_gpio_get_psr(int pin);

#endif

#endif