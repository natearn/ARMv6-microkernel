.global _start
_start:
	ldr sp, =0x07FFFFFF
	bl main

/* don't call this, for reference only */
.global _fun
.type _fun,%function
_fun:
	nop
