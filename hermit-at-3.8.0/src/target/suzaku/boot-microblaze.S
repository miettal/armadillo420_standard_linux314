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

	addi	r1, r0, XPAR_OPB_GPIO_LED_BASEADDR
	sw	r0, r0, r1

	addi	r1, r0, DRAM_START	/* init stack pointer */
	addi	r1, r1, DRAM_SIZE	/* set sp to high up */
	
	brai	entry			/* see entry.c */

1:	bri	1b

