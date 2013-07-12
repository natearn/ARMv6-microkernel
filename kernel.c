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
	char *string = "Hello, World!\n";
	unsigned int user_stack[1000];
	while(*string) {
		*(volatile char *)0x101f1000 = *string;
		string++;
	}
	activate(&first, user_stack);
	while(1); /* We can't exit, there's nowhere to go */
	return 0;
}
