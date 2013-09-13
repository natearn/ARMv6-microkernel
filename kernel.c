#include "versatilepb.h"
#include "stddef.h"
#include "kernel.h"
#include "pipe.h"
#include "syscall.h"

void bwputs(char *s) {
	while(*s) {
		while(*(UART0 + UARTFR) & UARTFR_TXFF);
		*UART0 = *s;
		s++;
	}
}

char digit(unsigned int n) {
	unsigned int x = n / 10 * 10;
	unsigned int d = n - x;
	switch(d) {
		case 0: return '0';
		case 1: return '1';
		case 2: return '2';
		case 3: return '3';
		case 4: return '4';
		case 5: return '5';
		case 6: return '6';
		case 7: return '7';
		case 8: return '8';
		case 9: return '9';
	}
	return '?';
}

void nputs(unsigned int n) {
	unsigned int x;
	char c[2] = {'\0','\0'};
	do {
		x = n / 10 * 10;
		c[0] = digit(n-x);
		bwputs(c);
		n = x / 10;
	} while(x > 0);
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

int yield_task(void) {
	while(1) {
		bwputs("yield\n");
		yield();
	}
	return 0;
}

int first_task(void) {
	int x;
	char buf[20] = {'h','e','l','l','o',' ','w','o','r','l','d'};
	unsigned int val;
	size_t amount;
	x = fork();
	if(x == 0) {
		while(1) {
			bwputs("reading\n");
			amount = read(sizeof(val),(char*)&val);
			bwputs("I received a ");
			nputs(amount);
			bwputs("-byte message!\n");

			bwputs("I expected a ");
			nputs(sizeof(val));
			bwputs("-byte message!\n");

			bwputs("The value of my message is: ");
			nputs(val);
			bwputs("\n");
		}
	} else if(x > 0) {
		while(1) {
			bwputs("writing\n");
			write(x,5,buf);
		}
	} else {
		bwputs("fork error\n");
		while(1);
	}
}

/*
	initialize a process given its stack
	returns the pointer to the top of the stack (needed by activate)
*/
unsigned int *init_process(struct Process *proc, unsigned int size, int (*task)(void)) {
	/* this is necessary to run something in user mode */
	pipe_init(&(proc->msgs));
	QUEUE_INIT(proc->writers);
	proc->blocked = 0;
	proc->stack[size-16] = (unsigned int)task; /* (pc) program counter */
	proc->stack[size-15] = 0x10; /* (SPSR) saved state */
	return proc->stack + size - 16;
}

/* basic */
unsigned int nextpid(unsigned int pid, unsigned int num_pids) {
	if(pid == num_pids - 1) return 0;
	return pid + 1;
}
unsigned int scheduler(struct Process procs[], unsigned int num_procs, unsigned int active_proc) {
	unsigned int pid;
	for(pid = nextpid(active_proc,num_procs); procs[pid].blocked; pid = nextpid(pid,num_procs)) {
		/* assume that pid is blocked */
		if(pid == active_proc) {
			bwputs("DEADLOCK\n");
			debug(pid,0,0);
		}
	}
	return pid;
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

unsigned int _read(struct Process *proc) {
	struct Process *wakeproc;
	size_t buf_size = (size_t)proc->stackptr[2+0];
	size_t msg_size;
	char *buf = (char*)proc->stackptr[2+1];
bwputs("-> _read(");
	msg_size = pipe_pop_message(&(proc->msgs),buf_size,buf);
	if(msg_size == 0) {
nputs(pipe_len(&(proc->msgs)));
bwputs(",block) ");
		/* not enough data to complete the read */
		proc->blocked = (unsigned int)&_read;
	} else {
		proc->stackptr[2+0] = msg_size; /* return number of bytes in the message */
		/* unblock writers */
		if(QUEUE_LEN(proc->writers) > 0) {
bwputs("unblock) ");
			QUEUE_POP(proc->writers,wakeproc);
			wakeproc->blocked = 0;
			_write(wakeproc,proc);
		} else {
bwputs("success) ");
		}
	}
	return proc->blocked;
}

unsigned int _write(struct Process *sender, struct Process *receiver) {
	int bytes = (int)sender->stackptr[2+1];
	char *buf = (char*)sender->stackptr[2+2];
bwputs("-> _write(");
	if(pipe_push_message(&(receiver->msgs),bytes,buf)) {
nputs(pipe_len(&(receiver->msgs)));
bwputs(",block) ");
		/* the queue is full */
		sender->blocked = (unsigned int)&_write;
		QUEUE_PUSH(receiver->writers,sender);
	} else {
		sender->stackptr[2+0] = 0; /* not using return value yet */
		/* unblock reader */
		if(receiver->blocked == (unsigned int)&_read) {
bwputs("unblock) ");
			receiver->blocked = 0;
			_read(receiver);
		} else {
bwputs("success) ");
		}
	}
	return sender->blocked;
}

int main(void) {
	struct Process procs[NUM_STACKS];
	unsigned int num_procs = 0; /* the next available stack. n should never be >= NUM_STACKS */
	unsigned int active_proc = 0;
	unsigned int val; /* interrupt condition */

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
		val = procs[active_proc].stackptr[2+7]; /* the interrupt value */
		if(val == (unsigned int)PIC) {
			bwputs("hardware interrupt\n");
			if(*PIC & PIC_TIMER01) {
				if(*(TIMER0 + TIMER_MIS)) {
					bwputs("  timer0\n");
					*(TIMER0 + TIMER_INTCLR) = 1;
				}
			}
		} else if(val == (unsigned int)&yield) {
			bwputs("yield\n");
		} else if(val == (unsigned int)&fork) {
			num_procs = _fork(procs, active_proc, num_procs);
			bwputs("fork\n");
		} else if(val == (unsigned int)&write) {
			bwputs("write ");
			val = procs[active_proc].stackptr[2+0]; /* pid of receiver */
			if(_write(&(procs[active_proc]),&(procs[val]))) {
				bwputs("-> blocked\n");
			} else {
				bwputs("-> success\n");
			}
		} else if(val == (unsigned int)&read) {
			bwputs("read ");
			if(_read(&(procs[active_proc]))) {
				bwputs("-> blocked\n");
			} else {
				bwputs("-> success\n");
			}
		} else {
			bwputs("UNKNOWN SYSCALL\n");
		}
	}
	return 0;
}
