#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "kernel.h"

#define QUEUE_SIZE 512 /* Size of largest atomic queue message */

/* make structs for message types */

struct Queue {
	int start;
	int end;
	char data[QUEUE_SIZE];
};

void init_queue(struct Queue *q) {
	q->start = 0;
	q->end = 0;
}

#define RB_PUSH(rb, size, v) do { \
		(rb).data[(rb).end] = (v); \
		(rb).end++; \
		if((rb).end > size) (rb).end = 0; \
	} while(0)

#define RB_POP(rb, size, v) do { \
		(v) = (rb).data[(rb).start]; \
		(rb).start++; \
		if((rb).start > size) (rb).start = 0; \
	} while(0)

#define RB_LEN(rb, size) (((rb).end - (rb).start) + \
	(((rb).end < (rb).start) ? size : 0))

#define QUEUE_PUSH(queue, v) RB_PUSH((queue), QUEUE_SIZE, (v))
#define QUEUE_POP(queue, v)  RB_POP((queue), QUEUE_SIZE, (v))
#define QUEUE_LEN(queue)     (RB_LEN((queue), QUEUE_SIZE))

#endif
