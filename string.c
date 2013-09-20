#include "stddef.h" /* size_t */

int strcmp(const char *a, const char *b) {
	size_t i = 0;
	while(a[i] != '\0' && a[i] == b[i]) i++;
	return a[i] - b[i];
}

size_t strlcpy(char *dest, const char *src, size_t size) {
	size_t i=0;
	while(src[i] != '\0') {
		if(i < size) dest[i] = src[i];
		i++;
	}
	dest[i < size ? i : size-1] = '\0';
	return i;
}
