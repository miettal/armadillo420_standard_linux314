#ifndef _HERMIT_ARCH_MX25_SYSTEM_H_
#define _HERMIT_ARCH_MX25_SYSTEM_H_

/* System Memory Map:
 * ----------
 * 0x0000_0000 - 0x0000_3FFF	16KB	ROM (36KB)
 * 0x0000_4000 - 0x0040_3FFF	---	Reserved
 * 0x0040_4000 - 0x0040_8FFF	20KB	ROM (36KB)
 * 0x0040_9000 - 0x0FFF_FFFF	---	Reserved
 * 0x1000_0000 - 0x1FFF_FFFF	---	Reserved
 * 0x2000_0000 - 0x2FFF_FFFF	---	Reserved
 * 0x3000_0000 - 0x3FFF_FFFF	---	Reserved
 * 0x4000_0000 - 0x43EF_FFFF	---	Reserved
 * 0x43F0_0000 - 0x43F0_3FFF	16KB	AIPS A control registers
 * 0x43F0_4000 - 0x43F0_7FFF	16KB	ARM926 platform MAX
 * 0x43F0_8000 - 0x43F0_BFFF	16KB	ARM926 platform CLKCTL
 * 0x43F0_C000 - 0x43F0_FFFF	16KB	ARM926 platform ETB registers
 * 0x43F1_0000 - 0x43F1_3FFF	16KB	ARM926 platform ETB memory
 * 0x43F1_4000 - 0x43F1_7FFF	16KB	ARM926 platform AAPE registers
 * 0x43F1_8000 - 0x43F7_FFFF	---	Reserved
 * 0x43F8_0000 - 0x43F8_3FFF	16KB	I2C-1
 * 0x43F8_4000 - 0x43F8_7FFF	16KB	I2C-3
 * 0x43F8_8000 - 0x43F8_BFFF	16KB	CAN-1
 * 0x43F8_C000 - 0x43F8_FFFF	16KB	CAN-2
 * 0x43F9_0000 - 0x43F9_3FFF	16KB	UART-1
 * 0x43F9_4000 - 0x43F9_7FFF	16KB	UART-2
 * 0x43F9_8000 - 0x43F9_BFFF	16KB	I2C-2
 * 0x43F9_C000 - 0x43F9_FFFF	16KB	1-Wire
 * 0x43FA_0000 - 0x43FA_3FFF	16KB	ATA (CPU side)
 * 0x43FA_4000 - 0x43FA_7FFF	16KB	CSPI-1
 * 0x43FA_8000 - 0x43FA_BFFF	16KB	KPP
 * 0x43FA_C000 - 0x43FA_FFFF	16KB	IOMUXC
 * 0x43FB_0000 - 0x43FB_3FFF	16KB	AUDMUX
 * 0x43FB_4000 - 0x43FB_7FFF	---	Reserved
 * 0x43FB_8000 - 0x43FB_BFFF	16KB	ECT (IP BUS A)
 * 0x43FB_C000 - 0x43FB_FFFF	16KB	ECT (IP BUS B)
 * 0x43FC_0000 - 0x43FF_FFFF	---	Reserved
 * 0x4400_0000 - 0x4FFF_FFFF	---	Reserved
 * 0x5000_0000 - 0x5000_3FFF	16KB	SPBA base address
 * 0x5000_4000 - 0x5000_7FFF	16KB	CSPI-3
 * 0x5000_8000 - 0x5000_BFFF	16KB	UART-4
 * 0x5000_C000 - 0x5000_FFFF	16KB	UART-3
 * 0x5001_0000 - 0x5001_3FFF	16KB	CSPI-2
 * 0x5001_4000 - 0x5001_7FFF	16KB	SSI-2
 * 0x5001_8000 - 0x5001_BFFF	16KB	ESAI
 * 0x5001_C000 - 0x5001_FFFF	---	Reserved
 * 0x5002_0000 - 0x5002_3FFF	16KB	ATA
 * 0x5002_4000 - 0x5002_7FFF	16KB	SIM-1
 * 0x5002_8000 - 0x5002_BFFF	16KB	SIM-2
 * 0x5002_C000 - 0x5002_FFFF	16KB	UART-5
 * 0x5003_0000 - 0x5003_3FFF	16KB	TSC
 * 0x5003_4000 - 0x5003_7FFF	16KB	SSI-1
 * 0x5003_8000 - 0x5003_BFFF	16KB	FEC
 * 0x5003_C000 - 0x5003_FFFF	16KB	SPBA registers
 * 0x5004_0000 - 0x51FF_FFFF	---	Reserved
 * 0x5200_0000 - 0x53EF_FFFF	---	Reserved
 * 0x53F0_0000 - 0x53F0_3FFF	16KB	AIPS B	control registers
 * 0x53F0_4000 - 0x53F7_FFFF	---	Reserved
 * 0x53F8_0000 - 0x53F8_3FFF	16KB	CCM
 * 0x53F8_4000 - 0x53F8_7FFF	16KB	GPT-4
 * 0x53F8_8000 - 0x53F8_BFFF	16KB	GPT-3
 * 0x53F8_C000 - 0x53F8_FFFF	16KB	GPT-2
 * 0x53F9_0000 - 0x53F9_3FFF	16KB	GPT-1
 * 0x53F9_4000 - 0x53F9_7FFF	16KB	EPIT-1
 * 0x53F9_8000 - 0x53F9_BFFF	16KB	EPIT-2
 * 0x53F9_C000 - 0x53F9_FFFF	16KB	GPIO-4
 * 0x53FA_0000 - 0x53FA_3FFF	16KB	PWM-2
 * 0x53FA_4000 - 0x53FA_7FFF	16KB	GPIO-3
 * 0x53FA_8000 - 0x53FA_BFFF	16KB	PWM-3
 * 0x53FA_C000 - 0x53FA_FFFF	16KB	SCC
 * 0x53FB_0000 - 0x53FB_3FFF	16KB	RNGB
 * 0x53FB_4000 - 0x53FB_7FFF	16KB	eSDHC-1
 * 0x53FB_8000 - 0x53FB_BFFF	16KB	eSDHC-2
 * 0x53FB_C000 - 0x53FB_FFFF	16KB	LCDC
 * 0x53FC_0000 - 0x53FC_3FFF	16KB	SLCDC
 * 0x53FC_4000 - 0x53FC_7FFF	---	Reserved
 * 0x53FC_8000 - 0x53FC_BFFF	16KB	PWM-4
 * 0x53FC_C000 - 0x53FC_FFFF	16KB	GPIO-1
 * 0x53FD_0000 - 0x53FD_3FFF	16KB	GPIO-2
 * 0x53FD_4000 - 0x53FD_7FFF	16KB	SDMA
 * 0x53FD_8000 - 0x53FD_BFFF	---	Reserved
 * 0x53FD_C000 - 0x53FD_FFFF	16KB	WDOG16KB TSC
 * 0x53FE_0000 - 0x53FE_3FFF	16KB	PWM-1
 * 0x53FE_4000 - 0x53FE_7FFF	---	Reserved
 * 0x53FE_8000 - 0x53FE_BFFF	---	Reserved
 * 0x53FE_C000 - 0x53FE_FFFF	16KB	RTICv3
 * 0x53FF_0000 - 0x53FF_3FFF	16KB	IIM
 * 0x53FF_4000 - 0x53FF_7FFF	16KB	USB
 * 0x53FF_8000 - 0x53FF_BFFF	16KB	CSI
 * 0x53FF_C000 - 0x53FF_FFFF	16KB	DryIce
 * 0x5400_0000 - 0x5FFF_FFFF	---	Reserved
 * 0x6000_0000 - 0x67FF_FFFF	128MB	ARM926 platform ROMPATCH
 * 0x6800_0000 - 0x6FFF_FFFF	128MB	ARM926 platform ASIC
 * 0x7000_0000 - 0x77FF_FFFF	---	Reserved
 * 0x7800_0000 - 0x7801_FFFF	128KB	RAM
 * 0x7802_0000 - 0x7FFF_FFFF	128MB	RAM alias
 * 0x8000_0000 - 0x8FFF_FFFF	256MB	SDRAM bank 0
 * 0x9000_0000 - 0x9FFF_FFFF	256MB	SDRAM bank 1
 * 0xA000_0000 - 0xA7FF_FFFF	128MB	WEIM CS0 (flash 128)1
 * 0xA800_0000 - 0xAFFF_FFFF	128MB	WEIM CS1 (flash 64)1
 * 0xB000_0000 - 0xB1FF_FFFF	32MB	WEIM CS2 (SRAM)
 * 0xB200_0000 - 0xB3FF_FFFF	32MB	WEIM CS3 (SRAM)
 * 0xB400_0000 - 0xB5FF_FFFF	32MB	WEIM CS4
 * 0xB600_0000 - 0xB7FF_FFFF	---	Reserved
 * 0xB800_0000 - 0xB800_0FFF	---	Reserved
 * 0xB800_1000 - 0xB800_1FFF	4KB	SDRAM control registers
 * 0xB800_2000 - 0xB800_2FFF	4KB	WEIM control registers
 * 0xB800_3000 - 0xB800_3FFF	4KB	M3IF control registers
 * 0xB800_4000 - 0xB800_4FFF	4KB	EMI control registers
 * 0xB800_5000 - 0xBAFF_FFFF	---	Reserved
 * 0xBB00_0000 - 0xBB00_0FFF	4KB	NAND flash main area buffer
 * 0xBB00_1000 - 0xBB00_11FF	512B	NAND flash spare area buffer
 * 0xBB00_1200 - 0xBB00_1DFF	---	Reserved
 * 0xBB00_1E00 - 0xBB00_1FFF	512B	NAND flash control registers
 * 0xBB01_2000 - 0xBFFF_FFFF	96MB	Reserved
 * 0xC000_0000 - 0xFFFF_FFFF	---	Reserved
 */

