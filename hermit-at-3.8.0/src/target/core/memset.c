#include <htypes.h>

void *memset(void *__src, int __c, size_t __n)
{
	int i;
	unsigned char *s = __src;
	unsigned char c = (unsigned char)__c;

	for(i = (__n >> 3); i > 0; i--){
		*s++ = c;
		*s++ = c;
		*s++ = c;
		*s++ = c;
		*s++ = c;
		*s++ = c;
		*s++ = c;
		*s++ = c;
	}

	if(__n & (1 << 2)){
		*s++ = c;
		*s++ = c;
		*s++ = c;
		*s++ = c;
	}

	if(__n & (1 << 1)){
		*s++ = c;
		*s++ = c;
	}

	if(__n & 1)
		*s++ = c;

	return __src;
}
