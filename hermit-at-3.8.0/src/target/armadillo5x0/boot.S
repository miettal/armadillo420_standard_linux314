/*
 * boot.S: assembly language bootstrap
 *
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */
#include <autoconf.h>
#include <arch/regmap.h>
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
	
##############################################################################
# Platform Initialize
	mov	r0, #0			/* MMU mandatory */
	mcr	p15, 0, r0, c1, c0, 0	/* MMU init */
	mcr	p15, 0, r0, c7, c7, 0	/* flush v3/v4 cache */
	mcr	p15, 0, r0, c8, c7, 0	/* flush v4 TLB */
	mcr	p15, 0, r0, c7, c10, 4	/* Drain the write buffer */

	/* Also setup the Peripheral Port Rename register inside the core */
	ldr	r0, =0x40000015
	mcr	p15, 0, r0, c15, c2, 4

	/* L2 Cache setup/invalidation/disable */
	ldr	r0, =L2CC_BASE_ADDR
	ldr	r2, [r0, #0x0100]
	bic	r2, r2, #1
	str	r2, [r0, #0x0100]

	/* Configure L2 Cache */
	ldr	r1, [r0, #0x0104]
	and	r1, r1, #0xfe000000
	ldr	r2, =0x00030024
	orr	r1, r1, r2
	str	r1, [r0, #0x0104]

	/* Invalidate L2 */
	mov	r1, #0xff
	str	r1, [r0, #0x077c]
L2_loop:
	ldr	r2, [r0, #0x077c]
	cmp	r2, #0
	bne	L2_loop

##############################################################################
# AIPS setup - Only setup MPROTx. The PACR default values are good.
	ldr	r0, =AIPS1_CTRL_BASE_ADDR
	ldr	r1, =0x77777777
	str	r1, [r0]
	str	r1, [r0, #0x04]
	ldr	r0, =AIPS2_CTRL_BASE_ADDR
	str	r1, [r0]
	str	r1, [r0, #0x04]

	ldr	r0, =AIPS1_CTRL_BASE_ADDR
	mov	r1, #0x00
	str	r1, [r0, #0x40]
	str	r1, [r0, #0x44]
	str	r1, [r0, #0x48]
	str	r1, [r0, #0x4c]
	ldr	r1, [r0, #0x50]
	and	r1, r1, #0x00ffffff
	str	r1, [r0, #0x50]
	
	ldr	r0, =AIPS2_CTRL_BASE_ADDR
	mov	r1, #0x00
	str	r1, [r0, #0x40]
	str	r1, [r0, #0x44]
	str	r1, [r0, #0x48]
	str	r1, [r0, #0x4c]
	ldr	r1, [r0, #0x50]
	and	r1, r1, #0x00ffffff
	str	r1, [r0, #0x50]

##############################################################################
# MAX (Multi-Layer AHB Crossbar Switch) setup

	ldr	r0, =MAX_BASE_ADDR
	ldr	r1, =0x00302154
	str	r1, [r0, #0x0000]
	str	r1, [r0, #0x0100]
	str	r1, [r0, #0x0200]
	str	r1, [r0, #0x0300]
	str	r1, [r0, #0x0400]

	mov	r1, #0x10
	str	r1, [r0, #0x0010]
	str	r1, [r0, #0x0110]
	str	r1, [r0, #0x0210]
	str	r1, [r0, #0x0310]
	str	r1, [r0, #0x0410]

	mov	r1, #0x00
	str	r1, [r0, #0x0800]
	str	r1, [r0, #0x0900]
	str	r1, [r0, #0x0a00]
	str	r1, [r0, #0x0b00]
	str	r1, [r0, #0x0c00]
	str	r1, [r0, #0x0d00]

##############################################################################
# Configure M3IF
	ldr	r0, =M3IF_BASE_ADDR
	mov	r1, #0x40
	str	r1, [r0]

##############################################################################
# PAD Control setup (To Support 133MHz DDR)
	
	ldr	r0, =IOMUXC_BASE_ADDR
	mov	r1, #0
	mov	r2, #0x1000
	str	r1, [r0, #0x26c] /* SDCLK */
	str	r1, [r0, #0x270] /* CAS */
	str	r1, [r0, #0x274] /* RAS */
	str	r2, [r0, #0x27c] /* CS2 (CSD0) */
	str	r1, [r0, #0x284] /* DQM3 */

	/* DQM2-DQM0, SD31-SD0, A25-A0, MA10(0x0288..0x02dc) */
	add	r0, r0, #0x288
	mov	r2, #0x16
pad_loop_:
	str	r1, [r0]
	add	r0, r0, #0x04
	subs	r2, r2, #0x01
	bne	pad_loop_

##############################################################################
# Board ID GPIO setup
	/* Borad ID: GPIO1_29/30/31 */
	ldr	r0, =IOMUXC_BASE_ADDR
	ldr	r1, =0x01010100
	str	r1, [r0, #0x054]

##############################################################################
# CS0 async mode setup
	ldr	r0, =WEIM_BASE_ADDR
	ldr	r1, =0x11414e85
	str	r1, [r0, #CSCR0U]
	ldr	r1, =0x45002553
	str	r1, [r0, #CSCR0L]
	ldr	r1, =0x00470800
	str	r1, [r0, #CSCR0A]

##############################################################################
# IPU Setup
	ldr	r0, =IPU_BASE_ADDR
	mov	r1, #0x40
	str	r1, [r0]

##############################################################################
# Clock Setup
	/* RVAL/WVAL for L2 cache memory */
	ldr	r0, =0x515
	ldr	r1, =CLK_CTRL_BASE_ADDR
	str	r0, [r1, #0x10]

	/* default setting
	 *   PLL@ MCU: 400MHz, USB: 240MHz
	 *   CLK@ MCU: 400MHz, HCLK: 133MHz, IPG: 66.5MHz
	 */

	ldr	r0, =CCM_BASE_ADDR
	ldr	r1, [r0, #CCMR]
	and	r1, r1, #0x06
	ldr	r2, =0x064b0b79 /* Default Value */
	orr	r1, r1, r2
	orr	r2, r1, #0x80
	bic	r2, r2, #0x308
	str	r2, [r0, #CCMR]

	ldr	r3, =PDR0_DEFAULT
	str	r3, [r0, #0x04]
	
	ldr	r3, =PDR1_DEFAULT
	str	r3, [r0, #0x08]
	
	and	r2, r2, #0x06
	cmp	r2, #0x4
	beq	_setup_pll_use_CKIH
_setup_pll_use_FPM:
	ldr	r2, =MPCTL_FPM_DEFAULT
	str	r2, [r0, #0x10]

	ldr	r2, =UPCTL_FPM_240
	str	r2, [r0, #0x14]

	ldr	r2, =SPCTL_FPM_294
	str	r2, [r0, #0x18]
	b	_setup_pll_start
_setup_pll_use_CKIH:
	ldr	r2, =MPCTL_CKIH_DEFAULT
	str	r2, [r0, #0x10]

	ldr	r2, =UPCTL_CKIH_240
	str	r2, [r0, #0x14]
	
	ldr	r2, =SPCTL_CKIH_294
	str	r2, [r0, #0x18]
_setup_pll_start:
	str	r1, [r0, #CCMR]
	nop
	nop
	nop
	nop
		
##############################################################################
# 32bit DDR Init
#if defined(CONFIG_SYSTEM_ENTRYPOINT_FLASH)
	ldr	r7, =GPIO1_BASE_ADDR
	ldr	r8, [r7]
	and	r8, r8, #0xe0000000 /* r8 : Board ID */
	ldr	r0, =ESD_CTRL_BASE_ADDR
	mov	r1, #0x04
	str	r1, [r0, #0x10]
	ldr	r1, =0x006ac73d
	str	r1, [r0, #0x04]

	/* reset & wait 200us */
	ldr	r1, =0x80000000
	str	r1, [r0]
ddr_ready_wait:	
	ldr	r1, [r0, #0x10]
	tst	r1, #0x80000000
	beq	ddr_ready_wait

	/* Precharge ALL(A10=1) */
	ldr	r1, =0x92100000
	str	r1, [r0]

	ldr	r2, =0x80000f00
	ldr	r3, =0x12344321
	str	r3, [r2]

	/* Auto-Refresh x2 */
	ldr	r1, =0xa2100000
	str	r1, [r0]
	ldr	r2, =0x80000000
	str	r3, [r2]
	str	r3, [r2]

	/* Load Mode Register */
	ldr	r1, =0xb2100000
	str	r1, [r0]

	/* Mode */		
	ldr	r2, =0x80000033
	ldrb	r3, [r2]
	
	/* Extended Mode */
	tst	r8, #0x20000000
	ldreq	r2, =0x80800000 /* 256Mb */
	ldrne	r2, =0x81000000 /* 512Mb */
	ldrb	r3, [r2]

	/* Normal Read/Write */
	tst	r8, #0x20000000
	ldreq	r1, =0x82126080 /* 256Mb */
	ldrne	r1, =0x82226080 /* 512Mb */
	str	r1, [r0]

ram_test_loop:
	ldr	r2, =0x80000000
	ldr	r3, =0xdeadbeef
	str	r3, [r2]
	ldr	r4, [r2]
	cmp	r3, r4
	bne	ram_test_loop
	
	mov	r1, #0x0c
	str	r1, [r0, #0x10]
#endif	

##############################################################################
# Setup & Go!
	ldr	sp, =STACK_START
	ldr	r0, =DRAM_HERMIT_START
	blx	entry /* see entry.c */
1:	b	1b
