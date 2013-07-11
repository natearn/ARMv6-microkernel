#include "versatilepb.h"

void bwputs(char *s) {
	while(*s) {
		while(*(UART0 + UARTFR) & UARTFR_TXFF);
		*UART0 = *s;
		s++;
	}
}

int main(void) {
	char *string = "Hello, World!\n";
	while(*string) {
		*(volatile char *)0x101f1000 = *string;
		string++;
	}

	while(1); /* We can't exit, there's nowhere to go */
	return 0;
}
