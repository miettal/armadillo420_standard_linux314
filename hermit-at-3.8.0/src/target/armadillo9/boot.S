/*
 * boot.S: assembly language bootstrap
 *
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#include <autoconf.h>
#include "board.h"

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
	.long	0x53555243		/* CRUS */

	mov	r0, #0			/* MMU mandatory */
	mcr	p15, 0, r0, c1, c0, 0	/* MMU init */
	mcr	p15, 0, r0, c7, c7, 0	/* flush v3/v4 cache */
	mcr	p15, 0, r0, c8, c7, 0	/* flush v4 TLB */

	mov	r3, #0x80000000		/* I/O base */

	orr	r0, r3, #0x840000
	mov	r1, #0x08
	str	r1, [r0, #0x00]
	str	r1, [r0, #0x10]
	mov	r1, #0x20
	str	r1, [r0, #0x14]
	mov	r1, #0xf0
	str	r1, [r0, #0x18]
	mov	r1, #0x50
	str	r1, [r0, #0x08]
	mov	r1, #0x00
	str	r1, [r0, #0x1c]
	str	r1, [r0, #0x20]
	mov	r1, #0xff
	str	r1, [r0, #0x24]
	mov	r1, #0x00
	str	r1, [r0, #0x34]
	mov	r1, #0x0c
	str	r1, [r0, #0x3c]
	mov	r1, #0x00
	str	r1, [r0, #0x38]
	mov	r1, #0x00
	str	r1, [r0, #0x44]

	orr	r0, r3, #0x080000
	mov	r1, #0x18000000
	orr	r1, r1, #0x4d00
	orr	r1, r1, #0x23
	str	r1, [r0, #0x18]
	mov	r1, #0x4700
	orr	r1, r1, #0xe3
	str	r1, [r0, #0x04]
	orr	r1, r1, #0x10000000
	str	r1, [r0, #0x08]

	orr	r0, r3, #0x930000
	mov	r1, #0x02000000
	orr	r1, r1, #0xd00000
#if !defined(PLL_HI) || !defined(PLL_LO)
	/* S*29/31*29 = 400.033858MHz */
	/*orr	r1, r1, #0xe300*/
	/*orr	r1, r1, #0x9e*/
	/* S*21/24*31 = 399.974400MHz */
	orr	r1, r1, #0xa300
	orr	r1, r1, #0xd7
	/* S*21/16*19 = 367.718400MHz */
	/*orr	r1, r1, #0xa200*/
	/*orr	r1, r1, #0x4f*/
#else
	orr	r1, r1, #PLL_HI
	orr	r1, r1, #PLL_LO
#endif
	str	r1, [r0, #0x20]
	nop
	nop
	nop
	nop
	nop

	mov	r1, #0x30000000
	orr	r1, r1, #0x0d0000
	orr	r1, r1, #0xc300
	orr	r1, r1, #0x17
	str	r1, [r0, #0x24]

	mrc	p15, 0, r1, c1, c0, 0
	orr	r1, r1, #0xc0000000
	mcr	p15, 0, r1, c1, c0, 0

	mov	r1, #80
1:	subs	r1, r1, #1
	bne	1b
	orr	r0, r3, #0x060000
	mov	r1, #0x210000
	orr	r1, r1, #0x1000000
	orr	r1, r1, #0x28
	str	r1, [r0, #0x10]
	mov	r2, #0x80000000
	orr	r1, r2, #0x03
	str	r1, [r0, #0x04]
	mov	r1, #160
1:	subs	r1, r1, #1
	bne	1b
	orr	r1, r2, #0x01
	str	r1, [r0, #0x04]
	mov	r1, #10
	str	r1, [r0, #0x08]
	mov	r1, #80
1:	subs	r1, r1, #1
	bne	1b
	mov	r1, #780
	str	r1, [r0, #0x08]
	orr	r1, r2, #0x02
	str	r1, [r0, #0x04]
	mov	r4, #0xc0000000
	orr	r4, r4, #0x8800
	ldr	r1, [r4]
	str	r2, [r0, #0x04]

	mov r4, #0xc0000000
	ldr r1, [r4]
	orr r5, r4, #0x400000
	ldr r1, [r5]
	orr r5, r4, #0x800000
	ldr r1, [r5]
	orr r5, r4, #0xc00000
	ldr r1, [r5]

	orr	r0, r3, #0x060000
	mov	r1, #0x210000
	orr	r1, r1, #0x28
	str	r1, [r0, #0x10]

	orr	r0, r3, #0x930000
	mov	r1, #0x02000000
	orr	r1, r1, #0xa40000
#if !defined(PLL_HI) || !defined (PLL_LO)
	/* S*29/31*29 = 400.033858MHz */
	/*orr	r1, r1, #0xe300*/
	/*orr	r1, r1, #0x9e*/
	/* S*21/24*31 = 399.974400MHz */
	orr	r1, r1, #0xa300
	orr	r1, r1, #0xd7
	/* S*21/16*19 = 367.718400MHz */
	/*orr	r1, r1, #0xa200*/
	/*orr	r1, r1, #0x4f*/
#else
	orr	r1, r1, #PLL_HI
	orr	r1, r1, #PLL_LO
#endif
	str	r1, [r0, #0x20]
	nop
	nop
	nop
	nop
	nop

	orr	r0, r3, #0x930000
	mov	r2, #0xaa
	str	r2, [r0, #0xc0]
	ldr	r1, [r0, #0x04]
	orr	r1, r1, #0x20000000
	str	r1, [r0, #0x04]
	str	r2, [r0, #0xc0]
	ldr	r1, [r0, #0x80]
	orr	r1, r1, #0x08000000
	orr	r1, r1, #0x140000
	str	r1, [r0, #0x80]

	orr	r0, r3, #0x8c0000
	orr	r2, r3, #0x8d0000
	mov	r1, #0
	str	r1, [r0, #0x014]
	str	r1, [r2, #0x014]
	str	r1, [r0, #0x100]
	str	r1, [r0, #0x004]
	str	r1, [r2, #0x004]
	str	r1, [r0, #0x00c]
	str	r1, [r2, #0x00c]
	mov	r1, #7
	str	r1, [r0, #0x010]
	str	r1, [r2, #0x010]
	mov	r1, #0x60
	str	r1, [r0, #0x008]
	str	r1, [r2, #0x008]
	mov	r1, #1
	str	r1, [r0, #0x014]
	str	r1, [r2, #0x014]

	mov	r0, #0x10000000
	mov	r1, #0
	str	r1, [r0, #0x0e]
#endif

	ldr	sp, =STACK_START
	ldr	r0, =DRAM_HERMIT_START
	ldr	r5, =entry
	bx	r5			/* see entry.c */
1:	b	1b