/*******************************************
 * AIPS
 *******************************************/
#define AIPS1_BASE_ADDR		0x43F00000
#define AIPS2_BASE_ADDR		0x53F00000
#define AIPS(REG)		AIPS_##REG
#define AIPS_MPROT0_7		(0x00)
#define AIPS_MPROT8_15		(0x04)
#define AIPS_PACR0_7		(0x20)
#define AIPS_PACR8_15		(0x24)
#define AIPS_PACR16_23		(0x28)
#define AIPS_PACR24_31		(0x2C)
#define AIPS_OPACR0_7		(0x40)
#define AIPS_OPACR8_15		(0x44)
#define AIPS_OPACR16_23		(0x48)
#define AIPS_OPACR24_31		(0x4C)
#define AIPS_OPACR32_33		(0x50)

/*******************************************
 * MAX
 *******************************************/
#define MAX_BASE_ADDR		0x43F04000
#define MAX(REG)		MAX_##REG
#define MAX_SLV0_MPR0		(0x000)
#define MAX_SLV0_SGPCR0		(0x010)
#define MAX_SLV1_MPR1		(0x100)
#define MAX_SLV1_SGPCR1		(0x110)
#define MAX_SLV2_MPR2		(0x200)
#define MAX_SLV2_SGPCR2		(0x210)
#define MAX_SLV3_MPR3		(0x300)
#define MAX_SLV3_SGPCR3		(0x310)
#define MAX_SLV4_MPR4		(0x400)
#define MAX_SLV4_SGPCR4		(0x410)
#define MAX_MST0_MGPCR0		(0x800)
#define MAX_MST1_MGPCR1		(0x900)
#define MAX_MST2_MGPCR2		(0xA00)
#define MAX_MST3_MGPCR3		(0xB00)
#define MAX_MST4_MGPCR4		(0xC00)

/*******************************************
 * CLKCTL
 *******************************************/
#define CTL_BASE_ADDR		0x43F08000
#define CTL(REG)		CTL_##REG
#define CTL_GP_CTRL		(0x00)
#define CTL_GP_SER		(0x04)
#define CTL_GP_CER		(0x08)
#define CTL_GP_TAPID		(0x010)

/*******************************************
 * ETB
 *******************************************/
#define ETB_BASE_ADDR		0x43F0C000
#define ETB(REG)		ETB_##REG
#define ETB_ID			(0x00)
#define ETB_RAM_DEPTH		(0x04)
#define ETB_RAM_WIDTH		(0x08)
#define ETB_STATUS		(0x0C)
#define ETB_RAM_W_PTR		(0x18)
#define ETB_TRG_CNT		(0x1C)
#define ETB_CTL			(0x20)

/*******************************************
 * ECT
 *******************************************/
#define ECTA_BASE_ADDR		0x43F18000
#define ECT1_BASE_ADDR		0x43FB8000
#define ECT2_BASE_ADDR		0x43FBC000
#define ECT(REG)		ECT_##REG
#define ECT_CTICONTROL		(0x0)
#define ECT_CTISTATUS		(0x4)
#define ECT_CTILOCK		(0x8)
#define ECT_CTIPROTECTION	(0xC)
#define ECT_CTIINTACK		(0x10)
#define ECT_CTIAPPTEST		(0x14)
#define ECT_CTIAPPCLEAR		(0x18)
#define ECT_CTIAPPPULSE		(0x1C)
#define ECT_CTIINEN0		(0x20)
#define ECT_CTIINEN1		(0x24)
#define ECT_CTIINEN2		(0x28)
#define ECT_CTIINEN3		(0x2C)
#define ECT_CTIINEN4		(0x30)
#define ECT_CTIINEN5		(0x34)
#define ECT_CTIINEN6		(0x38)
#define ECT_CTIINEN7		(0x3C)
#define ECT_CTIOUTEN0		(0xA0)
#define ECT_CTIOUTEN1		(0xA4)
#define ECT_CTIOUTEN2		(0xA8)
#define ECT_CTIOUTEN3		(0xAC)
#define ECT_CTIOUTEN4		(0xB0)
#define ECT_CTIOUTEN5		(0xB4)
#define ECT_CTIOUTEN6		(0xB8)
#define ECT_CTIOUTEN7		(0xBC)
#define ECT_CTITRIGINSTATUS	(0x130)
#define ECT_CTITRIGOUTSTATUS	(0x134)
#define ECT_CTICHINSTATUS	(0x138)
#define ECT_CTICHOUTSTATUS	(0x13C)
#define ECT_CTIITCR		(0x200)
#define ECT_CTIITIP0		(0x204)
#define ECT_CTIITIP1		(0x208)
#define ECT_CTIITIP2		(0x20C)
#define ECT_CTIITIP3		(0x210)
#define ECT_CTITIOP0		(0x214)
#define ECT_CTITIOP1		(0x218)
#define ECT_CTITIOP2		(0x21C)
#define ECT_CTITIOP3		(0x220)
#define ECT_CTIPERIPHID0	(0xFE0)
#define ECT_CTIPERIPHID1	(0xFE4)
#define ECT_CTIPERIPHID2	(0xFE8)
#define ECT_CTIPERIPHID3	(0xFEC)
#define ECT_CTIPCELLID0		(0xFF0)
#define ECT_CTIPCELLID1		(0xFF4)
#define ECT_CTIPCELLID2		(0xFF8)
#define ECT_CTIPCELLID3		(0xFFC)

/*******************************************
 * I2C
 *******************************************/
#define I2C1_BASE_ADDR		0x43F80000
#define I2C2_BASE_ADDR		0x43F98000
#define I2C3_BASE_ADDR		0x43F84000
#define I2C(REG)		I2C_##REG
#define I2C_IADR		(0x00)
#define I2C_IFDR		(0x04)
#define I2C_I2CR		(0x08)
#define I2C_I2SR		(0x0C)
#define I2C_I2DR		(0x10)

/*******************************************
 * CAN
 *******************************************/
#define CAN1_BASE_ADDR		0x43F88000
#define CAN2_BASE_ADDR		0x43F8C000

/*******************************************
 * UART
 *******************************************/
