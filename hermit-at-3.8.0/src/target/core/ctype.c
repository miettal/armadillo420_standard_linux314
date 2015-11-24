#include <ctype.h>

static int __ctype(char c)
{
	if ('0' <= c && c <= '9')
		return CTYPE_DIGIT;
	if ('a' <= c && c <= 'f')
		return CTYPE_DIGIT | CTYPE_HEX | CTYPE_LOWER;
	if ('A' <= c && c <= 'F')
		return CTYPE_DIGIT | CTYPE_HEX | CTYPE_UPPER;
	return 0;
}

int is_digit(char c)
{
	return __ctype(c) & CTYPE_DIGIT;
}

int is_decimal(char c)
{
	return __ctype(c) == CTYPE_DIGIT;
}

int is_hex(char c)
{
	return __ctype(c) & CTYPE_HEX;
}

int is_lower(char c)
{
	return __ctype(c) & CTYPE_LOWER;
}

int is_upper(char c)
{
	return __ctype(c) & CTYPE_UPPER;
}

unsigned char to_lower(unsigned char c)
{
	if (is_upper(c))
		c -= 'A' - 'a';
	return c;
}
