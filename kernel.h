#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "queue.h"

#define NUM_STACKS 16
#define STACK_SIZE 1024

struct Process {
	unsigned int stack[STACK_SIZE];
	unsigned int *stackptr;
	struct Queue msgs;
	struct Queue writers; /* PID of blocked writers */
	unsigned int blocked; /* updgrade this to status when there is more than 2 */
};

unsigned int _read(struct Process *);
unsigned int _write(struct Process *,struct Process *);

#endif 
