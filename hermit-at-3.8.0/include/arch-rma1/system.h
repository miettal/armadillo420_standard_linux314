#ifndef _HERMIT_ARCH_RMA1_SYSTEM_H_
#define _HERMIT_ARCH_RMA1_SYSTEM_H_

#include <boost.h>

/* GPIO */
#define GPIO_BASE_ADDR		0xe6050000

/* SCIFA */
#define SCIFA0_BASE_ADDR	0xe6c40000
#define SCIFA1_BASE_ADDR	0xe6c50000
#define SCIFA2_BASE_ADDR	0xe6c60000
#define SCIFA3_BASE_ADDR	0xe6c70000
#define SCIFA4_BASE_ADDR	0xe6c80000
#define SCIFA5_BASE_ADDR	0xe6cb0000
#define SCIFA6_BASE_ADDR	0xe6cc0000
#define SCIFA7_BASE_ADDR	0xe6cd0000
#define SCIFB_BASE_ADDR		0xe6c30000
#define SCIF_SMR	0x00
#define SCIF_BRR	0x04
#define SCIF_SCR	0x08
#define SCIF_TDSR	0x0c
#define SCIF_FER	0x10
#define SCIF_SSR	0x14
#define 	SSR_RDF		(1 << 1)
#define 	SSR_DR		(1 << 0)
#define SCIF_FCR	0x18
#define SCIF_PCR	0x30
#define SCIF_PDR	0x34

#define SCIFA_FDR	0x1c
#define		SCIFA_FDR_MASK	0x7f
#define 	SCIFA_FDR_FULL	0x3f
#define SCIFA_FTDR	0x20
#define SCIFA_FRDR	0x24

#define SCIFB_TFDR	0x38
#define SCIFB_RFDR	0x3c
#define		SCIFB_FDR_MASK	0x1ff
#define		SCIFB_FDR_FULL	0x0ff
#define SCIFB_FTDR	0x40
#define SCIFB_FRDR	0x60

/* I2C */
#define I2C0_BASE_ADDR		(mmu_active ? 0xe5f20000 : 0xfff20000)
#define I2C1_BASE_ADDR		0xe6c20000
#define I2C(PORT, REG)		(I2C##PORT##_BASE_ADDR + I2C_##REG)
#define I2C_ICDR		0x00
#define I2C_ICCR		0x04
#define I2C_ICSR		0x08
#define I2C_ICIC		0x0c
#define I2C_ICCL		0x10
#define I2C_ICCH		0x14
#define I2C_ICTR		0x18
#define I2C_ICRR		0x1c
#define I2C_ICTA		0x20
#define I2C_ICTB		0x24
#define I2C_ICTC		0x28
#define I2C_ICTD		0x2c
#define I2C_ICSF		0x30
#define I2C_ICSTART		0x70

/* SDHI */
#define SDHI0_BASE_ADDR		0xe6850000
#define SDHI(PORT, REG)		(SDHI##PORT##_BASE_ADDR + SDHI_##REG)
#define SDHI_CMD		0x00
#define SDHI_PORTSEL		0x02
#define SDHI_ARG0		0x04
#define SDHI_ARG1		0x06
#define SDHI_STOP		0x08
#define SDHI_SECCNT		0x0a
#define SDHI_RSP0		0x0c
#define SDHI_RSP1		0x0e
#define SDHI_RSP2		0x10
#define SDHI_RSP3		0x12
#define SDHI_RSP4		0x14
#define SDHI_RSP5		0x16
#define SDHI_RSP6		0x18
#define SDHI_RSP7		0x1a
#define SDHI_INFO1		0x1c
#define SDHI_INFO2		0x1e
#define SDHI_INFO1_MASK		0x20
#define SDHI_INFO2_MASK		0x22
#define SDHI_CLK_CTRL		0x24
#define SDHI_SIZE		0x26
#define SDHI_OPTION		0x28
#define SDHI_ERR_STS1		0x2c
#define SDHI_ERR_STS2		0x2e
#define SDHI_BUF		0x30
#define SDHI_MODE		0x34
#define SDHI_SDIO_INFO		0x36
#define SDHI_SDIO_INFO_MASK	0x38
#define SDHI_CC_EXT_MODE	0xd8
#define SDHI_SOFT_RST		0xe0
#define SDHI_VERSION		0xe2
#define SDHI_EXT_ACC		0xe4
#define SDHI_EXT_SWAP		0xf0