#define UART1_BASE_ADDR		0x43F90000
#define UART2_BASE_ADDR		0x43F94000
#define UART3_BASE_ADDR		0x5000C000
#define UART4_BASE_ADDR		0x50008000
#define UART5_BASE_ADDR		0x5002C000
#define UART(REG)		UART_##REG
#define UART_URXD		(0x00)
#define UART_UTXD		(0x40)
#define UART_UCR1		(0x80)
#define UART_UCR2		(0x84)
#define UART_UCR3		(0x88)
#define UART_UCR4		(0x8C)
#define UART_UFCR		(0x90)
#define UART_USR1		(0x94)
#define UART_USR2		(0x98)
#define UART_UESC		(0x9C)
#define UART_UTIM		(0xA0)
#define UART_UBIR		(0xA4)
#define UART_UBMR		(0xA8)
#define UART_UBRC		(0xAC)
#define UART_ONEMS		(0xB0)
#define UART_UTS		(0xB4)

/*******************************************
 * OWIRE
 *******************************************/
#define OWIRE_BASE_ADDR		0x43F9C000
#define OWIRE(REG)		OWIRE_##REG
#define OWIRE_CTRL		(0x00)
#define OWIRE_TIME_DIV		(0x02)
#define OWIRE_RESET		(0x04)
#define OWIRE_COMMAND		(0x06)
#define OWIRE_TXRX		(0x08)
#define OWIRE_INTR		(0x0A)
#define OWIRE_INTR_EN		(0x0C)

/*******************************************
 * CSPI
 *******************************************/
#define CSPI1_BASE_ADDR		0x43FA4000
#define CSPI2_BASE_ADDR		0x50010000
#define CSPI3_BASE_ADDR		0x50004000
#define CSPI(REG)		CSPI_##REG
#define CSPI_RXDATA		(0x00)
#define CSPI_TXDATA		(0x04)
#define CSPI_CONREG		(0x08)
#define CSPI_INTREG		(0x0C)
#define CSPI_DMAREG		(0x10)
#define CSPI_STATREG		(0x14)
#define CSPI_PERIODREG		(0x18)
#define CSPI_TESTREG		(0x1C)

/*******************************************
 * KPP
 *******************************************/
#define KPP_BASE_ADDR		0x43FA8000
#define KPP(REG)		KPP_##REG
#define KPP_KPCR		(0x00)
#define KPP_KPSR		(0x02)
#define KPP_KDDR		(0x04)
#define KPP_KPDR		(0x06)

/*******************************************
 * IOMUXC
 *******************************************/
#define IOMUXC_BASE_ADDR	0x43FAC000
#include <arch/iomux.h>

/*******************************************
 * AUDMUX
 *******************************************/
#define AUDMUX_BASE_ADDR	0x43FB0000
#define AUDMUX_HPCR1		(0x00)
#define AUDMUX_HPDCR1		(0x04)
#define AUDMUX_HPCR2		(0x08)
#define AUDMUX_HPDCR2		(0x0C)
#define AUDMUX_HPCR3		(0x10)
#define AUDMUX_HPDCR3		(0x14)
#define AUDMUX_PPCR1		(0x18)
#define AUDMUX_PPDCR1		(0x1C)
#define AUDMUX_PPCR2		(0x20)
#define AUDMUX_PPDCR2		(0x24)
#define AUDMUX_PPCR3		(0x28)
#define AUDMUX_PPDCR3		(0x2C)
#define AUDMUX_PPCR4		(0x30)
#define AUDMUX_PPDCR4		(0x34)
#define AUDMUX_CNMCR		(0x38)

/*******************************************
 * SPBA
 *******************************************/
#define SPBA_BASE_ADDR		0x50000000
#define SPBA_PER0_START_ADDR	(SPBA_BASE_ADDR+0x00000000)
#define SPBA_PER1_START_ADDR	(SPBA_BASE_ADDR+0x00004000)
#define SPBA_PER2_START_ADDR	(SPBA_BASE_ADDR+0x00008000)
#define SPBA_PER3_START_ADDR	(SPBA_BASE_ADDR+0x0000C000)
#define SPBA_PER4_START_ADDR	(SPBA_BASE_ADDR+0x00010000)
#define SPBA_PER5_START_ADDR	(SPBA_BASE_ADDR+0x00014000)
#define SPBA_PER6_START_ADDR	(SPBA_BASE_ADDR+0x00018000)
#define SPBA_PER7_START_ADDR	(SPBA_BASE_ADDR+0x0001C000)
#define SPBA_PER8_START_ADDR	(SPBA_BASE_ADDR+0x00020000)
#define SPBA_PER9_START_ADDR	(SPBA_BASE_ADDR+0x00024000)
#define SPBA_PER10_START_ADDR	(SPBA_BASE_ADDR+0x00028000)
#define SPBA_PER11_START_ADDR	(SPBA_BASE_ADDR+0x0002C000)
#define SPBA_PER12_START_ADDR	(SPBA_BASE_ADDR+0x00030000)
#define SPBA_PER13_START_ADDR	(SPBA_BASE_ADDR+0x00034000)
#define SPBA_PER14_START_ADDR	(SPBA_BASE_ADDR+0x00038000)

#define SPBA_REG_ADDR		(SPBA_BASE_ADDR+0x0003C000)
#define SPBA_PRR0		(0x00)
#define SPBA_PRR1		(0x04)
#define SPBA_PRR2		(0x08)
#define SPBA_PRR3		(0x0C)
#define SPBA_PRR4		(0x10)
#define SPBA_PRR5		(0x14)
#define SPBA_PRR6		(0x18)
#define SPBA_PRR7		(0x1C)
#define SPBA_PRR8		(0x20)
#define SPBA_PRR9		(0x24)
#define SPBA_PRR10		(0x28)
#define SPBA_PRR11		(0x2C)
#define SPBA_PRR12		(0x30)
#define SPBA_PRR13		(0x34)
#define SPBA_PRR14		(0x38)

/*******************************************
 * ESAI
 *******************************************/
#define ESAI_BASE_ADDR		0x50018000
#define ESAI_ETDR		(0x00)
#define ESAI_ERDR		(0x04)
#define ESAI_ECR		(0x08)
#define ESAI_ESR		(0x0c)
#define ESAI_TFCR		(0x10)
#define ESAI_TFSR		(0x14)
#define ESAI_RFCR		(0x18)
#define ESAI_RFSR		(0x1c)
#define ESAI_TX0		(0x80)
#define ESAI_TX1		(0x84)
#define ESAI_TX2		(0x88)
#define ESAI_TX3		(0x8c)
#define ESAI_TX4		(0x90)
#define ESAI_TX5		(0x94)
#define ESAI_TSR		(0x98)
#define ESAI_RX0		(0xA0)
#define ESAI_RX1		(0xA4)
#define ESAI_RX2		(0xA8)
#define ESAI_RX3		(0xAC)
#define ESAI_SAISR		(0xCC)
#define ESAI_SAICR		(0xD0)
#define ESAI_TCR		(0xD4)
#define ESAI_TCCR		(0xD8)
#define ESAI_RCR		(0xDC)
#define ESAI_RCCR		(0xE0)
#define ESAI_TSMA		(0xE4)
#define ESAI_TSMB		(0xE8)
#define ESAI_RSMA		(0xEC)
#define ESAI_RSMB		(0xF0)
#define ESAI_PDRC		(0xF4)
#define ESAI_PRRC		(0xF8)
#define ESAI_PCRC		(0xFC)

/*******************************************
 * ATA
 *******************************************/
#define ATA_BASE_ADDR		0x50020000
#define ATA_TIME_CONFIG0	(0x00)
#define ATA_TIME_CONFIG1	(0x04)
#define ATA_TIME_CONFIG2	(0x08)
#define ATA_TIME_CONFIG3	(0x0C)
#define ATA_TIME_CONFIG4	(0x10)
#define ATA_TIME_CONFIG5	(0x14)
#define ATA_FIFO_DATA_32	(0x18)
#define ATA_FIFO_DATA_16	(0x1C)
#define ATA_FIFO_FILL		(0x20)
#define ATA_CONTROL		(0x24)
#define ATA_INT_PENDING		(0x28)
#define ATA_INT_ENABLE		(0x2C)
#define ATA_INT_CLEAR		(0x30)
#define FIFO_ALARM		(0x34)
#define ATA_DCTR		(0xD8)
#define ATA_DDTR		(0xA0)
#define ATA_DFTR		(0xA4)
#define ATA_DSCR		(0xA8)
#define ATA_DSNR		(0xAC)
#define ATA_DCLR		(0xB0)
#define ATA_DCHR		(0xB4)
#define ATA_DDHR		(0xB8)
#define ATA_DCDR		(0xBC)

