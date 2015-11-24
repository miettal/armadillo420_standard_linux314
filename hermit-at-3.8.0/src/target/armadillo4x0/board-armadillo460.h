#ifndef _HERMIT_TARGET_ARMADILLO4X0_BOARD_ARMADILLO460_H_
#define _HERMIT_TARGET_ARMADILLO4X0_BOARD_ARMADILLO460_H_

#include <io.h>
#include <arch/system.h>

#define A460_CPLD_BASE		CS1_BASE_ADDR

/*
 * CPLD Register definitions
 */
/* macro */
#define cpld_read8(a)		read8(A460_CPLD_BASE + (a))
#define cpld_read16(a)		read16(A460_CPLD_BASE + (a))
#define cpld_write8(a, v)	write8(A460_CPLD_BASE + (a), (v))
#define cpld_write16(a, v)	write16(A460_CPLD_BASE + (a), (v))

/* register offset */
#define PISR0		(0x00)	/* Ext Interrupt Status0 */
#define PISR1		(0x01)	/* Ext Interrupt Status1 */
#define PISR		(PISR0)
#define PIMR0		(0x02)	/* Ext Interrupt Mask0 */
#define PIMR1		(0x03)	/* Ext Interrupt Mask1 */
#define PIMR		(PIMR0)
#define PIPT0		(0x04)	/* Ext Interrupt Polarity Type0 */
#define PIPT1		(0x05)	/* Ext Interrupt Polarity Type1 */
#define PIPT		(PIPT0)
#define PIDT0		(0x06)	/* Ext Interrupt Detection Type0 */
#define PIDT1		(0x07)	/* Ext Interrupt Detection Type1 */
#define PIDT		(PIDT0)
#define PBCR		(0x08)	/* Ext Bus Control */
#define PICR		(0x09)	/* Ext I/F Control */
#define PRTC		(0x0a)	/* RTC Control */
#define PVER		(0x0f)	/* CPLD Version */

/* PBCR: Ext Bus Control Register definitions */
#define PBCR_RST	(1 << 0)
#define PBCR_PC104	(0 << 1)
#define PBCR_ASYNC	(1 << 1)
#define PBCR_SYNC	(3 << 1)
#define PBCR_CLK_R	(1 << 3)

/* PBCR: Ext I/F Control Register definitions */
#define PICR_IF_EN	(1 << 0)
#define PICR_SEL1_C11	(1 << 1)
#define PICR_SEL1_C19	(0 << 1)
#define PICR_SEL2_C11	(1 << 2)
#define PICR_SEL2_C19	(0 << 2)
#define PICR_SEL_CON11	(3 << 1)
#define PICR_SEL_CON19	(0 << 1)

#endif
