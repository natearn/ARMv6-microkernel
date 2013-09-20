#ifndef _STRING_H_
#define _STRING_H_

#include "stddef.h" /* size_t */

int strcmp(const char *a, const char *b);
size_t strlcpy(char *dest, const char *src, size_t size);

#endif