/*******************************************
 * SIM
 *******************************************/
#define SIM1_BASE_ADDR		0x50024000
#define SIM2_BASE_ADDR		0x50028000
#define SIM(REG)		SIM_##REG
#define SIM_PORT1_CNTL		(0x00)
#define SIM_SETUP		(0x04)
#define SIM_PORT1_DETECT	(0x08)
#define SIM_XMT_BUF		(0x0C)
#define SIM_RCV_BUF		(0x10)
#define SIM_PORT0_CNTL		(0x14)
#define SIM_CNTL		(0x18)
#define SIM_CLOCK_SELECT	(0x1C)
#define SIM_RCV_THRESHOLD	(0x20)
#define SIM_ENABLE		(0x24)
#define SIM_XMT_STATUS		(0x28)
#define SIM_RCV_STATUS		(0x2C)
#define SIM_INT_MASK		(0x30)
#define SIM_PORT0_DETECT	(0x3C)
#define SIM_DATA_FORMAT		(0x40)
#define SIM_XMT_THRESHOLD	(0x44)
#define SIM_GUARD_CNTL		(0x48)
#define SIM_OD_CONFIG		(0x4C)
#define SIM_RESET_CNTL		(0x50)
#define SIM_CHAR_WAIT		(0x54)
#define SIM_GPCNT		(0x58)
#define SIM_DIVISOR		(0x5C)
#define SIM_BWT			(0x60)
#define SIM_BGT			(0x64)
#define SIM_BWT_H		(0x68)
#define SIM_XMT_FIFO_STATUS	(0x6C)
#define SIM_RCV_FIFO_CNT	(0x70)
#define SIM_RCV_FIFO_WPTR	(0x74)
#define SIM_RCV_FIFO_RPTR	(0x78)

/*******************************************
 * TSC
 *******************************************/
#define TSC_BASE_ADDR		0x50030000

/*******************************************
 * SSI
 *******************************************/
#define SSI1_BASE_ADDR		0x50034000
#define SSI2_BASE_ADDR		0x50014000
#define SSI(REG)		SSI_##REG
#define SSI_STX0		(0x00)
#define SSI_STX1		(0x04)
#define SSI_SRX0		(0x08)
#define SSI_SRX1		(0x0C)
#define SSI_SCR			(0x10)
#define SSI_SISR		(0x14)
#define SSI_SIER		(0x18)
#define SSI_STCR		(0x1C)
#define SSI_SRCR		(0x20)
#define SSI_STCCR		(0x24)
#define SSI_SRCCR		(0x28)
#define SSI_SFCSR		(0x2C)
#define SSI_STR			(0x30)
#define SSI_SOR			(0x34)
#define SSI_SACNT		(0x38)
#define SSI_SACADD		(0x3C)
#define SSI_SACDAT		(0x40)
#define SSI_SATAG		(0x44)
#define SSI_STMSK		(0x48)
#define SSI_SRMSK		(0x4C)
#define SSI_SACCST		(0x50)
#define SSI_SACCEN		(0x54)
#define SSI_SACCDIS		(0x58)

/*******************************************
 * FEC
 *******************************************/
#define FEC_BASE_ADDR		0x50038000
#define FEC_EIR			(0x004)
#define FEC_EIMR		(0x008)
#define FEC_RDAR		(0x010)
#define FEC_TDAR		(0x014)
#define FEC_ECR			(0x024)
#define FEC_MMFR		(0x040)
#define FEC_MSCR		(0x044)
#define FEC_MIBC		(0x064)
#define FEC_RCR			(0x084)
#define FEC_TCR			(0x0C4)
#define FEC_PALR		(0x0E4)
#define FEC_PAUR		(0x0E8)
#define FEC_OPDR		(0x0EC)
#define FEC_IAUR		(0x118)
#define FEC_IALR		(0x11C)
#define FEC_GAUR		(0x120)
#define FEC_GALR		(0x124)
#define FEC_TFWR		(0x144)
#define FEC_FRBR		(0x14C)
#define FEC_FRSR		(0x150)
#define FEC_RDSR		(0x180)
#define FEC_TDSR		(0x184)
#define FEC_MRBR		(0x188)

#define RMON_T_DROP		(0x200)
#define RMON_T_PACKETS		(0x204)
#define RMON_T_BC_PKT		(0x208)
#define RMON_T_MC_PKT		(0x20C)
#define RMON_T_CRC_ALIGN	(0x210)
#define RMON_T_UNDERSIZE	(0x214)
#define RMON_T_OVERSIZE		(0x218)
#define RMON_T_FRAG		(0x21C)
#define RMON_T_JAB		(0x220)
#define RMON_T_COL		(0x224)
#define RMON_T_P64		(0x228)
#define RMON_T_P65TO127		(0x22C)
#define RMON_T_P128TO255	(0x230)
#define RMON_T_P256TO511	(0x234)
#define RMON_T_P512TO1023	(0x238)
#define RMON_T_P1024TO2047	(0x23C)
#define RMON_T_P_GTE20448	(0x240)
#define RMON_T_OCTETS		(0x244)
#define IEEE_T_DROP		(0x248)
#define IEEE_T_FRAME_OK		(0x24C)
#define IEEE_T_1COL		(0x250)
#define IEEE_T_MCOL		(0x254)
#define IEEE_T_DEF		(0x258)
#define IEEE_T_LCOL		(0x25C)
#define IEEE_T_EXCOL		(0x260)
#define IEEE_T_MACERR		(0x264)
#define IEEE_T_CSERR		(0x268)
#define IEEE_T_SQE		(0x26C)
#define IEEE_T_FDXFC		(0x270)
#define IEEE_T_OCTETS_OK	(0x274)
#define RMON_R_PACKETS		(0x284)
#define RMON_R_BC_PKT		(0x288)
#define RMON_R_MC_PKT		(0x28C)
#define RMON_R_CRC_ALIGN	(0x290)
#define RMON_R_UNDERSIZE	(0x294)
#define RMON_R_OVERSIZE		(0x298)
#define RMON_R_FRAG		(0x29C)
#define RMON_R_JAB		(0x2A0)
#define RMON_R_RESVD_0		(0x2A4)
#define RMON_R_P64		(0x2A8)
#define RMON_R_P65TO127		(0x2AC)
#define RMON_R_P128TO255	(0x2B0)
#define RMON_R_P256TO511	(0x2B4)
#define RMON_R_P512TO1023	(0x2B8)
#define RMON_R_P1024TO2047	(0x2BC)
#define RMON_R_P_GTE2048	(0x2C0)
#define RMON_R_OCTETS		(0x2C4)
#define IEEE_R_DROP		(0x2C8)
#define IEEE_R_FRAME_OK		(0x2CC)
#define IEEE_R_CRC		(0x2D0)
#define IEEE_R_ALIGN		(0x2D4)
#define IEEE_R_MACERR		(0x2D8)
#define IEEE_R_FDXFC		(0x2DC)
#define IEEE_R_OCTETS_OK	(0x2E0)
#define MIIGSK_CFGR		(0x300)
#define MIIGSK_ENR		(0x308)

/*******************************************
 * CRM
 *******************************************/
#define CCM_BASE_ADDR		0x53F80000
#define CCM_MPCTL		(0x00)
#define CCM_UPCTL		(0x04)
#define CCM_CCTL		(0x08)
#define CCM_CGR0		(0x0C)
#define CCM_CGR1		(0x10)
#define CCM_CGR2		(0x14)
#define CCM_PDR0		(0x18)
#define CCM_PDR1		(0x1C)
#define CCM_PDR2		(0x20)
#define CCM_PDR3		(0x24)
#define CCM_CRSR		(0x28)
#define CCM_CRDR		(0x2C)
#define CCM_DCVR0		(0x30)
#define CCM_DCVR1		(0x34)
#define CCM_DCVR2		(0x38)
#define CCM_DCVR3		(0x3C)
#define CCM_LTR0		(0x40)
#define CCM_LTR1		(0x44)
#define CCM_LTR2		(0x48)
#define CCM_LTR3		(0x4C)
#define CCM_LTBR0		(0x50)
#define CCM_LTBR1		(0x54)
#define CCM_PMCR0		(0x58)
#define CCM_PMCR1		(0x5C)
#define CCM_PMCR2		(0x60)
#define CCM_MISC		(0x64)

