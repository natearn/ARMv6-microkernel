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

int task1(void) {
	while(1) {
		bwputs("task1\n");
		fork();
		/* yield(); */
	}
	return 0;
}

int task2(void) {
	while(1) {
		bwputs("task2\n");
		yield();
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

unsigned int reg(unsigned int num, unsigned int stack[]) {
	return stack[2 + num];
}

unsigned int scheduler(unsigned int *procs[], unsigned int size, unsigned int active) {
	unsigned int i;
	if(procs[size-1] == procs[active]) return 0;
	for(i = 0; i < size-1; i++) {
		if(procs[i] == procs[active]) return i+1;
	}
	bwputs("scheduler error\n");
	while(1);
	return 0;
}

void *memcpy(void *src, size_t size, void* dest) {
	char *to = dest;
	const char *from = src;
	size_t i;
	for(i = 0; i < size; i++) {
		to[i] = from[i];
	}
	return dest;
}

unsigned int _fork(unsigned int *stacks[], unsigned int *procs[], unsigned int parent, unsigned int next_pid) {
	bwputs("_fork\n");
	if(next_pid == NUM_STACKS) {
		procs[parent][2+0] = -1;
		return next_pid;
	}
	/* THIS IS WRONG IN MORE THAN ONE WAY */
	procs[next_pid] = memcpy(stacks[parent], stacks[parent] - procs[parent], stacks[next_pid]);
	procs[parent][2+0] = next_pid;
	procs[next_pid][2+0] = 0;
	return next_pid++;
}

int main(void) {
	unsigned int user_stack[NUM_STACKS][STACK_SIZE];
	unsigned int n = 0; /* the next available stack. n should never be >= NUM_STACKS */
	unsigned int *procs[NUM_STACKS];
	unsigned int active = 0;

	procs[n++] = init_process(user_stack[n],STACK_SIZE,&task1);
	procs[n++] = init_process(user_stack[n],STACK_SIZE,&task2);
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
