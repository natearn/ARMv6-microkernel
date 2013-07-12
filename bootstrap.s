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
	/* first set the stack pointer */
	msr CPSR_c, #0xDF
	mov sp, r1
	msr CPSR_c, #0xD3
	/* then switch to user mode and run the program */
	mov r2, #0x10  /* This magic number has the right bit set for user mode */
	msr SPSR, r2
	mov lr, r0 /* Load the address of first into lr */
	movs pc, lr
