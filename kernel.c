#include "versatilepb.h"
#include "asm.h"

#define STACK_SIZE 10000

void bwputs(char *s) {
	while(*s) {
		while(*(UART0 + UARTFR) & UARTFR_TXFF);
		*UART0 = *s;
		s++;
	}
}

int my_user_process(void) {
	while(1) {
		bwputs("yield");
		yield();
	}
	return 0;
}

int main(void) {
	unsigned int user_stack[STACK_SIZE];
	unsigned int *x;

	/* this is necessary to run something in user mode */
	user_stack[STACK_SIZE-16] = &my_user_process; /* (pc) program counter */
	user_stack[STACK_SIZE-15] = 0x10;   /* (SPSR) saved state */
	x = activate(user_stack + STACK_SIZE - 16);
	while(1) {
		bwputs("activate");
		x = activate(x);
	}

	while(1); /* We can't exit, there's nowhere to go */
	return 0;
}
