/* vector.S */

	.section .vector
	.global _vector

_vector:
	ldr	pc, =_common_handler	/* exp_reset */
	ldr	pc, =_common_handler	/* exp_undefined */
	ldr	pc, =_common_handler	/* exp_software */
	ldr	pc, =_common_handler	/* exp_prefetch_abort */
	ldr	pc, =_common_handler	/* exp_data_abort */
	ldr	pc, =_common_handler	/* exp_reserved */
	ldr	pc, =_irq_handler	/* exp_irq */
	ldr	pc, =_common_handler	/* exp_fiq */

	.section .vector.handler
	.global _common_handler
_common_handler:
	bl	common_handler
1:	b	1b
	
	.global _irq_handler
_irq_handler:
	stmdb	sp!, {r0-r12, lr}
	mrs	r0, cpsr
	stmdb	sp!, {r0}
	bl	irq_handler
	ldmia	sp!, {r0}
	msr	cpsr, r0
	ldmia	sp!, {r0-r12, lr}
	subs	pc, lr, #4
