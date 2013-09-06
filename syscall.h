#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include "stddef.h" /* size_t */

unsigned int *activate(unsigned int *stackptr); /* enter into user mode (and execute the "first" program) */
void yield(void); /* cause a software interrupt */
int fork(void); /* fork the process */
int write(unsigned int pid,size_t size,void *msg); /* send a message to another process */
int read(size_t size,unsigned int *msg); /* receive a message system message */

#endif
