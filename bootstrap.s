/* this sets the stack pointer for the kernel
   the _start label is used by the linker as the entry point for the program */
.global _start
_start:

	/* set up the interrupt handler */
	mov r0, #0x08
	ldr r1, instr
	ldr r2, addr
	str r1, [r0, #0x0]
	str r2, [r0, #0x4] /* the address of the interrupt handler needs to be stored at a known location */

	/* set the stack pointer */
	ldr sp, =0x07FFFFFF

	/* run the kernel */
	bl main

	/* helpers */
	instr: ldr pc, addr
	addr: .word interrupt_handler

interrupt_handler:
	save_user_state:
		msr CPSR_c, #0xDF /* system mode */
		push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
		mov r0, sp
		msr CPSR_c, #0xD3 /* supervisor mode */
		mov r1, lr
		mrs r2, SPSR
		stmfd r0!, {r1,r2}

	restore_kernel_state:
		pop {r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}

	bx lr
	b activate
	loop: b loop

/* entering user mode */
.type activeate, %function
.global activate
activate:
	/* r0 is the user mode stack pointer */
	/* #0x10 is the magic number that has the right bit set for user mode */
	/* ldmfd r0!, {r1,r2,...} <- Same as pop, but use r0 as the stack pointer */

	save_kernel_state:
		push {r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}

	restore_user_state:
		/* first restore named registers */
		ldmfd r0!, {r1,r2} /* pc, SPSR */
		mov lr, r1
		msr SPSR, r2

		/* now set the stack pointer in system mode (which is shared with user mode) and restore the numbered registers */
		msr CPSR_c, #0xDF /* system mode */
		mov sp, r0
		pop {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,fp,ip,lr}
		msr CPSR_c, #0xD3 /* supervisor mode */

	/* finally switch to user mode and run the program */
	movs pc, lr

.type yield, %function
.global yield
yield:
	push {r7}
	ldr r7 ,=yield
	svc #0x0
	pop {r7}
	bx lr

.type fork, %function
.global fork
fork:
	push {r7}
	ldr r7 ,=fork
	svc #0x0
	pop {r7}
	bx lr