/*******************************************
 * GPT
 *******************************************/
#define GPT1_BASE_ADDR		0x53F90000
#define GPT2_BASE_ADDR		0x53F8C000
#define GPT3_BASE_ADDR		0x53F88000
#define GPT4_BASE_ADDR		0x53F84000
#define GPT(REG)		GPT_##REG
#define GPT_CR			(0x00)
#define GPT_PR			(0x04)
#define GPT_SR			(0x08)
#define GPT_IR			(0x0C)
#define GPT_OCR1		(0x10)
#define GPT_OCR2		(0x14)
#define GPT_OCR3		(0x18)
#define GPT_ICR1		(0x1C)
#define GPT_ICR2		(0x20)
#define GPT_CNT			(0x24)

/*******************************************
 * EPIT
 *******************************************/
#define EPIT1_BASE_ADDR		0x53F94000
#define EPIT2_BASE_ADDR		0x53F98000
#define EPIT(REG)		EPIT_##REG
#define EPIT_CR			(0x00)
#define EPIT_SR			(0x04)
#define EPIT_LR			(0x08)
#define EPIT_CMPR		(0x0C)
#define EPIT_CNR		(0x10)

/*******************************************
 * GPIO
 *******************************************/
#define GPIO1_BASE_ADDR		0x53FCC000
#define GPIO2_BASE_ADDR		0x53FD0000
#define GPIO3_BASE_ADDR		0x53FA4000
#define GPIO4_BASE_ADDR		0x53F9C000
#define GPIO(REG)		GPIO_##REG
#define GPIO_DR			(0x00)
#define GPIO_GDIR		(0x04)
#define GPIO_PSR		(0x08)
#define GPIO_ICR1		(0x0C)
#define GPIO_ICR2		(0x10)
#define GPIO_IMR		(0x14)
#define GPIO_ISR		(0x18)

/*******************************************
 * PWM
 *******************************************/
#define PWM1_BASE_ADDR		0x53FE0000
#define PWM2_BASE_ADDR		0x53FA0000
#define PWM3_BASE_ADDR		0x53FA8000
#define PWM4_BASE_ADDR		0x53FC8000
#define PWM(REG)		RQM_##REG
#define PWM_PWMCR		(0x00)
#define PWM_PWMSR		(0x04)
#define PWM_PWMIR		(0x08)
#define PWM_PWMSAR		(0x0C)
#define PWM_PWMPR		(0x10)
#define PWM_PWMCNR		(0x14)

/*******************************************
 * SCC
 *******************************************/
#define SCC_BASE_ADDR		0x53FAC000
#define SCM_BASE_ADDR		0x53FAE000
#define SCM_RED_START		(0x00)
#define SCM_BLACK_START		(0x04)
#define SCM_LENGTH		(0x08)
#define SCM_CONTROL		(0x0C)
#define SCM_STATUS		(0x10)
#define SCM_ERROR		(0x14)
#define SCM_INT_CONTROL		(0x18)
#define SCM_CONFIGURATION	(0x1C)
#define SCM_INIT_VECTOR0	(0x20)
#define SCM_INIT_VECTOR1	(0x24)

#define SMN_BASE_ADDR		0x53FAF000
#define SMN_STATUS		(0x00)
#define SMN_COMMAND		(0x04)
#define SMN_SEQSTART		(0x08)
#define SMN_SEQEND		(0x0C)
#define SMN_SEQCHK		(0x10)
#define SMN_BITCNT		(0x14)
#define SMN_INCSIZE		(0x18)
#define SMN_BBDEC		(0x1C)
#define SMN_COMPSIZE		(0x20)
#define SMN_PTCHK		(0x24)
#define SMN_CTCHK		(0x28)
#define SMN_TIMERIV		(0x2C)
#define SMN_TIMERCTL		(0x30)
#define SMN_DDSTATUS		(0x34)
#define SMN_TIMER		(0x38)

/*******************************************
 * RNGB
 *******************************************/
#define RNGB_BASE_ADDR		0x53FB0000
#define RNGB_VER_ID		(0x000)
#define RNGB_COMMAND		(0x004)
#define RNGB_CONTROL		(0x008)
#define RNGB_STAT		(0x00C)
#define RNGB_ERR_STAT		(0x010)
#define RNGB_FIFO		(0x014)
#define RNGB_VERIF_CTL		(0x020)
#define RNGB_OSC_CNTR_CTL	(0x028)
#define RNGB_OSC_CNTR		(0x02C)
#define RNGB_OSC_CNTR_STAT	(0x030)

/*******************************************
 * ESDHC
 *******************************************/
#define ESDHC1_BASE_ADDR	0x53FB4000
#define ESDHC2_BASE_ADDR	0x53FB8000
#define ESDHC(REG)		ESDHC_##REG
#define ESDHC_DSADDR		(0x000)
#define ESDHC_BLKATTR		(0x004)
#define ESDHC_CMDARG		(0x008)
#define ESDHC_XFERTYP		(0x00c)
#define ESDHC_CMDRSP0		(0x010)
#define ESDHC_CMDRSP1		(0x014)
#define ESDHC_CMDRSP2		(0x018)
#define ESDHC_CMDRSP3		(0x01c)
#define ESDHC_DATPORT		(0x020)
#define ESDHC_PRSSTAT		(0x024)
#define ESDHC_PROCTL		(0x028)
#define ESDHC_SYSCTL		(0x02c)
#define ESDHC_IRQSTAT		(0x030)
#define ESDHC_IRQSTATEN		(0x034)
#define ESDHC_IRQSIGEN		(0x038)
#define ESDHC_AUTOC12ERR	(0x03c)
#define ESDHC_HOSTCAPBLT	(0x040)
#define ESDHC_WML		(0x044)
#define ESDHC_FEVT		(0x050)
#define ESDHC_HOSTVER		(0x0fc)

/*******************************************
 * LCDC
 *******************************************/
#define LCDC_BASE_ADDR		0x53FBC000

/*******************************************
 * SLCDC
 *******************************************/
#define SLCDC_BASE_ADDR		0x53FC0000

/*******************************************
 * SDMA
 *******************************************/
