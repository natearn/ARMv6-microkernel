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

void pipe_seek(struct Pipe *pipe, size_t num_bytes) {
	size_t i;
	char c = '\0';
	for(i=0; i < num_bytes; i++) {
		RB_POP(*pipe,BUF_SIZE+1,c);
	}
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

int pipe_push_message(struct Pipe *pipe, size_t num_bytes, char *bytes) {
	if(pipe_len(pipe) + num_bytes + sizeof(num_bytes) > BUF_SIZE) return 1;
	pipe_push(pipe, sizeof(num_bytes), (char*)&num_bytes);
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

/*
	Attempt to read a message into the buffer.
	If there is no message to read, returns 0.
	Otherwise the original size of the message will be returned.
	If the message is too big for the buffer, the message is truncated.
*/
size_t pipe_pop_message(struct Pipe *pipe, size_t buf_size, char *buf) {
	size_t msg_size;
	if(pipe_len(pipe) < sizeof(msg_size)) return 0; /* no message */
	pipe_pop(pipe, sizeof(msg_size), (char*)&msg_size);
	if(buf_size < msg_size) {
		pipe_pop(pipe, buf_size, buf);
		pipe_seek(pipe,msg_size-buf_size); /* throw away the rest of the message */
	} else {
		pipe_pop(pipe, msg_size, buf);
	}
	return msg_size;
}
