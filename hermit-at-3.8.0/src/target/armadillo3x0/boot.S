/*
 * boot.S: assembly language bootstrap
 *
 * Copyright (c) 2006 Atmark Techno, Inc.
 */

#include <autoconf.h>
#include "board.h"
#include <arch/ns9750.h>
#include <arch/ioregs.h>

#if defined(CONFIG_SYSTEM_ENTRYPOINT_FLASH)
	.section .flash.boot
	.global _start_flash
_start_flash:
#elif defined(CONFIG_SYSTEM_ENTRYPOINT_RAM)
	.section .ram.boot
	.global _start_ram
_start_ram:
#endif

#if defined(CONFIG_SYSTEM_ENTRYPOINT_FLASH)
	orr	pc, pc, #0x50000000
	nop

	mov	r0, #0x0		/* MMU mandatory */
	mcr	p15, 0, r0, c1, c0, 0	/* MMU init */

	mov	r0, #0
	mcr	p15, 0, r0, c7, c7, 0	/* flush v3/v4 cache */
	mcr	p15, 0, r0, c8, c7, 0	/* flush v4 TLB */

	/* Red LED: ON */
	mov	r6, #A3X0_EXT_PHYS_BASE
	ldr	r0, [r6, #EXT_MISC_GCR]
	orr	r0, r0, #0x8
	str	r0, [r6, #EXT_MISC_GCR]
	
	mov	r3, #0xa0000000

	add	r0, r3, #0x700000	/* mem module */
	mov	r2, #0x1
	str	r2, [r0, #NS_MEM_CONTROL]
	mov	r2, #0x6
	str	r2, [r0, #NS_MEM_DYNAMIC_REFRESH]
	mov	r2, #0x1
	str	r2, [r0, #NS_MEM_DYNAMIC_READ_CONFIG]
	str	r2, [r0, #NS_MEM_DYNAMIC_TRP]
	mov	r2, #0x4
	str	r2, [r0, #NS_MEM_DYNAMIC_TRAS]
	mov	r2, #0x1
	str	r2, [r0, #NS_MEM_DYNAMIC_TAPR]
	mov	r2, #0x5
	str	r2, [r0, #NS_MEM_DYNAMIC_TDAL]
	mov	r2, #0x1
	str	r2, [r0, #NS_MEM_DYNAMIC_TWR]
	mov	r2, #0x6
	str	r2, [r0, #NS_MEM_DYNAMIC_TRC]
	str	r2, [r0, #NS_MEM_DYNAMIC_TRFC]
	mov	r2, #0x1
	str	r2, [r0, #NS_MEM_DYNAMIC_TRRD]
	str	r2, [r0, #NS_MEM_DYNAMIC_TMRD]

	mov	r1, #0x4000
	orr	r2, r1, #0x680
	str	r2, [r0, #NS_MEM_DYNAMIC_CONFIG_0]
	mov	r4, #0x200
	orr	r4, r4, #0x03
	str	r4, [r0, #NS_MEM_DYNAMIC_RAS_CAS_0]
	orr	r2, r1, #0x280
	str	r2, [r0, #NS_MEM_DYNAMIC_CONFIG_1]
	str	r4, [r0, #NS_MEM_DYNAMIC_RAS_CAS_1]
	str	r2, [r0, #NS_MEM_DYNAMIC_CONFIG_2]
	str	r4, [r0, #NS_MEM_DYNAMIC_RAS_CAS_2]
	str	r2, [r0, #NS_MEM_DYNAMIC_CONFIG_3]
	str	r4, [r0, #NS_MEM_DYNAMIC_RAS_CAS_3]

	mov	r2, #0x100
	add	r2, r2, #0x03
	str	r2, [r0, #NS_MEM_DYNAMIC_CONTROL]
	mov	r2, #0x1
	str	r2, [r0, #NS_MEM_DYNAMIC_REFRESH]

	mov	r2, #0xa
	str	r2, [r0, #NS_MEM_STATIC_EXTENDED_WAIT]

	mov	r1, #0x50
_sdram_settle:
	subs	r1, r1, #1
	bne	_sdram_settle

	mov	r2, #0x30
	str	r2, [r0, #NS_MEM_DYNAMIC_REFRESH]
	mov	r2, #0x83
	str	r2, [r0, #NS_MEM_DYNAMIC_CONTROL]

	mov	r2, #0x44000
	ldr	r2, [r2]

	mov	r2, #0x3
	str	r2, [r0, #NS_MEM_DYNAMIC_CONTROL]
	mov	r1, #0x84000
	orr	r2, r1, #0x680
	str	r2, [r0, #NS_MEM_DYNAMIC_CONFIG_0]
	orr	r2, r1, #0x280
	str	r2, [r0, #NS_MEM_DYNAMIC_CONFIG_1]
	str	r2, [r0, #NS_MEM_DYNAMIC_CONFIG_2]
	str	r2, [r0, #NS_MEM_DYNAMIC_CONFIG_3]

	mov	r2, #0x81
	str	r2, [r0, #NS_MEM_STATIC_CONFIG_1]
	orr	r2, r2, #0x100
	str	r2, [r0, #NS_MEM_STATIC_CONFIG_2]
	mov	r2, #0x80
	str	r2, [r0, #NS_MEM_STATIC_CONFIG_3]
	mov	r2, #0x0
	str	r2, [r0, #NS_MEM_STATIC_WAIT_WEN_1]
	str	r2, [r0, #NS_MEM_STATIC_WAIT_WEN_2]
	str	r2, [r0, #NS_MEM_STATIC_WAIT_WEN_3]
	str	r2, [r0, #NS_MEM_STATIC_WAIT_OEN_1]
	str	r2, [r0, #NS_MEM_STATIC_WAIT_OEN_2]
	str	r2, [r0, #NS_MEM_STATIC_WAIT_OEN_3]
	mov	r2, #0x0a
	str	r2, [r0, #NS_MEM_STATIC_WAIT_RD_1]
	mov	r2, #0x02
	str	r2, [r0, #NS_MEM_STATIC_WAIT_RD_2]
	mov	r1, #0x1f
	str	r1, [r0, #NS_MEM_STATIC_WAIT_RD_3]
	str	r1, [r0, #NS_MEM_STATIC_WAIT_PAGE_1]
	str	r1, [r0, #NS_MEM_STATIC_WAIT_PAGE_2]
	str	r1, [r0, #NS_MEM_STATIC_WAIT_PAGE_3]
	mov	r2, #0x0a
	str	r2, [r0, #NS_MEM_STATIC_WAIT_WR_1]
	mov	r2, #0x02
	str	r2, [r0, #NS_MEM_STATIC_WAIT_WR_2]
	str	r1, [r0, #NS_MEM_STATIC_WAIT_WR_3]
	mov	r2, #0x0F
	str	r2, [r0, #NS_MEM_STATIC_WAIT_TURN_1]
	str	r2, [r0, #NS_MEM_STATIC_WAIT_TURN_2]
	str	r1, [r0, #NS_MEM_STATIC_WAIT_TURN_3]

	add	r0, r3, #0x900000	/* sys module */
	mov	r2, #0xffffffff
	str	r2, [r0, #NS_SCM_AHB_BUS_ARBMON_TIMEOUT]
	mov	r2, #0x5
	str	r2, [r0, #NS_SCM_AHB_ERR_MON_CONFIG]
	mov	r2, #0x00000000
	add	r1, r0, #NS_SCM_MEM_CS4_DYNAMIC_BASE
	str	r2, [r1]
	mov	r2, #0x10000000
	add	r1, r0, #NS_SCM_MEM_CS5_DYNAMIC_BASE
	str	r2, [r1]
	mov	r2, #0x20000000
	add	r1, r0, #NS_SCM_MEM_CS6_DYNAMIC_BASE
	str	r2, [r1]
	mov	r2, #0x30000000
	add	r1, r0, #NS_SCM_MEM_CS7_DYNAMIC_BASE
	str	r2, [r1]
	mov	r2, #0x40000000
	add	r1, r0, #NS_SCM_MEM_CS0_DYNAMIC_BASE
	str	r2, [r1]
	mov	r2, #0x50000000
	add	r1, r0, #NS_SCM_MEM_CS1_DYNAMIC_BASE
	str	r2, [r1]
	mov	r2, #0x60000000
	add	r1, r0, #NS_SCM_MEM_CS2_DYNAMIC_BASE
	str	r2, [r1]
	mov	r2, #0x70000000
	add	r1, r0, #NS_SCM_MEM_CS3_DYNAMIC_BASE
	str	r2, [r1]
	mov	r2, #0xfc000000
	add	r1, r0, #NS_SCM_MEM_CS4_DYNAMIC_MASK
	str	r2, [r1]
	mov	r2, #0xf0000000
	add	r1, r0, #NS_SCM_MEM_CS5_DYNAMIC_MASK
	str	r2, [r1]
	add	r1, r0, #NS_SCM_MEM_CS6_DYNAMIC_MASK
	str	r2, [r1]
	add	r1, r0, #NS_SCM_MEM_CS7_DYNAMIC_MASK
	str	r2, [r1]
	add	r1, r0, #NS_SCM_MEM_CS0_DYNAMIC_MASK
	str	r2, [r1]
	mov	r2, #0xff000000
	orr	r2, r2, #0xc00000
	add	r1, r0, #NS_SCM_MEM_CS1_DYNAMIC_MASK
	str	r2, [r1]
	add	r1, r0, #NS_SCM_MEM_CS2_DYNAMIC_MASK
	str	r2, [r1]
	mov	r2, #0xf0000000
	add	r1, r0, #NS_SCM_MEM_CS3_DYNAMIC_MASK
	str	r2, [r1]
	mov	r2, #0x2
	str	r2, [r0, #NS_SCM_EXTINT0_CTRL]
	mov	r1, #0x80000000
	orr	r1, r1, #0x8000
	orr	r2, r1, #0x810000
	orr	r2, r2, #0x82
	str	r2, [r0, #NS_SCM_BRC0]
	orr	r2, r1, #0x840000
	orr	r2, r2, #0x85
	str	r2, [r0, #NS_SCM_BRC1]
	orr	r2, r1, #0x860000
	bic	r2, r2, #0xff00
	str	r2, [r0, #NS_SCM_BRC2]
	mov	r2, #0x00000000
	str	r2, [r0, #NS_SCM_BRC3]
	mov	r2, #0x1
	str	r2, [r0, #NS_SCM_AHB_GEN_CONFIG]

	mov	r3, #0x90000000

	add	r0, r3, #0x600000	/* bbu module */
	mov	r1, #0x0
	str	r1, [r0, #NS_BBU_MASTER_RESET]
	mov	r1, #0x080000
	orr	r1, r1, #0x0800
	orr	r1, r1, #0x08
	str	r1, [r0, #NS_BBU_GPIO_CONFIG_1]
	mov	r1, #0x33000000
	orr	r1, r1, #0x1b0000
	orr	r1, r1, #0x1b00
	orr	r1, r1, #0x08
	str	r1, [r0, #NS_BBU_GPIO_CONFIG_2]
	mov	r1, #0x99000000
	orr	r1, r1, #0x990000
	orr	r1, r1, #0x9900
	orr	r2, r1, #0x83
	str	r2, [r0, #NS_BBU_GPIO_CONFIG_3]
	orr	r1, r1, #0x99
	str	r1, [r0, #NS_BBU_GPIO_CONFIG_4]
	mov	r1, #0xaa000000
	orr	r1, r1, #0xaa0000
	orr	r1, r1, #0xaa00
	orr	r1, r1, #0xaa
	str	r1, [r0, #NS_BBU_GPIO_CONFIG_5]
	mov	r1, #0x33000000
	orr	r1, r1, #0x330000
	orr	r1, r1, #0x3300
	orr	r1, r1, #0xaa
	str	r1, [r0, #NS_BBU_GPIO_CONFIG_6]
	mov	r1, #0x33
	str	r1, [r0, #NS_BBU_GPIO_CONFIG_7]
	mov	r1, #0
	str	r1, [r0, #NS_BBU_USB_CONFIG]
	add	r0, r3, #0x200000	/* ser module */
	mov	r1, #0x03000000
	str	r1, [r0, #NS_SER_CHA_CTRL_A]
	str	r1, [r0, #NS_SER_CHB_CTRL_A]
	mov	r1, #0x08000000
	str	r1, [r0, #NS_SER_CHA_CTRL_B]
	str	r1, [r0, #NS_SER_CHB_CTRL_B]
	mov	r2, #0xa0000000
	add	r2, r2, #0x900000	/* sys module */
	ldr	r1, [r2, #NS_SCM_PLL_CONFIG]
	tst	r1, #0x02000000
	moveq	r1, #0xc0000000
	addeq	r1, r1, #0x140000
	addeq	r1, r1, #0x07
	movne	r1, #0x81000000
	addne	r1, r1, #0x19
	str	r1, [r0, #NS_SER_CHA_BITRATE]
	str	r1, [r0, #NS_SER_CHB_BITRATE]
	mov	r1, #0x80000000
	str	r1, [r0, #NS_SER_CHA_RX_BUF_TMR]
	str	r1, [r0, #NS_SER_CHB_RX_BUF_TMR]
	mov	r1, #0x83000000
	str	r1, [r0, #NS_SER_CHA_CTRL_A]
	str	r1, [r0, #NS_SER_CHB_CTRL_A]
#endif

	ldr	sp, =STACK_START		/* init stack pointer */
	ldr	r0, =DRAM_HERMIT_START
	blx	entry			/* see entry.c */
1:	b	1b