#define SDMA_BASE_ADDR		0x53FD4000
#define SDMA_COPTR		(0x000)
#define SDMA_INTR		(0x004)
#define SDMA_STOP		(0x008)
#define SDMA_START		(0x00C)
#define SDMA_EVTOVR		(0x010)
#define SDMA_DSPOVR		(0x014)
#define SDMA_HOSTOVR		(0x018)
#define SDMA_EVTPEND		(0x01C)
#define SDMA_DSPENBL		(0x020)
#define SDMA_RESET		(0x024)
#define SDMA_EVTERR		(0x028)
#define SDMA_INTRMASK		(0x02C)
#define SDMA_PSW		(0x030)
#define SDMA_EVTERRDBG		(0x034)
#define SDMA_CONFIG		(0x038)
#define SDMA_ONCE_ENB		(0x040)
#define SDMA_ONCE_DATA		(0x044)
#define SDMA_ONCE_INSTR		(0x048)
#define SDMA_ONCE_STAT		(0x04C)
#define SDMA_ONCE_CMD		(0x050)
#define SDMA_MIRROR		(0x054)
#define SDMA_ILLINSTADDR	(0x058)
#define SDMA_CHN0ADDR		(0x05C)
#define SDMA_XTRIG_CONF1	(0x070)
#define SDMA_XTRIG_CONF2	(0x074)
#define SDMA_CHNPRI_0		(0x100)
#define SDMA_CHNPRI_1		(0x104)
#define SDMA_CHNPRI_2		(0x108)
#define SDMA_CHNPRI_3		(0x10C)
#define SDMA_CHNPRI_4		(0x110)
#define SDMA_CHNPRI_5		(0x114)
#define SDMA_CHNPRI_6		(0x118)
#define SDMA_CHNPRI_7		(0x11C)
#define SDMA_CHNPRI_8		(0x120)
#define SDMA_CHNPRI_9		(0x124)
#define SDMA_CHNPRI_10		(0x128)
#define SDMA_CHNPRI_11		(0x12C)
#define SDMA_CHNPRI_12		(0x130)
#define SDMA_CHNPRI_13		(0x134)
#define SDMA_CHNPRI_14		(0x138)
#define SDMA_CHNPRI_15		(0x13C)
#define SDMA_CHNPRI_16		(0x140)
#define SDMA_CHNPRI_17		(0x144)
#define SDMA_CHNPRI_18		(0x148)
#define SDMA_CHNPRI_19		(0x14C)
#define SDMA_CHNPRI_20		(0x150)
#define SDMA_CHNPRI_21		(0x154)
#define SDMA_CHNPRI_22		(0x158)
#define SDMA_CHNPRI_23		(0x15C)
#define SDMA_CHNPRI_24		(0x160)
#define SDMA_CHNPRI_25		(0x164)
#define SDMA_CHNPRI_26		(0x168)
#define SDMA_CHNPRI_27		(0x16C)
#define SDMA_CHNPRI_28		(0x170)
#define SDMA_CHNPRI_29		(0x174)
#define SDMA_CHNPRI_30		(0x178)
#define SDMA_CHNPRI_31		(0x17C)
#define SDMA_CHENBL_0		(0x200)
#define SDMA_CHENBL_1		(0x204)
#define SDMA_CHENBL_2		(0x208)
#define SDMA_CHENBL_3		(0x20C)
#define SDMA_CHENBL_4		(0x210)
#define SDMA_CHENBL_5		(0x214)
#define SDMA_CHENBL_6		(0x218)
#define SDMA_CHENBL_7		(0x21C)
#define SDMA_CHENBL_8		(0x220)
#define SDMA_CHENBL_9		(0x224)
#define SDMA_CHENBL_10		(0x228)
#define SDMA_CHENBL_11		(0x22C)
#define SDMA_CHENBL_12		(0x230)
#define SDMA_CHENBL_13		(0x234)
#define SDMA_CHENBL_14		(0x238)
#define SDMA_CHENBL_15		(0x23C)
#define SDMA_CHENBL_16		(0x240)
#define SDMA_CHENBL_17		(0x244)
#define SDMA_CHENBL_18		(0x248)
#define SDMA_CHENBL_19		(0x24C)
#define SDMA_CHENBL_20		(0x250)
#define SDMA_CHENBL_21		(0x254)
#define SDMA_CHENBL_22		(0x258)
#define SDMA_CHENBL_23		(0x25C)
#define SDMA_CHENBL_24		(0x260)
#define SDMA_CHENBL_25		(0x264)
#define SDMA_CHENBL_26		(0x268)
#define SDMA_CHENBL_27		(0x26C)
#define SDMA_CHENBL_28		(0x270)
#define SDMA_CHENBL_29		(0x274)
#define SDMA_CHENBL_30		(0x278)
#define SDMA_CHENBL_31		(0x27C)
#define SDMA_CHENBL_32		(0x280)
#define SDMA_CHENBL_33		(0x284)
#define SDMA_CHENBL_34		(0x288)
#define SDMA_CHENBL_35		(0x28C)
#define SDMA_CHENBL_36		(0x290)
#define SDMA_CHENBL_37		(0x294)
#define SDMA_CHENBL_38		(0x298)
#define SDMA_CHENBL_39		(0x29C)
#define SDMA_CHENBL_40		(0x2A0)
#define SDMA_CHENBL_41		(0x2A4)
#define SDMA_CHENBL_42		(0x2A8)
#define SDMA_CHENBL_43		(0x2AC)
#define SDMA_CHENBL_44		(0x2B0)
#define SDMA_CHENBL_45		(0x2B4)
#define SDMA_CHENBL_46		(0x2B8)
#define SDMA_CHENBL_47		(0x2BC)

/*******************************************
 * WDOG
 *******************************************/
#define WDOG_BASE_ADDR		0x53FDC000
#define WDOG(REG)		WDOG_##REG
#define WDOG_WCR		(0x00)
#define WDOG_WSR		(0x02)
#define WDOG_WRSR		(0x04)
#define WDOG_WICR		(0x06)
#define WDOG_WMCR		(0x08)

/*******************************************
 * RTIC
 *******************************************/
#define RTIC_BASE_ADDR		0x53FEC000
#define RTIC_STATUS		(0x000)
#define RTIC_COMMAND		(0x004)
#define RTIC_CONTROL		(0x008)
#define RTIC_DELAY_TIMER	(0x00C)
#define RTIC_MEMORYA_ADD1	(0x010)
#define RTIC_MEMORYA_LEN1	(0x014)
#define RTIC_MEMORYA_ADD2	(0x018)
#define RTIC_MEMORYA_LEN2	(0x01C)
#define RTIC_MEMORYB_ADD1	(0x030)
#define RTIC_MEMORYB_LEN1	(0x034)
#define RTIC_MEMORYB_ADD2	(0x038)
#define RTIC_MEMORYB_LEN2	(0x03C)
#define RTIC_MEMORYC_ADD1	(0x050)
#define RTIC_MEMORYC_LEN1	(0x054)
#define RTIC_MEMORYC_ADD2	(0x058)
#define RTIC_MEMORYC_LEN2	(0x05C)
#define RTIC_MEMORYD_ADD1	(0x070)
#define RTIC_MEMORYD_LEN1	(0x074)
#define RTIC_MEMORYD_ADD2	(0x078)
#define RTIC_MEMORYD_LEN2	(0x07C)
#define RTIC_FAULT_ADD		(0x090)
#define RTIC_WATCHDOG		(0x094)
#define RTIC_HASHA_128		(0x0A0)
#define RTIC_HASHA_96		(0x0A4)
#define RTIC_HASHA_64		(0x0A8)
#define RTIC_HASHA_32		(0x0AC)
#define RTIC_HASHA_0		(0x0B0)
#define RTIC_HASHB_128		(0x0C0)
#define RTIC_HASHB_96		(0x0C4)
#define RTIC_HASHB_64		(0x0C8)
#define RTIC_HASHB_32		(0x0CC)
#define RTIC_HASHB_0		(0x0D0)
#define RTIC_HASHC_128		(0x0E0)
#define RTIC_HASHC_96		(0x0E4)
#define RTIC_HASHC_64		(0x0E8)
#define RTIC_HASHC_32		(0x0EC)
#define RTIC_HASHC_0		(0x0F0)
#define RTIC_HASHD_128		(0x100)
#define RTIC_HASHD_96		(0x104)
#define RTIC_HASHD_64		(0x108)
#define RTIC_HASHD_32		(0x10C)
#define RTIC_HASHD_0		(0x110)

/*******************************************
 * IIM
 *******************************************/
