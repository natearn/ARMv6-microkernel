#include "versatilepb.h"
#include "asm.h"

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
	size_t stack_size = 10000;
	unsigned int user_stack[stack_size];

	activate(&first, user_stack + stack_size);

	while(1); /* We can't exit, there's nowhere to go */
	return 0;
}
