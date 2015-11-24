#include <autoconf.h>

#if __ARM_ARCH__ < 7
	.type	arch_boost_on, %function
	.global arch_boost_on
arch_boost_on:
	stmdb	sp!, {r0, r5}
	mov	r5, r0
	mov	r0, #0
	mcr	p15, 0, r0, c7, c7
	mcr	p15, 0, r0, c7, c10, 4
	mcr	p15, 0, r0, c8, c7
	mcr	p15, 0, r5, c2, c0
	mov	r0, #0x03
	mcr	p15, 0, r0, c3, c0

	/* CP15:cr1 Control register
	 *     M[ 0]: MMU Enable
	 *     C[ 2]: D-Cache Enable
	 *     W[ 3]: Write-buffer Enable
	 *     Z[11]: Bredict Enable
	 *     I[12]: I-Cache Enable
	 *     V[13]: High-vector */
	mrc	p15, 0, r0, c1, c0
	orr	r0, r0, #0x3800
	orr	r0, r0, #0x000d
#if __ARM_ARCH__ == 6
	orr	r0, r0, #(1 << 31)
#endif
	mcr	p15, 0, r0, c1, c0

	ldmia	sp!, {r0, r5}
	bx	lr

	.type	arch_boost_off, %function
	.global arch_boost_off
arch_boost_off:
	stmdb	sp!, {r0, r1, lr}
	mov	r0, #0
#if __ARM_ARCH__ == 6
	mcr	p15, 0, r0, c7, c14, 0	/* D cache clean+invalidate */
	mcr	p15, 0, r0, c7, c10, 4	/* drain write buffer */
#elif __ARM_ARCH__ == 5
1:	mrc	p15, 0, r15, c7, c14, 3	/* D cache:                */
	bne	1b			/*   test+clean+invalidate */
#elif __ARM_ARCH__ == 4
	mov	r1, #7 << 5
1:	orr	r0, r1, #63 << 26
2:	mcr	p15, 0, r0, c7, c14, 2
	subs	r0, r0, #1 << 26
	bcs	2b
	subs	r1, r1, #1 << 5
	bcs	1b
	mov	r0, #0
#endif

#if __ARM_ARCH__ == 6
	bl	v6_icache_inval_all
#endif

	mrc	p15, 0, r0, c1, c0, 0	/* ctrl register */
	bic	r0, r0, #0x05
	bic	r0, r0, #0x1000
#if __ARM_ARCH__ == 6
	bic	r0, r0, #0x80000000
#endif
	mcr	p15, 0, r0, c1, c0, 0	/* disable caches */

	ldmia	sp!, {r0, r1, lr}
	bx	lr

#if __ARM_ARCH__ == 6
	.global v6_icache_inval_all
v6_icache_inval_all:
	stmdb	sp!, {r0, r1}
	mov	r0, #0
	mrs	r1, cpsr
	mcr	p15, 0, r0, c7, c5, 0	/* invalidate entire I-cache */
	mcr	p15, 0, r0, c7, c5, 0	/* invalidate entire I-cache */
	mcr	p15, 0, r0, c7, c5, 0	/* invalidate entire I-cache */
	mcr	p15, 0, r0, c7, c5, 0	/* invalidate entire I-cache */
	msr	cpsr_cx, r1		/* restore interrupts */
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	ldmia	sp!, {r0, r1}
	bx	lr
#endif

#else /* __ARM_ARCH__ < 7 */

/*
 *	v7_flush_icache_all()
 *
 *	Flush the whole I-cache.
 *
 *	Registers:
 *	r0 - set to 0
 *
 *	Based on linux-3.2/arch/arm/mm/cache-v7.S
 */
v7_flush_icache_all:
	mov	r0, #0
	mcr	p15, 0, r0, c7, c1, 0
	mcr	p15, 0, r0, c7, c5, 0
	bx	lr

/*
 *	v7_inv_dcache_all()
 *
 *	Invalidate the whole D-cache.
 *
 *	Corrupted registers: r0-r7, r9-r11
 *
 *	Based on linux-2.6.35/arch/arm/mm/cache-v7.S
 */
v7_flush_dcache_all:
	dmb					@ ensure ordering with previous memory accesses
	mrc	p15, 1, r0, c0, c0, 1		@ read clidr
	ands	r3, r0, #0x7000000		@ extract loc from clidr
	mov	r3, r3, lsr #23			@ left align loc bit field
	beq	finished			@ if loc is 0, then no need to clean
	mov	r10, #0				@ start clean at cache level 0
loop1:
	add	r2, r10, r10, lsr #1		@ work out 3x current cache level
	mov	r1, r0, lsr r2			@ extract cache type bits from clidr
	and	r1, r1, #7			@ mask of the bits for current cache only
	cmp	r1, #2				@ see what cache we have at this level
	blt	skip				@ skip if no cache, or just i-cache
	mcr	p15, 2, r10, c0, c0, 0		@ select current cache level in cssr
	isb					@ isb to sych the new cssr&csidr
	mrc	p15, 1, r1, c0, c0, 0		@ read the new csidr
	and	r2, r1, #7			@ extract the length of the cache lines
	add	r2, r2, #4			@ add 4 (line length offset)
	ldr	r4, =0x3ff
	ands	r4, r4, r1, lsr #3		@ find maximum number on the way size
	clz	r5, r4				@ find bit position of way size increment
	ldr	r7, =0x7fff
	ands	r7, r7, r1, lsr #13		@ extract max number of the index size