#define IIM_BASE_ADDR		0x53FF0000
#define IIM_STAT		(0x0000)
#define IIM_STATM		(0x0004)
#define IIM_ERR			(0x0008)
#define IIM_EMASK		(0x000C)
#define IIM_FCTL		(0x0010)
#define IIM_UA			(0x0014)
#define IIM_LA			(0x0018)
#define IIM_SDAT		(0x001C)
#define IIM_PREV		(0x0020)
#define IIM_SREV		(0x0024)
#define IIM_PRG_P		(0x0028)
#define IIM_SCS0		(0x002C)
#define IIM_SCS1		(0x0030)
#define IIM_SCS2		(0x0034)
#define IIM_SCS3		(0x0038)
#define IIM_FBAC0		(0x0800)
#define IIM_JAC_HWV0		(0x0804)
#define IIM_HWV1		(0x0808)
#define IIM_HWV2		(0x080C)
#define IIM_HAB0		(0x0810)
#define IIM_HAB1		(0x0814)
#define IIM_PREV_FUSE		(0x0818)
#define IIM_SREV_FUSE		(0x081C)
#define IIM_SJC_CHALL_0		(0x0820)
#define IIM_SJC_CHALL_1		(0x0824)
#define IIM_SJC_CHALL_2		(0x0828)
#define IIM_SJC_CHALL_3		(0x082C)
#define IIM_SJC_CHALL_4		(0x0830)
#define IIM_SJC_CHALL_5		(0x0834)
#define IIM_SJC_CHALL_6		(0x0838)
#define IIM_SJC_CHALL_7		(0x083C)
#define IIM_FB0UC17		(0x0840)
#define IIM_FB0UC255		(0x0BFC)
#define IIM_FBAC1		(0x0C00)
#define IIM_KEY0		(0x0C04)
#define IIM_KEY1		(0x0C08)
#define IIM_KEY2		(0x0C0C)
#define IIM_KEY3		(0x0C10)
#define IIM_KEY4		(0x0C14)
#define IIM_KEY5		(0x0C18)
#define IIM_KEY6		(0x0C1C)
#define IIM_KEY7		(0x0C20)
#define IIM_KEY8		(0x0C24)
#define IIM_KEY9		(0x0C28)
#define IIM_KEY10		(0x0C2C)
#define IIM_KEY11		(0x0C30)
#define IIM_KEY12		(0x0C34)
#define IIM_KEY13		(0x0C38)
#define IIM_KEY14		(0x0C3C)
#define IIM_KEY15		(0x0C40)
#define IIM_KEY16		(0x0C44)
#define IIM_KEY17		(0x0C48)
#define IIM_KEY18		(0x0C4C)
#define IIM_KEY19		(0x0C50)
#define IIM_KEY20		(0x0C54)
#define IIM_SJC_RESP_0		(0x0C58)
#define IIM_SJC_RESP_1		(0x0C5C)
#define IIM_SJC_RESP_2		(0x0C60)
#define IIM_SJC_RESP_3		(0x0C64)
#define IIM_SJC_RESP_4		(0x0C68)
#define IIM_SJC_RESP_5		(0x0C6C)
#define IIM_SJC_RESP_6		(0x0C70)
#define IIM_SJC_RESP_7		(0x0C74)
#define IIM_FB2UC30		(0x0C78)
#define IIM_FB2UC255		(0x0FFC)
#define IIM_FBAC2		(0x1000)
#define IIM_XCORD		(0x1004)
#define IIM_YCORD		(0x1008)
#define IIM_FAB			(0x100C)
#define IIM_WAFER		(0x1010)
#define IIM_LOT0		(0x1014)
#define IIM_LOT1		(0x1018)
#define IIM_LOT2		(0x101C)
#define IIM_PROB		(0x1020)
#define IIM_FT			(0x1024)
#define IIM_FB1UC10		(0x1028)
#define IIM_FB1UC255		(0x13FC)
#define IIM_FBAC3		(0x1400)
#define IIM_FB3UC1		(0x1404)
#define IIM_FB3UC255		(0x17FC)

/*******************************************
 * USB
 *******************************************/
#define USB_MODULE_BASE_ADDR	0x53FF4000
#define USB_CONTROL		(USB_MODULE_BASE_ADDR+0x600)
#define USB_OTG_MIRROR		(USB_MODULE_BASE_ADDR+0x604)
#define USB_OTG_UTMI_PHY_CTRL_FUNC (USB_MODULE_BASE_ADDR+0x608)
#define USB_OTG_UTMI_PHY_CTRL_TEST (USB_MODULE_BASE_ADDR+0x60C)

#define USB_OTG_BASE_ADDR	(USB_MODULE_BASE_ADDR+0x000)
#define USB_H1_BASE_ADDR	(USB_MODULE_BASE_ADDR+0x200)
#define USB_H2_BASE_ADDR	(USB_MODULE_BASE_ADDR+0x400)
#define USB(REG)		USB_##REG
#define USB_ID			(0x000)
#define USB_HWGENERAL		(0x004)
#define USB_HWHOST		(0x008)
#define USB_HWDEVICE		(0x00C)
#define USB_HWTXBUF		(0x010)
#define USB_HWRXBUF		(0x014)
#define USB_GPTIMER0LD		(0x080)
#define USB_GPTIMER0CTRL	(0x084)
#define USB_GPTIMER1LD		(0x088)
#define USB_GPTIMER1CTRL	(0x08C)
#define USB_CAPLENGTH		(0x100)
#define USB_HCIVERSION		(0x102)
#define USB_HCSPARAMS		(0x104)
#define USB_HCCPARAMS		(0x108)
#define USB_DCIVERSION		(0x120)
#define USB_DCCPARAMS		(0x124)
#define USB_USBCMD		(0x140)
#define USB_USBSTS		(0x144)
#define USB_USBINTR		(0x148)
#define USB_FRINDEX		(0x14C)
#define USB_PERIODICLISTBASE	(0x154)
#define USB_ASYNCLISTADDR	(0x158)
#define USB_BURSTSIZE		(0x160)
#define USB_TXFILLTUNING	(0x164)
#define USB_CONFIGFLAG		(0x180)
#define USB_PORTSC1		(0x184)
#define USB_OTGSC		(0x1A4)
#define USB_USBMODE		(0x1A8)
#define USB_ENPDTSETUPSTAT	(0x1AC)
#define USB_ENDPTPRIME		(0x1B0)
#define USB_ENDPTFLUSH		(0x1B4)
#define USB_ENDPTSTATUS		(0x1B8)
#define USB_ENDPTCOMPLETE	(0x1BC)
#define USB_ENDPTCTRL0		(0x1C0)
#define USB_ENDPTCTRL1		(0x1C4)
#define USB_ENDPTCTRL2		(0x1C8)
#define USB_ENDPTCTRL3		(0x1CC)
#define USB_ENDPTCTRL4		(0x1D0)
#define USB_ENDPTCTRL5		(0x1D4)
#define USB_ENDPTCTRL6		(0x1D8)
#define USB__ENDPTCTRL7		(0x1DC)

/*******************************************
 * CSI
 *******************************************/
#define CSI_BASE_ADDR		0x53FF8000

/*******************************************
 * DRYICE
 *******************************************/
#define DRYICE_BASE_ADDR	0x53FFC000

/*******************************************
 * ROMPATCH
 *******************************************/
#define ROMPATCH_BASE_ADDR	0x60000000
#define ROMPATCH_D15		(0x0B4)
#define ROMPATCH_D14		(0x0B8)
#define ROMPATCH_D13		(0x0BC)
#define ROMPATCH_D12		(0x0C0)
#define ROMPATCH_D11		(0x0C4)
#define ROMPATCH_D10		(0x0C8)
#define ROMPATCH_D9		(0x0CC)
#define ROMPATCH_D8		(0x0D0)
#define ROMPATCH_D7		(0x0D4)
#define ROMPATCH_D6		(0x0D8)
#define ROMPATCH_D5		(0x0DC)
#define ROMPATCH_D4		(0x0E0)
#define ROMPATCH_D3		(0x0E4)
#define ROMPATCH_D2		(0x0E8)
#define ROMPATCH_D1		(0x0EC)
#define ROMPATCH_D0		(0x0F0)
#define ROMPATCH_CNTL		(0x0F4)
#define ROMPATCH_ENH		(0x0F8)
#define ROMPATCH_ENL		(0x0FC)
#define ROMPATCH_A0		(0x100)
#define ROMPATCH_A1		(0x104)
#define ROMPATCH_A2		(0x108)
#define ROMPATCH_A3		(0x10C)
#define ROMPATCH_A4		(0x110)
#define ROMPATCH_A5		(0x114)
#define ROMPATCH_A6		(0x118)
#define ROMPATCH_A7		(0x11C)
#define ROMPATCH_A8		(0x120)
#define ROMPATCH_A9		(0x124)
#define ROMPATCH_A10		(0x128)
#define ROMPATCH_A11		(0x12C)
#define ROMPATCH_A12		(0x130)
#define ROMPATCH_A13		(0x134)
#define ROMPATCH_A14		(0x138)
#define ROMPATCH_A15		(0x13C)
#define ROMPATCH_A16		(0x140)
#define ROMPATCH_A17		(0x144)
#define ROMPATCH_A18		(0x148)
#define ROMPATCH_A19		(0x14C)
#define ROMPATCH_A20		(0x150)
#define ROMPATCH_A21		(0x154)
#define ROMPATCH_A22		(0x158)
#define ROMPATCH_A23		(0x15C)
#define ROMPATCH_A24		(0x160)
#define ROMPATCH_A25		(0x164)
#define ROMPATCH_A26		(0x168)
#define ROMPATCH_A27		(0x16C)
#define ROMPATCH_A28		(0x170)
#define ROMPATCH_A29		(0x174)
#define ROMPATCH_A30		(0x178)
#define ROMPATCH_A31		(0x17C)
#define ROMPATCH_BRPT		(0x200)
#define ROMPATCH_BASE		(0x204)
#define ROMPATCH_SR		(0x208)
#define ROMPATCH_ABSR		(0x20C)
#define ROMPATCH_DADR		(0x210)
#define ROMPATCH_IADR		(0x214)

