#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#define PAYLOAD unsigned int *

unsigned int *activate(unsigned int *); /* enter into user mode (and execute the "first" program) */
void yield(void); /* cause a software interrupt */
int fork(void); /* fork the process */
int write(unsigned int,unsigned int); /* send a message to another process */
int read(unsigned int *); /* receive a message system message */

#endif
