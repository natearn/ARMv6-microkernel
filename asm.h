#ifndef _ASM_H_
#define _ASM_H_

unsigned int *activate(unsigned int *); /* enter into user mode (and execute the "first" program) */
void yield(void); /* cause a software interrupt */

#endif
