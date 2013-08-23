/* this sets the stack pointer for the kernel
   the _start label is used by the linker as the entry point for the program */
.global _start
_start:

	/* set up the software interrupt handler */
	mov r0, #0x08
	ldr r1, soft
	ldr r2, soft_addr
	str r1, [r0, #0x0]
	str r2, [r0, #0x4] /* the address of the interrupt handler needs to be stored at a known location */

	/* set up the hardware interrupt handler */
	mov r0, #0x18
	ldr r1, hard
	ldr r2, hard_addr
	str r1, [r0, #0x0]
	str r2, [r0, #0x4] /* the address of the interrupt handler needs to be stored at a known location */

	/* set the stack pointer */
	ldr sp, =0x07FFFFFF

	/* run the kernel */
	bl main

	/* helpers */
	soft: ldr pc, soft_addr
	soft_addr: .word software_interrupt
	hard: ldr pc, hard_addr
	hard_addr: .word hardware_interrupt

software_interrupt:
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

debug:
	loop: b loop

hardware_interrupt:
	/* push r7 and lr onto user stack */
	msr CPSR_c, #0xDF /* system mode */
	push {r7}
	/* get lr and -4 from irq mode*/
	msr CPSR_c, #0xD2 /* irq mode */
	sub r7, lr, #0x4
	/* push th lr-4 onto user stack */
	msr CPSR_c, #0xDF /* system mode */
	push {r7}
	/* set supervisor mode lr to restore */
	msr CPSR_c, #0xD3 /* supervisor mode */
	ldr lr, =restore
	/* set r7 to PIC */
	ldr r7, =0x10140000 /* PIC address */
	/* run sowftware_interrupt which does the rest of the work */
	b software_interrupt

	/* THIS WILL RUN IN USER MODE WITH INTERUPTS AND EVERYTHING */
	restore:
		add sp, sp, #0x4
		pop {r7}
		ldr pc, [sp, #-0x8]

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
	ldr r7, =yield
	svc #0x0
	pop {r7}
	bx lr

.type fork, %function
.global fork
fork:
	push {r7}
	ldr r7, =fork
	svc #0x0
	pop {r7}
	bx lr

.type write, %function
.global write
write:
	push {r7}
	ldr r7, =write
	svc #0x0
	pop {r7}
	bx lr

.type read, %function
.global read
read:
	push {r7}
	ldr r7, =read
	svc #0x0
	pop {r7}
	bx lr
