/*
 * boot.S: assembly language bootstrap
 *
 * Copyright (c) 2012 Atmark Techno, Inc.
 */

#include <autoconf.h>
#include "board.h"

	.section .vrl4.boot
	/* VRL4 Format Header */
	.long	0xea000000
	.long	0x00000056
	.long	0xe59ff008
	.long	0x00000000
	.long	0x00010160
	.long	0x00000001
	.long	0xe80c0160
	.long	0x00000000
	.long	0x00000000
	.skip	316

#if defined(CONFIG_SYSTEM_NEED_RELOCATE)
	.section .pre.boot
	.global _start
_start:
#else
#error not support entry-point
#endif

#if defined(CONFIG_SYSTEM_ENTRYPOINT_FLASH)
	ldr	pc, =_start_
_start_:
#endif
	/*********************************************************************
	 * invalidate I/D cache/TLB
	 *********************************************************************/
	mov	r0, #0x00
	mcr	p15, 0, r0, c1, c0, 0
	mcr	p15, 0, r0, c8, c7, 0	/* invalidate TLBs */
	mcr	p15, 0, r0, c7, c1, 0	/* invalidate I-cache inner shareable */
	mcr	p15, 0, r0, c7, c5, 0	/* I+BTB cache invalidate */
	mcr	p15, 0, r0, c7, c14, 1	/* clean & invalidate D / U line */

	msr	cpsr_c, #0x13	/* MODE: SVC */
	ldr	sp, =0xe8200000
	blx	arch_init

	/*********************************************************************
	 * Setup & Go!
	 *********************************************************************/
	msr	cpsr_c, #0x52	/* MODE: IRQ */
	ldr	sp, =STACK_IRQ_START
	msr	cpsr_c, #0x13	/* MODE: SVC */
	ldr	sp, =STACK_SVC_START
	ldr	r0, =DRAM_HERMIT_START
	blx	entry /* see entry.c */

1:	b	1b