loop2:
	mov	r9, r4				@ create working copy of max way size
loop3:
	lsl	r6, r9, r5
	orr	r11, r10, r6			@ factor way and cache number into r11
	lsl	r6, r7, r2
	orr	r11, r11, r6			@ factor index number into r11
	mcr	p15, 0, r11, c7, c14, 2		@ clean & invalidate by set/way
	subs	r9, r9, #1			@ decrement the way
	bge	loop3
	subs	r7, r7, #1			@ decrement the index
	bge	loop2
skip:
	add	r10, r10, #2			@ increment cache number
	cmp	r3, r10
	bgt	loop1
finished:
	mov	r10, #0				@ swith back to cache level 0
	mcr	p15, 2, r10, c0, c0, 0		@ select current cache level in cssr
	dsb
	isb
	bx	lr

/*
 *	v7_inv_dcache_all()
 *
 *	Invalidate the whole D-cache.
 *
 *	Corrupted registers: r0-r7, r9-r11
 *
 *	Based on linux-2.6.35/arch/arm/mm/cache-v7.S
 */
v7_inv_dcache_all:
	dmb					@ ensure ordering with previous memory accesses
	mrc	p15, 1, r0, c0, c0, 1		@ read clidr
	ands	r3, r0, #0x7000000		@ extract loc from clidr
	mov	r3, r3, lsr #23			@ left align loc bit field
	beq	invfinished			@ if loc is 0, then no need to clean
	mov	r10, #0				@ start clean at cache level 0
invloop1:
	add	r2, r10, r10, lsr #1		@ work out 3x current cache level
	mov	r1, r0, lsr r2			@ extract cache type bits from clidr
	and	r1, r1, #7			@ mask of the bits for current cache only
	cmp	r1, #2				@ see what cache we have at this level
	blt	invskip				@ skip if no cache, or just i-cache
	mcr	p15, 2, r10, c0, c0, 0		@ select current cache level in cssr
	isb					@ isb to sych the new cssr&csidr
	mrc	p15, 1, r1, c0, c0, 0		@ read the new csidr
	and	r2, r1, #7			@ extract the length of the cache lines
	add	r2, r2, #4			@ add 4 (line length offset)
	ldr	r4, =0x3ff
	ands	r4, r4, r1, lsr #3		@ find maximum number on the way size
	clz	r5, r4				@ find bit position of way size increment
	ldr	r7, =0x7fff
	ands	r7, r7, r1, lsr #13		@ extract max number of the index size
invloop2:
	mov	r9, r4				@ create working copy of max way size
invloop3:
	lsl	r6, r9, r5
	orr	r11, r10, r6			@ factor way and cache number into r11
	lsl	r6, r7, r2
	orr	r11, r11, r6			@ factor index number into r11
	mcr	p15, 0, r11, c7, c6, 2		@ invalidate by set/way
	subs	r9, r9, #1			@ decrement the way
	bge	invloop3
	subs	r7, r7, #1			@ decrement the index
	bge	invloop2
invskip:
	add	r10, r10, #2			@ increment cache number
	cmp	r3, r10
	bgt	invloop1
invfinished:
	mov	r10, #0				@ swith back to cache level 0
	mcr	p15, 2, r10, c0, c0, 0		@ select current cache level in cssr
	dsb
	isb
	bx	lr

	.align	2
__v7_setup_stack:
	.space	4 * 16

	.type	arch_boost_on, %function
	.global arch_boost_on
arch_boost_on:
	stmdb	sp!, {r0, r12}

	adr	r12, __v7_setup_stack
	stmia	r12, {r0-r7, r9-r11, lr}
	bl	v7_inv_dcache_all
	ldmia	r12, {r0-r7, r9-r11, lr}

	mcr	p15, 0, r0, c2, c0
	mov	r0, #0x03
	mcr	p15, 0, r0, c3, c0

	/* CP15:cr1 Control register
	 *     M[ 0]: MMU Enable
	 *     C[ 2]: D-Cache Enable
	 *     W[ 3]: Write-buffer Enable
	 *     Z[11]: Bredict Enable
	 *     I[12]: I-Cache Enable
	 *     V[13]: High-vector */
	mrc	p15, 0, r0, c1, c0
	orr	r0, r0, #0x3800
	orr	r0, r0, #0x000d
	mcr	p15, 0, r0, c1, c0

	ldmia	sp!, {r0, r12}
	bx	lr

	.type	arch_boost_off, %function
	.global arch_boost_off
arch_boost_off:
	stmdb	sp!, {r0, r12, lr}

	adr	r12, __v7_setup_stack
	stmia	r12, {r0-r7, r9-r11, lr}
	bl	v7_flush_dcache_all
	ldmia	r12, {r0-r7, r9-r11, lr}
	bl	v7_flush_icache_all

	mrc	p15, 0, r0, c1, c0, 0	/* ctrl register */
	bic	r0, r0, #0x0007
	bic	r0, r0, #0x1000
	mcr	p15, 0, r0, c1, c0, 0	/* disable caches */

	ldmia	sp!, {r0, r12, lr}
	bx	lr

#endif /* __ARM_ARCH__ < 7 */
