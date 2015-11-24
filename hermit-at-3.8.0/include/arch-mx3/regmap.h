#ifndef _MX3_REGMAP_H_
#define _MX3_REGMAP_H_

#define L2CC_BASE_ADDR		0x30000000
#define AIPS1_CTRL_BASE_ADDR	0x43f00000
#define MAX_BASE_ADDR		0x43f04000
#define EVTMON_BASE_ADDR	0x43f08000
#define CLK_CTRL_BASE_ADDR	0x43f0c000
#define ETB_REG_BASE_ADDR	0x43f10000
#define ETB_MEM_BASE_ADDR	0x43f14000
#define ECT_CTIO_BASE_ADDR	0x43f18000
#define I2C_BASE_ADDR		0x43f80000
#define I2C3_BASE_ADDR		0x43f84000
#define USB_OTG_BASE_ADDR	0x43f88000
#define ATA_CTRL_BASE_ADDR	0x43f8c000
#define UART1_BASE_ADDR		0x43f90000
#define UART2_BASE_ADDR		0x43f94000
#define OWIRE_BASE_ADDR		0x43f9c000
#define SSI1_BASE_ADDR		0x43fa0000
#define CSPI1_BASE_ADDR		0x43fa4000
#define KPP_BASE_ADDR		0x43fa8000
#define IOMUXC_BASE_ADDR	0x43fac000
#define UART4_BASE_ADDR		0x43fb0000
#define UART5_BASE_ADDR		0x43fb4000
#define ECT1_BASE_ADDR		0x43fb8000
#define ECT2_BASE_ADDR		0x43fbc000
#define SPBA_BASE_ADDR		0x50000000
#define SDHC1_BASE_ADDR		0x50004000
#define SDHC2_BASE_ADDR		0x50008000
#define UART3_BASE_ADDR		0x5000c000
#define CSPI2_BASE_ADDR		0x50010000
#define SSI2_BASE_ADDR		0x50014000
#define SIM_BASE_ADDR		0x50018000
#define IIM_BASE_ADDR		0x5001c000
#define ATA_DMA_BASE_ADDR	0x50020000
#define MSHC1_BASE_ADDR		0x50024000
#define MSHC2_BASE_ADDR		0x50028000
#define SPBA_REG_BASE_ADDR     	0x5003c000
#define AIPS2_CTRL_BASE_ADDR	0x53f00000
#define CCM_BASE_ADDR		0x53f80000
#define FIRI_BASE_ADDR		0x53f8c000
#define GPT1_BASE_ADDR		0x53f90000
#define EPIT1_BASE_ADDR		0x53f94000
#define EPIT2_BASE_ADDR		0x53f98000
#define GPIO3_BASE_ADDR		0x53fa4000
#define SCC_BASE_ADDR		0x53fac000
#define RNGA_BASE_ADDR		0x53fb0000
#define IPU_BASE_ADDR		0x53fc0000
#define AUDMUX_BASE_ADDR	0x53fc4000
#define MPEG4_BASE_ADDR		0x53fc8000
#define GPIO1_BASE_ADDR		0x53fcc000
#define GPIO2_BASE_ADDR		0x53fd0000
#define SDMA_BASE_ADDR		0x53fd4000
#define RTC_BASE_ADDR		0x53fd8000
#define WDOG_BASE_ADDR		0x53fdc000
#define PWM_BASE_ADDR		0x53fe0000
#define RTIC_BASE_ADDR		0x53fec000

#define AVIC_BASE_ADDR		0x68000000

#define NAND_BASE_ADDR		0xb8000000
#define ESD_CTRL_BASE_ADDR	0xb8001000
#define WEIM_BASE_ADDR		0xb8002000
#define M3IF_BASE_ADDR		0xb8003000
#define PCMCIA_IF_BASE_ADDR	0xb8004000
#define PCMCIA_MEM_BASE_ADDR	0xbc000000

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+ IOMUXC - offset
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
#define GPR			0x0008
#define SW_MUX_CTL_BASE		0x000c
#define SW_PAD_CTL_BASE		0x0154

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+ IIM - offset
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
#define SREV			0x0024
#define HAB1			0x0814

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+ WEIM - offset
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
#define CSCR0U			0x00
#define CSCR0L			0x04
#define CSCR0A			0x08
#define CSCR3U			0x30
#define CSCR3L			0x34
#define CSCR3A			0x38
#define CSCR4U			0x40
#define CSCR4L			0x44
#define CSCR4A			0x48

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+ UART - offset
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
#define URXD			0x00
#define UTXD			0x40
#define UCR1			0x80
#define UCR2			0x84
#define UCR3			0x88
#define UCR4			0x8c
#define UFCR			0x90
#define USR1			0x94
#define USR2			0x98
#define UESC			0x9c
#define UTIM			0xa0
#define UBIR			0xa4
#define UBMR			0xa8
#define UBRC			0xac
#define ONEMS			0xb0
#define UTS			0xb4

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+ CCM - offset
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
#define CCMR			0x00
#define PDR0			0x04
#define PDR1			0x08
#define MPCTL			0x10
#define UPCTL			0x14
#define SPCTL			0x18

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+ GPIO - offset
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
#define DR			0x00
#define GDIR			0x04
#define PSR			0x08
#define ICR1			0x0c
#define ICR2			0x10
#define IMR			0x14
#define ISR			0x18

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+ PCMCIA - offset
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
#define PIPR			0x00
#define PSCR			0x04
#define PER			0x08
#define PBR0			0x0c
#define PBR1			0x10
#define PBR2			0x14
#define PBR3			0x18
#define PBR4			0x1c
#define POR0			0x28
#define POR1			0x2c
#define POR2			0x30
#define POR3			0x34
#define POR4			0x38
#define POFR0			0x44
#define POFR1			0x48
#define POFR2			0x4c
#define POFR3			0x50
#define POFR4			0x54
#define PGCR			0x60
#define PGSR			0x64

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+ CSPI - offset
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
#define RXDATA			0x00
#define TXDATA			0x04
#define CONREG			0x08
#define INTREG			0x0c
#define DMAREG			0x10
#define STATREG			0x14
#define PERIODREG		0x18

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+ IPU - offset
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
#define SDC_PWM_CTRL		0xC8

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+ EPIT - offset
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
#define EPITCR			0x00
#define EPITSR			0x04
#define EPITLR			0x08
#define EPITCMPR		0x0c
#define EPITCNT			0x10

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+ SDHC - offset
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
#define SDHC(x)			(SDHC_##x)
#define SDHC_STR_STP_CLK	0x00
#define SDHC_STATUS		0x04
#define SDHC_CLK_RATE		0x08
#define SDHC_CMD_DAT_CONT	0x0c
#define SDHC_RES_TO		0x10
#define SDHC_READ_TO		0x14
#define SDHC_BLK_LEN		0x18
#define SDHC_NOB		0x1c
#define SDHC_REV_NO		0x20
#define SDHC_INT_CNTR		0x24
#define SDHC_CMD		0x28
#define SDHC_ARG		0x2c
#define SDHC_RES_FIFO		0x34
#define SDHC_BUFFER_ACCESS	0x38

#endif
