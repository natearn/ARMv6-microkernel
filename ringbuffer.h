#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_

/* generic ring buffer operations */

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

#endif
