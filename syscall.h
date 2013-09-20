#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include "stddef.h" /* size_t */

unsigned int *activate(unsigned int *stackptr); /* enter into user mode (and execute the "first" program) */
void yield(void); /* cause a software interrupt */
int fork(void); /* fork the process */
int write(unsigned int pid,size_t size,char *msg); /* send a message to another process */
size_t read(size_t buf_size,char *buf); /* receive a message system message */
unsigned int getpid(); /* return the pid of the current process */

#endif
