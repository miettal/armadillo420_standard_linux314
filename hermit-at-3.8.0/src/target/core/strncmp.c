#include <htypes.h>

int strncmp(const char *s1, const char *s2, size_t n)
{
	for (--n; *s1 == *s2; ++s1, ++s2, --n) {
		if (!n || !*s1)
			return 0;
	}
	return ((int) *s1) - ((int) *s2);
}
