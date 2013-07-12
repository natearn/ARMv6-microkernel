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

int first(void) {
	bwputs("busy-wait put string");
	while(1);
	return 0;
}

int main(void) {
	unsigned int user_stack[STACK_SIZE];

	user_stack[STACK_SIZE-16] = &first; /* (pc) program counter */
	user_stack[STACK_SIZE-15] = 0x10;   /* (SPSR) saved state */
	activate(user_stack + STACK_SIZE - 16);

	while(1); /* We can't exit, there's nowhere to go */
	return 0;
}
