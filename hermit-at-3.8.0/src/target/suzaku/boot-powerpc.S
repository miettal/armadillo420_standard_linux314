/*
 * boot.S: assembly language bootstrap
 *
 * Copyright (c) 2003-2005 Atmark Techno, Inc.
 *
 * Based on:
 *      Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#include "board.h"

	.section .boot
	.global _start
_start:

	lis	r1, (XPAR_OPB_GPIO_LED_BASEADDR)@h
	ori	r1, r1, (XPAR_OPB_GPIO_LED_BASEADDR)@l
	stw	r0, 0(r1)

	lis	r1, (DRAM_START+DRAM_SIZE)@h		/* init stack pointer */
	ori	r1, r1, (DRAM_START+DRAM_SIZE)@l	/* set sp to high up */
	subi	r1, r1, 16				/* reserve stack frame header (16byte) */
	
	bl	entry			/* see entry.c */

1:	bl	1b

