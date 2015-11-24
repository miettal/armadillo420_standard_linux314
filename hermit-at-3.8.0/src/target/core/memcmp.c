#include <htypes.h>

int memcmp(const void *__s1, const void *__s2, size_t __n)
{
	const unsigned char *su1, *su2;
	int res = 0;

	for( su1 = __s1, su2 = __s2; 0 < __n; ++su1, ++su2, __n--)
		if ((res = *su1 - *su2) != 0)
			break;
	return res;
}
