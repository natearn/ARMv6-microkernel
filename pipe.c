#include "pipe.h"
#include "ringbuffer.h"

/* a safe pipe implementation */

#include "stddef.h" /* size_t */

void pipe_init(struct Pipe *pipe) {
	pipe->start = pipe->end = 0;
}

size_t pipe_len(struct Pipe *pipe) {
	return (size_t)RB_LEN(*pipe,BUF_SIZE+1);
}

void pipe_push(struct Pipe *pipe, size_t num_bytes, char *bytes) {
	size_t i;
	for(i=0; i < num_bytes; i++) {
		RB_PUSH(*pipe,BUF_SIZE+1,bytes[i]);
	}
}

int pipe_push_safe(struct Pipe *pipe, size_t num_bytes, char *bytes) {
	if(pipe_len(pipe) + num_bytes > BUF_SIZE) return 1;
	pipe_push(pipe, num_bytes, bytes);
	return 0;
}

void pipe_pop(struct Pipe *pipe, size_t num_bytes, char *bytes) {
	size_t i;
	for(i=0; i < num_bytes; i++) {
		RB_POP(*pipe,BUF_SIZE+1,bytes[i]);
	}
}

int pipe_pop_safe(struct Pipe *pipe, size_t num_bytes, char *bytes) {
	if(pipe_len(pipe) < num_bytes) return 1;
	pipe_pop(pipe, num_bytes, bytes);
	return 0;
}
