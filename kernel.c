#include "versatilepb.h"
#include "stddef.h"
#include "asm.h"

#define NUM_STACKS 10
#define STACK_SIZE 1000

void bwputs(char *s) {
	while(*s) {
		while(*(UART0 + UARTFR) & UARTFR_TXFF);
		*UART0 = *s;
		s++;
	}
}

int task(void) {
	int x;
	while(1) {
		bwputs("task\n");
		x = fork();
		if(x == 0) {
			bwputs("I am the child\n");
		} else if(x > 0) {
			bwputs("I am the parent\n");
		} else if(x < 0) {
			bwputs("Something went wrong\n");
			debug(x,0,0);
		}
	}
	return 0;
}
/*
initialize a process given its stack
	returns the pointer to the top of the stack (needed by activate)
*/
unsigned int *init_process(unsigned int *stack, unsigned int size, int (*task)(void)) {
	/* this is necessary to run something in user mode */
	stack[size-16] = task; /* (pc) program counter */
	stack[size-15] = 0x10; /* (SPSR) saved state */
	return stack + size - 16;
}

unsigned int scheduler(unsigned int *procs[], unsigned int size, unsigned int active) {
	unsigned int i;
	if(procs[size-1] == procs[active]) return 0;
	for(i = 0; i < size-1; i++) {
		if(procs[i] == procs[active]) return i+1;
	}
	bwputs("scheduler error\n");
	debug(procs, size, active);
	return 0;
}

void *memcpy(void* dest, void *src, size_t size) {
	char *to = dest;
	const char *from = src;
	size_t i;
	for(i = 0; i < size; i++) {
		to[i] = from[i];
	}
	return dest;
}

void debug(unsigned int a, unsigned int b, unsigned int c) {
	bwputs("debug halt\n");
	while(1);
}

unsigned int _fork(unsigned int stacks[][STACK_SIZE], unsigned int *procs[], unsigned int parent, unsigned int next_pid) {
	unsigned int offset;
	bwputs("_fork\n");
	if(next_pid == NUM_STACKS) {
		procs[parent][2+0] = -1;
		return next_pid;
	}
	offset = procs[parent] - stacks[parent];
	procs[next_pid] = stacks[next_pid] + offset;
	memcpy(procs[next_pid], procs[parent], sizeof(*procs[parent]) * (STACK_SIZE - offset));
	procs[parent][2+0] = next_pid;
	procs[next_pid][2+0] = 0;
	return next_pid + 1;
}

int main(void) {
	unsigned int user_stack[NUM_STACKS][STACK_SIZE];
	unsigned int *procs[NUM_STACKS];
	unsigned int n = 0; /* the next available stack. n should never be >= NUM_STACKS */
	unsigned int active = 0;

	procs[n++] = init_process(user_stack[n],STACK_SIZE,&task);
	while(1) {
		/* choose a process to run */
		active = scheduler(procs, n, active);

		/* run the process for some time */
		bwputs("activate\n");
		procs[active] = activate(procs[active]);

		/* handle the interrupt */
		unsigned int val = procs[active][2+7]; /* the interrupt value */
		if(val == &yield) {
			bwputs("yield\n");
		} else if(val == &fork) {
			n = _fork(user_stack, procs, active, n);
			bwputs("fork\n");
		}
	}

	return 0;
}
