#ifndef _PIPE_H_
#define _PIPE_H_

/* a safe pipe implementation */

#include "stddef.h" /* size_t */

#define BUF_SIZE 4096 /* Size of largest atomic queue message */

struct Pipe {
	int start;
	int end;
	char data[BUF_SIZE+1];
};

/* unsafe */
void pipe_seek(struct Pipe *pipe, size_t num_bytes);
void pipe_push(struct Pipe *pipe, size_t num_bytes, char *bytes);
void pipe_pop(struct Pipe *pipe, size_t num_bytes, char *bytes);

/* safe */
void pipe_init(struct Pipe *pipe);
size_t pipe_len(struct Pipe *pipe);
int pipe_push_safe(struct Pipe *pipe, size_t num_bytes, char *bytes);
int pipe_push_message(struct Pipe *pipe, size_t num_bytes, char *bytes);
int pipe_pop_safe(struct Pipe *pipe, size_t num_bytes, char *bytes);
size_t pipe_pop_message(struct Pipe *pipe, size_t num_bytes, char *bytes);

#endif
