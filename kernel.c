#include "versatilepb.h"
#include "stddef.h"
#include "syscall.h"

#define NUM_STACKS 10
#define STACK_SIZE 1024
#define QUEUE_SIZE 256

struct Process {
	unsigned int stack[STACK_SIZE];
	unsigned int *stackptr;
	unsigned int msgs[QUEUE_SIZE];
	unsigned int mstart;
	unsigned int mend;
	unsigned int blocked; /* updgrade this to status when there is more than 2 */
};

void bwputs(char *s) {
	while(*s) {
		while(*(UART0 + UARTFR) & UARTFR_TXFF);
		*UART0 = *s;
		s++;
	}
}

void debug(unsigned int a, unsigned int b, unsigned int c) {
	(void) a, (void) b, (void) c;
	bwputs("debug halt\n");
	while(1);
}

int do_nothing_task(void) {
	while(1) {
		bwputs("do nothing\n");
	}
	return 0;
}

int fork_task(void) {
	int x;
	while(1) {
		bwputs("task\n");
		x = fork();
		if(x == 0) {
			while(1) bwputs("child\n");
		} else if(x > 0) {
			bwputs("parent\n");
		} else if(x < 0) {
			while(1) bwputs("fork error\n");
		}
	}
	return 0;
}

int first_task(void) {
	unsigned int buf;
	while(1) {
		bwputs("writing\n");
		write(0,4);
		bwputs("reading\n");
		read(&buf);
	}
}

/*
	initialize a process given its stack
	returns the pointer to the top of the stack (needed by activate)
*/
unsigned int *init_process(struct Process *proc, unsigned int size, int (*task)(void)) {
	/* this is necessary to run something in user mode */
	proc->blocked = 0;
	proc->qstart = 0;
	proc->qend = 0;
	proc->stack[size-16] = (unsigned int)task; /* (pc) program counter */
	proc->stack[size-15] = 0x10; /* (SPSR) saved state */
	return proc->stack + size - 16;
}

/* basic */
unsigned int scheduler(struct Process procs[], unsigned int num_procs, unsigned int active_proc) {
	(void) procs;
	if(num_procs-1 == active_proc) return 0;
	return active_proc + 1;
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

unsigned int _fork(struct Process procs[], unsigned int parent_pid, unsigned int next_pid) {
	unsigned int offset;
	bwputs("_fork\n");
	if(next_pid == NUM_STACKS) {
		procs[parent_pid].stackptr[2+0] = -1;
		return next_pid;
	}
	offset = procs[parent_pid].stackptr - procs[parent_pid].stack;
	procs[next_pid].stackptr = procs[next_pid].stack + offset;
	memcpy(procs[next_pid].stackptr, procs[parent_pid].stackptr, sizeof(*(procs[parent_pid].stack)) * (STACK_SIZE - offset));
	procs[parent_pid].stackptr[2+0] = next_pid;
	procs[next_pid].stackptr[2+0] = 0;
	return next_pid + 1;
}

int main(void) {
	struct Process procs[NUM_STACKS];
	unsigned int num_procs = 0; /* the next available stack. n should never be >= NUM_STACKS */
	unsigned int active_proc = 0;
	unsigned int ic; /* interrupt condition */

	/* initialize the hardware timer */
	*(PIC + VIC_INTENABLE) = PIC_TIMER01;
	*TIMER0 = 10000;
	*(TIMER0 + TIMER_CONTROL) = TIMER_EN | TIMER_32BIT | TIMER_PERIODIC | TIMER_INTEN;

	/* start running tasks */
	procs[num_procs].stackptr = init_process(&(procs[num_procs]),STACK_SIZE,&first_task);
	num_procs++;
	while(1) {
		/* choose a process to run */
		active_proc = scheduler(procs, num_procs, active_proc);

		/* run the process for some time */
		bwputs("activate\n");
		procs[active_proc].stackptr = activate(procs[active_proc].stackptr);

		/* handle the interrupt */
		ic = procs[active_proc].stackptr[2+7]; /* the interrupt value */
		if(ic == (unsigned int)PIC) {
			bwputs("hardware interrupt\n");
			if(*PIC & PIC_TIMER01) {
				if(*(TIMER0 + TIMER_MIS)) {
					bwputs("  timer0\n");
					*(TIMER0 + TIMER_INTCLR) = 1;
				}
			}
		} else if(ic == (unsigned int)&yield) {
			bwputs("yield\n");
		} else if(ic == (unsigned int)&fork) {
			num_procs = _fork(procs, active_proc, num_procs);
			bwputs("fork\n");
		} else if(ic == (unsigned int)&write) {
			bwputs("write\n");
		} else if(ic == (unsigned int)&read) {
			bwputs("read\n");
		} else {
			bwputs("UNKNOWN SYSCALL\n");
		}
	}
	return 0;
}
