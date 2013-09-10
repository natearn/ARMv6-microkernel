#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "ringbuffer.h"
#include "pipe.h"
#include "stddef.h" /* size_t */

#define NUM_STACKS 16
#define STACK_SIZE 1024

struct ProcessQueue {
	int start;
	int end;
	struct Process *data[NUM_STACKS+1];
};

#define QUEUE_PUSH(queue, v) RB_PUSH((queue), NUM_STACKS+1, (v))
#define QUEUE_POP(queue, v)  RB_POP((queue), NUM_STACKS+1, (v))
#define QUEUE_LEN(queue)     (RB_LEN((queue), NUM_STACKS+1))
#define QUEUE_INIT(queue)    ((queue).start = (queue).end = 0)

struct Process {
	unsigned int stack[STACK_SIZE];
	unsigned int *stackptr;
	struct Pipe msgs;
	struct ProcessQueue writers; /* PID of blocked writers */
	unsigned int blocked; /* updgrade this to status when there is more than 2 */
};

unsigned int _read(struct Process *);
unsigned int _write(struct Process *,struct Process *);

#endif 