/*******************************************
 * AVIC
 *******************************************/
#define AVIC_BASE_ADDR		0x68000000
#define AVIC(REG)		AVIC_##REG
#define AVIC_INTCNTL		(0x00)
#define AVIC_NIMASK		(0x04)
#define AVIC_INTENNUM		(0x08)
#define AVIC_INTDISNUM		(0x0C)
#define AVIC_INTENABLEH		(0x10)
#define AVIC_INTENABLEL		(0x14)
#define AVIC_INTTYPEH		(0x18)
#define AVIC_INTTYPEL		(0x1C)
#define AVIC_NIPRIORITY7	(0x20)
#define AVIC_NIPRIORITY6	(0x24)
#define AVIC_NIPRIORITY5	(0x28)
#define AVIC_NIPRIORITY4	(0x2C)
#define AVIC_NIPRIORITY3	(0x30)
#define AVIC_NIPRIORITY2	(0x34)
#define AVIC_NIPRIORITY1	(0x38)
#define AVIC_NIPRIORITY0	(0x3C)
#define AVIC_NIVECSR		(0x40)
#define AVIC_FIVECSR		(0x44)
#define AVIC_INTSRCH		(0x48)
#define AVIC_INTSRCL		(0x4C)
#define AVIC_INTFRCH		(0x50)
#define AVIC_INTFRCL		(0x54)
#define AVIC_NIPNDH		(0x58)
#define AVIC_NIPNDL		(0x5C)
#define AVIC_FIPNDH		(0x60)
#define AVIC_FIPNDL		(0x64)

/*******************************************
 * External Memory
 *******************************************/
#define CSD0_BASE_ADDR		0x80000000
#define CSD0_END_ADD		(CSD0_BASE_ADDR + 0x0FFFFFFF)
#define CSD1_BASE_ADDR		0x90000000
#define CSD1_END_ADDR		(CSD1_BASE_ADDR + 0x0FFFFFFF)

/*******************************************
 * External Memory
 *******************************************/
#define CS0_BASE_ADDR		0xA0000000
#define CS0_END_ADDR		(CS0_BASE_ADDR + 0x07FFFFFF)
#define CS1_BASE_ADDR		0xA8000000
#define CS1_END_ADDR		(CS1_BASE_ADDR + 0x07FFFFFF)
#define CS2_BASE_ADDR		0xB0000000
#define CS2_END_ADDR		(CS2_BASE_ADDR + 0x01FFFFFF)
#define CS3_BASE_ADDR		0xB2000000
#define CS3_END_ADDR		(CS3_BASE_ADDR + 0x01FFFFFF)
#define CS4_BASE_ADDR		0xB4000000
#define CS4_END_ADDR		(CS4_BASE_ADDR + 0x01FFFFFF)
#define CS5_BASE_ADDR		0xB6000000
#define CS5_END_ADDR		(CS5_BASE_ADDR + 0x01FFFFFF)

/*******************************************
 * NFC
 *******************************************/
#define NFC_BASE_ADDR		0xBB000000
#define NFC_REG_BASE_ADDR	(NFC_BASE_ADDR+0x1E00)
#define NFC_RAM_BUF_ADDR	(0x04)
#define NFC_NAND_FLASH_ADDR	(0x06)
#define NFC_NAND_FLASH_CMD	(0x08)
#define NFC_CONFIG		(0x0A)
#define NFC_ECC_STAT_RSLT1	(0x0C)
#define NFC_ECC_STAT_RSLT2	(0x0E)
#define NFC_ECC_SPAS		(0x10)
#define NFC_NF_WR_PROT		(0x12)
#define NFC_NF_WR_PROT_STAT	(0x18)
#define NFC_NF_CONFIG1		(0x1A)
#define NFC_NF_CONFIG2		(0x1C)
#define NFC_ULOCK_START_BLK0	(0x20)
#define NFC_ULOCK_END_BLK0	(0x22)
#define NFC_ULOCK_START_BLK1	(0x24)
#define NFC_ULOCK_END_BLK1	(0x26)
#define NFC_ULOCK_START_BLK2	(0x28)
#define NFC_ULOCK_END_BLK2	(0x2A)
#define NFC_ULOCK_START_BLK3	(0x2C)
#define NFC_ULOCK_END_BLK3	(0x2E)

/*******************************************
 * ESDCTL
 *******************************************/
#define ESDCTL_BASE_ADDR	0xB8001000
#define ESD_ESDCTL0		(0x00)
#define ESD_ESDCFG0		(0x04)
#define ESD_ESDCTL1		(0x08)
#define ESD_ESDCFG1		(0x0C)
#define ESD_ESDMISC		(0x10)
#define CONFIG_DATA_MDDR1	(0x20)
#define CONFIG_DATA_MDDR2	(0x24)
#define CONFIG_DATA_MDDR3	(0x28)
#define CONFIG_DATA_MDDR4	(0x2C)
#define CONFIG_DATA_MDDR5	(0x30)
#define DEL_CYCL_LEN_MDDR	(0x34)
#define CONFIG_DATA_MDDR6	(0x38)

/*******************************************
 * WEIM
 *******************************************/
#define WEIM_BASE_ADDR		0xB8002000
#define WEIM_CS0U		(0x00)
#define WEIM_CS0L		(0x04)
#define WEIM_CS0A		(0x08)
#define WEIM_CS1U		(0x10)
#define WEIM_CS1L		(0x14)
#define WEIM_CS1A		(0x18)
#define WEIM_CS2U		(0x20)
#define WEIM_CS2L		(0x24)
#define WEIM_CS2A		(0x28)
#define WEIM_CS3U		(0x30)
#define WEIM_CS3L		(0x34)
#define WEIM_CS3A		(0x38)
#define WEIM_CS4U		(0x40)
#define WEIM_CS4L		(0x44)
#define WEIM_CS4A		(0x48)
#define WEIM_CS5U		(0x50)
#define WEIM_CS5L		(0x54)
#define WEIM_CS5A		(0x58)
#define WEIM_EIM		(0x60)

/*******************************************
 * M3IF
 *******************************************/
#define M3IF_BASE_ADDR		0xB8003000
#define M3IF_CTL		(0x00)
#define M3IF_WCFG0		(0x04)
#define M3IF_WCFG1		(0x08)
#define M3IF_WCFG2		(0x0C)
#define M3IF_WCFG3		(0x10)
#define M3IF_WCFG4		(0x14)
#define M3IF_WCFG5		(0x18)
#define M3IF_WCFG6		(0x1C)
#define M3IF_WCFG7		(0x20)
#define M3IF_WCSR		(0x24)
#define M3IF_SCFG0		(0x28)
#define M3IF_SCFG1		(0x2C)
#define M3IF_SCFG2		(0x30)
#define M3IF_SSR0		(0x34)
#define M3IF_SSR1		(0x38)

/*******************************************
 * RAM (128KB)
 *******************************************/
#define IRAM_BASE_ADDR		0x78000000
#define IRAM_END_ADDR		(IRAM_BASE_ADDR + 0x0001FFFF)

#endif
