/* this sets the stack pointer for the kernel
   the _start label is used by the linker as the entry point for the program */
.global _start
_start:
	ldr sp, =0x07FFFFFF
	bl main

/* don't call this, for reference only */
.type activeate, %function
.global activate
activate:
	/* r0 is the user mode stack pointer */
	/* #0x10 is the magic number that has the right bit set for user mode */
	/* ldmfd r0!, {r1,r2,...} <- Same as pop, but use r0 as the stack pointer */

	/* first restore named registers */
	ldmfd r0!, {r1,r2} /* pc, SPSR */
	mov lr, r1
	msr SPSR, r2

	/* now set the stack pointer in system mode (which is shared with user mode) and restore the numbered registers */
	msr CPSR_c, #0xDF /* system mode */
	mov sp, r0
	pop {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14}
	msr CPSR_c, #0xD3 /* supervisor mode */

	/* finally switch to user mode and run the program */
	movs pc, lr

.type yield, %function
.global yield
yield:
	svc #0x0
	bx lr