/* MMCIF */
#define MMCIF_BASE_ADDR		0xe6bd0000
#define MMC(REG)		(MMCIF_BASE_ADDR + MMC_##REG)
#define MMC_CMD_SET		0x00
#define MMC_ARG			0x08
#define MMC_ARG_CMD12		0x0c
#define MMC_CMD_CTRL		0x10
#define MMC_BLOCK_SET		0x14
#define MMC_CLK_CTRL		0x18
#define MMC_BUF_ACC		0x1c
#define MMC_RESP3		0x20
#define MMC_RESP2		0x24
#define MMC_RESP1		0x28
#define MMC_RESP0		0x2c
#define MMC_RESP_CMD12		0x30
#define MMC_DATA		0x34
#define MMC_BOOT		0x3c
#define MMC_INT			0x40
#define MMC_INT_MASK		0x44
#define MMC_HOST_STS1		0x48
#define MMC_HOST_STS2		0x4c
#define MMC_VERSION		0x7c
#define MMC_DELAY_SEL		0x80

/* TMU */
#define TMU0_BASE_ADDR		(mmu_active ? 0xe5f80000 : 0xfff80000)
#define TMU1_BASE_ADDR		(mmu_active ? 0xe5f80000 : 0xfff90000)
#define TMU(PORT, REG)		(TMU0_BASE_ADDR + TMU_##REG)
#define TMU_TSTR		0x04
#define TMU_TCOR0		0x08
#define TMU_TCNT0		0x0c
#define TMU_TCR0		0x10
#define TMU_TCOR01		0x14
#define TMU_TCNT01		0x18
#define TMU_TCR01		0x1c

/* CPG */
#define CPG_BASE_ADDR		0xe6150000
#define CPG(REG)		(CPG_BASE_ADDR + CPG_##REG)
#define CPG_MSTPSR		0x030
#define CPG_MSTPCR		0x130
#define REG_MSTPSR(nr)		(CPG_BASE_ADDR + CPG_MSTPSR + nr * 4)
#define REG_MSTPCR(nr)		(CPG_BASE_ADDR + CPG_MSTPCR + nr * 4)

#define cpg_set_mstpcr(nr)				\
({							\
	u32 _v = read32(REG_MSTPCR((nr) / 100));	\
	_v |= (1 << ((nr) % 100));			\
	write32(REG_MSTPCR((nr) / 100), _v);		\
})
#define cpg_clr_mstpcr(nr)				\
({							\
	u32 _v = read32(REG_MSTPCR((nr) / 100));	\
	_v &= ~(1 << ((nr) % 100));			\
	write32(REG_MSTPCR((nr) / 100), _v);		\
})

/* SYSC */
#define SYSC_BASE_ADDR		0xe6180000
#define SYSC(REG)		(SYSC_BASE_ADDR + SYSC_##REG)
#define SYSC_RESCNT		0x801c

#define BOOT_MODE_NOR		0x08
#define BOOT_MODE_SD		0x28
#define BOOT_MODE_EMMC		0x38

/* RWDT */
#define RWDT_BASE_ADDR		0xe6020000
#define RWTCNT0			(RWDT_BASE_ADDR + 0x00)
#define RWTCSRA0		(RWDT_BASE_ADDR + 0x04)
#define RWTCSRB0		(RWDT_BASE_ADDR + 0x08)

#endif

