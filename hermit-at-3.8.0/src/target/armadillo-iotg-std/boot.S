/*
 * boot.S: assembly language bootstrap
 *
 * Copyright (c) 2014 Atmark Techno, Inc.
 */

#include <autoconf.h>
#include <arch/system.h>
#include "board.h"

#if defined(CONFIG_SYSTEM_ENTRYPOINT_FLASH)
	.section .flash.boot
	.global _start_flash
_start_flash:
#elif defined(CONFIG_SYSTEM_ENTRYPOINT_RAM)
	.section .ram.boot
	.global _start_ram
_start_ram:
#else
#error not support entry-point
#endif

	/*********************************************************************
	 * invalidate I/D cache/TLB and drain write buffer
	 *********************************************************************/
	mov	r0, #0x00
	mcr	p15, 0, r0, c1, c0, 0
	mcr	p15, 0, r0, c7, c7, 0	/* invalidate I cache and D cache */
	mcr	p15, 0, r0, c8, c7, 0	/* invalidate TLBs */
	mcr	p15, 0, r0, c7, c10, 4	/* Drain the write buffer */

	/*********************************************************************
	 * Enable Watchdog Timer (if you want) Timeout: 10s
	 *********************************************************************/
#if defined(CONFIG_WDT_MX25)
	ldr	r0, =WDOG_BASE_ADDR
	ldr	r1, =0x137f
	strh	r1, [r0, #WDOG(WCR)]
	mov	r1, #0x00
	strh	r1, [r0, #WDOG(WMCR)]

	ldr	r0, =IOMUXC_SW_MUX_CTL_PAD_CONTRAST
	mov	r1, #MUX_ALT7
	str	r1, [r0]
#endif

	/*********************************************************************
	 * LED_ON (GREEN & RED)
	 *********************************************************************/
	ldr	r0, =0x43fac000 /* r0 = IOMUXC_BASE */
	mov	r2, #0x05
	str	r2, [r0, #0x78] /* MUX_CTL_PAD_NFALE */
	str	r2, [r0, #0x7c] /* MUX_CTL_PAD_NFCLE */
	ldr	r1, =GPIO3_BASE_ADDR
	ldr	r2, [r1, #0x04]
	orr	r2, r2, #0x30000000
	str	r2, [r1, #0x04]
	ldr	r2, [r1, #0x00]
	orr	r2, r2, #0x30000000
	str	r2, [r1, #0x00]

	/*********************************************************************
	 * get board ids
	 *********************************************************************/
	ldr	r0, =0x43fac000 /* r0 = IOMUXC_BASE */
	mov	r1, #0x05
	str	r1, [r0, #0x38] /* LA24 */
	str	r1, [r0, #0x3c] /* LA25 */
	ldr	r2, =0x53fd0000 /* r2 = GPIO2_BASE */
	mov	r1, #0x0
	str	r1, [r2, #0x04]
	ldr	r10, [r2, #0x00]
	mov	r3, #0x00000003 /* mask */
	and	r10, r3, r10, LSR #10 /* r10: board ids */
	mov	r1, #0x00
	str	r1, [r0, #0x38] /* LA24 */
	str	r1, [r0, #0x3c] /* LA25 */

	/*********************************************************************
	 * Initializing for PLLs & Clocks
	 *********************************************************************/
	ldr	r0, =CCM_BASE_ADDR
	ldr	r1, =0x000b2c01 /* MPLL=532MHz */
	str	r1, [r0, #0x00]
	ldr	r1, =0x00001400 /* UPLL=240MHz */
	str	r1, [r0, #0x04]
	ldr	r1, =0x2c034000
	str	r1, [r0, #0x08]
	nop
	nop
	nop
	nop
	ldr	r1, =0x4300ffff /* PER_CLK source is UPLL:240MHz */
	str	r1, [r0, #0x64]

	/*********************************************************************
	 * Initializing for Flash-Memory
	 *********************************************************************/
	ldr	r0, =WEIM_BASE_ADDR
	ldr	r1, =0x21414e85
	str	r1, [r0, #0x00]
	ldr	r1, =0x45002553
	str	r1, [r0, #0x04]
	ldr	r1, =0x00470800
	str	r1, [r0, #0x08]

#if defined(CONFIG_SYSTEM_ENTRYPOINT_FLASH)
	/*********************************************************************
	 * Initializing for Mobile-DDR
	 *   if r10 == 0 then 32MB
	 *   if r10 == 1 then 64MB
	 *   if r10 == 2 then 128MB
	 *********************************************************************/
	ldr	r0, =ESDCTL_BASE_ADDR
	/* ESDRAM Control
	 * if (r10 == 0x1)
	 *	ROW=13, COL=10, Rows/64ms@32kHz= 8192
	 * if (r10 == 0x2)
	 *	ROW=14, COL=10, Rows/64ms@32kHz=16384
	 * else
	 *	ROW=13, COL= 9, Rows/64ms@32kHz= 8192
	 **/
	ldr	r1, =0x82116080
	cmp	r10, #0x01
	ldreq	r1, =0x82216080
	cmp	r10, #0x02
	ldreq	r1, =0x83218080

	/* reset & wait 200us */
	ldr	r2, =0x00000006 /* MDDR_EN */
	str	r2, [r0, #0x10]
ddr_ready_wait:
	ldr	r2, [r0, #0x10]
	tst	r2, #0x80000000
	beq	ddr_ready_wait

	/* ESDRAM Configuration
	 * tXP=4clk, tWTR=2clk, tRP=4clk, tMRD=3clk,
	 * tWR=3clk, tRAS=8clk, tRRD=2clk, tCAS=3clk, tRCD=4clk, tRC=2clk
	 **/
	ldr	r2, =0x007ef73d
	str	r2, [r0, #0x04]

	/* Precharge ALL */
	orr	r2, r1, #0x10000000
	str	r2, [r0, #0x00]
	ldr	r2, =0x80000400
	ldrb	r3, [r2, #0x00]

	/* Auto-Refresh x2 */
	orr	r2, r1, #0x20000000
	str	r2, [r0, #0x00]
	ldr	r2, =0x80000000
	str	r3, [r2, #0x00]
	str	r3, [r2, #0x00]

	/* Load Mode Register */
	orr	r2, r1, #0x30000000
	str	r2, [r0, #0x00]

	/* Mode: CAS=3, BL=8 */
	ldr	r2, =0x80000000
	ldrb	r2, [r2, #0x33]

	/* Extended Mode: */
	ldr	r2, =0x80800000
	cmp	r10, #0x01
	ldreq	r2, =0x81000000
	cmp	r10, #0x02
	ldreq	r2, =0x82000000
	ldrb	r2, [r2, #0x00]

	/* Normal Mode: */
	str	r1, [r0, #0x00]
#endif

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
