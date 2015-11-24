#include <htypes.h>

void *memcpy(void *__dest, const void *__src, size_t __n)
{
	int i = 0;
	unsigned char *d = __dest;
	unsigned const char *s = __src;

	for (i = (__n >> 3); i > 0; i--) {
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
	}

	if (__n & (1 << 2)) {
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
	}

	if (__n & (1 << 1)) {
		*d++ = *s++;
		*d++ = *s++;
	}

	if (__n & 1)
		*d++ = *s++;

	return __dest;
}
