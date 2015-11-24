#include <ctype.h>

unsigned long strtol(char *ptr, char **endp, int base)
{
	unsigned long result = 0;
	unsigned long value;

	if (base == 0) {
		if (ptr[0] == '0' && ptr[1] == 'x') {
			base = 16;
			ptr += 2;
		} else
			base = 10;
	}

	while (is_digit(*ptr)) {
		value = is_hex(*ptr) ? to_lower(*ptr) - 'a' + 10 : *ptr - '0';
		result = result * base + value;
		ptr++;
	}

	if (endp && *endp)
		*endp = ptr;

	return result;
}
