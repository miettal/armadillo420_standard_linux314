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

	orr	r0, r3, #0x930000
	mov	r2, #0xaa
	str	r2, [r0, #0xc0]
	mov	r2, #0x0a000000
	orr	r2, r2, #0x040000
	orr	r2, r2, #0x0d00
	str	r2, [r0, #0x80]
	
		/***GPIO***/
	orr	r0, r3, #0x840000
	
		/*PHDDR=0x00*/
	mov	r1, #0x00
	str	r1, [r0, #0x44]
	ldr	r1, [r0, #0x40]
	and	r4, r1, #0x1c

		/**
		 * PortE Settings
		 * A210.A and B:		PEDDR=0x03, PEDR=0x03
		 * A210.C or later, A2x0:	PEDDR=0x03, PEDR=0x01
		 **/
	mov	r1, #0x03
	str	r1, [r0, #0x24]
	cmp	r4, #0x00
	moveq	r1, #0x03
	movne	r1, #0x01
	str	r1, [r0, #0x20]
	
		/*PADDR=0x00, PADR=0x00*/
	mov	r1, #0x00
	str	r1, [r0, #0x10]
	str	r1, [r0, #0x00]
	
		/*PBDDR=0x20*/
	mov	r1, #0x20
	str	r1, [r0, #0x14]
	
		/*PCDDR=0x00*/
	cmp	r4, #0x00
	moveq	r1, #0x00	/* a210(Rev.A or B) */
	movne	r1, #0x80	/* a2[2|3|4]0,a210(Rev.C or later) */
	str	r1, [r0, #0x18]
	str	r1, [r0, #0x08]
	
		/*PDDDR=0x00*/
	mov	r1, #0x00
	str	r1, [r0, #0x1c]

		/*PFDDR=0x00*/
	mov	r1, #0x00
	str	r1, [r0, #0x34]
	
		/*PGDDR=0x04, PGDR=0x00*/
	mov	r1, #0x04
	str	r1, [r0, #0x3c]
	mov	r1, #0x00
	str	r1, [r0, #0x38]

		/*PHDR&0x10?*/
	ldr	r6, [r0, #0x40]
	and	r6, r6, #0x20
	mov	r6, r6, lsr #3 /* 0x20 => 0x04 */

		/***SMC***/
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

		/***Syscon***/
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

	/* USBDIV:4,PSDIV:2 */
	mov	r1, #0x30000000
	orr	r1, r1, #0x0d0000
	/* S*25/24*25 = 384.000000MHz */
	orr	r1, r1, #0xc300
	orr	r1, r1, #0x17
	str	r1, [r0, #0x24]

	mrc	p15, 0, r1, c1, c0, 0
	orr	r1, r1, #0xc0000000
	mcr	p15, 0, r1, c1, c0, 0

	mov	r1, #80
1:	subs	r1, r1, #1
	bne	1b

		/***SDRAM***/
	orr	r0, r3, #0x060000
	mov	r1, #0x210000
	orr	r1, r1, #0x1000000
	orr	r1, r1, #0x2c /*ExternalBusWidth:16bits*/
	bic	r1, r1, r6    /*32bit?*/
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
		/*SDRAM*/
	mov	r4, #0xc0000000
	tst	r6, #0x04
	orreq	r4, r4, #0x4600 /*16bit*/
	orrne	r4, r4, #0x8800 /*32bit*/
	ldr	r1, [r4]
	str	r2, [r0, #0x04]

	mov	r4, #0xc0000000
	ldr	r1, [r4]
	tst	r6, #0x04
	orreq	r5, r4, #0x200000 /*16bit*/
	orrne	r5, r4, #0x400000 /*32bit*/
	ldr	r1, [r5]
	orreq	r5, r4, #0x400000 /*16bit*/
	orrne	r5, r4, #0x800000 /*32bit*/
	ldr	r1, [r5]
	orreq	r5, r4, #0x600000 /*16bit*/
	orrne	r5, r4, #0xc00000 /*32bit*/
	ldr	r1, [r5]

		/***SDRAM***/
	orr	r0, r3, #0x060000
	mov	r1, #0x210000
	orr	r1, r1, #0x2c /*ExternalBusWidth:16bits*/
	bic	r1, r1, r6    /*32bit?*/
	str	r1, [r0, #0x10]

		/***Syscon***/
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

		/***Syscon***/
	orr	r0, r3, #0x930000
	mov	r2, #0xaa
	str	r2, [r0, #0xc0]
	ldr	r1, [r0, #0x04]
	orr	r1, r1, #0x20000000
	str	r1, [r0, #0x04]
	str	r2, [r0, #0xc0]
	ldr	r1, [r0, #0x80]
	orr	r1, r1, #0x0a000000 /***/
	orr	r1, r1, #0x00140000 /***/
	orr	r1, r1, #0x00000d00 /***/
	str	r1, [r0, #0x80]

		/***UART1***/
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

	ldr	sp, =STACK_START	/* init stack pointer */
	ldr	r0, =DRAM_HERMIT_START
	ldr	r5, =entry
	bx	r5			/* see entry.c */
1:	b	1b
