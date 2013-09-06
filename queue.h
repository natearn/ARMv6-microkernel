#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "kernel.h"

#define BUF_SIZE 4096 /* Size of largest atomic queue message */

/* make structs for message types */

struct Pipe {
	int start;
	int end;
	char data[BUF_SIZE+1];
};

#define RB_PUSH(rb, size, v) do { \
		(rb).data[(rb).end] = (v); \
		(rb).end++; \
		if((rb).end >= size) (rb).end = 0; \
	} while(0)

#define RB_POP(rb, size, v) do { \
		(v) = (rb).data[(rb).start]; \
		(rb).start++; \
		if((rb).start >= size) (rb).start = 0; \
	} while(0)

#define RB_LEN(rb, size) (((rb).end - (rb).start) + \
	(((rb).end < (rb).start) ? size : 0))

#define PIPE_PUSH(pipe, v) RB_PUSH((pipe), BUF_SIZE+1, (v))
#define PIPE_POP(pipe, v)  RB_POP((pipe), BUF_SIZE+1, (v))
#define PIPE_LEN(pipe)     (RB_LEN((pipe), BUF_SIZE+1))
#define PIPE_INIT(pipe)    ((pipe).start = (pipe).end = 0)

#endif
