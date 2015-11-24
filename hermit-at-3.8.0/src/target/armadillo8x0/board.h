#ifndef _HERMIT_TARGET_ARMADILLO8X0_BOARD_H_
#define _HERMIT_TARGET_ARMADILLO8X0_BOARD_H_

#include <autoconf.h>
#if defined(CONFIG_PLATFORM_ARMADILLO840)
#include <platform/board-armadillo840.h>
#endif

#if !defined(FLASH_START)
#define FLASH_START		(0x00000000)
#endif
#define FLASH_SIZE		(0x00800000)

#define DRAM_START		(0x40000000)
#define DRAM_TYPE_DDR3          (0x00)
#define DRAM_TYPE_LPDDR2        (0x01)
#define DRAM_SIZE_128MB         (0x1b)
#define DRAM_SIZE_256MB         (0x1c)
#define DRAM_SIZE_512MB         (0x1d)
#define DRAM_SIZE_1GB           (0x1e)
#define DRAM_WIDTH_16BIT        (0x04)
#define DRAM_WIDTH_32BIT        (0x05)

#define DRAM_ID(type, size, width) ((type << 16) | (size << 8) | width)
#define DRAM_ID_TYPE(id)        ((id >> 16) & 0xff)
#define DRAM_ID_SIZE(id)        ((id >>  8) & 0xff)
#define DRAM_ID_WIDTH(id)       ((id >>  0) & 0xff)

#define FLASH_HERMIT_START	(0x00000000)
#define FLASH_HERMIT_SIZE	(0x00040000)

#define SRAM_HERMIT_START	(0xe80c0000)
#define SRAM_HERMIT_SIZE	(0x00040000)

#define DRAM_HERMIT_START	(DRAM_START + 0x00800000)
#define DRAM_HERMIT_SIZE	(0x00300000)

#define STACK_IRQ_START		(STACK_IRQ_END + STACK_IRQ_SIZE)
#define STACK_IRQ_END		(DRAM_START + 0x00d00000)
#define STACK_IRQ_SIZE		(0x00100000)
#define STACK_SVC_START		(STACK_SVC_END + STACK_SVC_SIZE)
#define STACK_SVC_END		(DRAM_START + 0x00e00000)
#define STACK_SVC_SIZE		(0x00100000)

#define STACK_START		(STACK_SVC_START)
#define STACK_SIZE		(STACK_SVC_END)

#define VECTOR_START		(DRAM_START + 0x00f00000)
#define VECTOR_END		(0x00100000)

#ifndef __ASSEMBLER__

#include <htypes.h>
#include <platform.h>

struct board_private {
	u8 mac1[8];	/* ORIG MAC1 address */
	u8 umac1[8];	/* USER MAC1 address */
	u8 jumper;	/* Jumper state */
	u8 tactsw[4];	/* Tact-SW state */

	u8 boot_mode;	/* MD status */

	u32 hw_type;	/* Board type */
	u32 dram_id;	/* DRAM Identification value */
#define BOARD_TYPE_ARMADILLO800EVA	0x08000000
	u16 product_rev_major;
	u16 product_rev_minor;
	u32 lot_num;	/* Lot Number */
	u32 serial_num;	/* Serial Number */

	char *board_name;
	void (*display_info)(struct platform_info *pinfo);
};
#endif
#endif
