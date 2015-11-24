#ifndef _HERMIT_TARGET_ARMADILLO4X0_BOARD_H_
#define _HERMIT_TARGET_ARMADILLO4X0_BOARD_H_

#define FLASH_START		(0xa0000000)
#define FLASH_SIZE		(0x01000000)

#define DRAM_START		(0x80000000)
#define DRAM_SIZE		(0x04000000)

#define DRAM_HERMIT_START	(0x80800000)
#define DRAM_HERMIT_SIZE	(0x00300000)

#define STACK_IRQ_START		(STACK_IRQ_END + STACK_IRQ_SIZE)
#define STACK_IRQ_END		(0x80d00000)
#define STACK_IRQ_SIZE		(0x00100000)
#define STACK_SVC_START		(STACK_SVC_END + STACK_SVC_SIZE)
#define STACK_SVC_END		(0x80e00000)
#define STACK_SVC_SIZE		(0x00100000)

#define STACK_START		(STACK_SVC_START)
#define STACK_SIZE		(STACK_SVC_END)

#define VECTOR_START		(0x80f00000)
#define VECTOR_END		(0x00100000)

#ifndef __ASSEMBLER__

#include <htypes.h>
#include "board-armadillo460.h"

struct board_private {
	u8 mac1[8];	/* ORIG MAC1 address */
	u8 umac1[8];	/* USER MAC1 address */
	u8 dram;	/* DRAM ID */
	u8 jumper;	/* Jumper state */
	u8 tactsw;	/* Tact-SW state */
	u8 reserved;
	u16 type;	/* Board type */
#define BOARD_TYPE_ARMADILLO410		0x0410
#define BOARD_TYPE_ARMADILLO420		0x0420
#define BOARD_TYPE_ARMADILLO440		0x0440
#define BOARD_TYPE_ARMADILLO460		0x0460
	u16 hardware;	/* Hardware ID */
};
#endif
#endif
