/* this sets the stack pointer for the kernel
   the _start label is used by the linker as the entry point for the program */
.global _start
_start:
	ldr sp, =0x07FFFFFF
	bl main

/* don't call this, for reference only */
.type _fun, %function
.global _fun
_fun:
	nop
