/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#include <io.h>
#include <scan.h>
#include <string.h>

unsigned char hex2char(unsigned char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return HEX_ERROR;
}

int scan_dec(const char *str, word_t *value)
{
	for (*value = 0; *str; ++str) {
		*value *= 10;
		if (*str < '0' || *str > '9')
			return -1;	/* error */
		*value += *str - '0';
	}
	return 0;
}

int scan_hex(const char *str, word_t *value)
{
	for (*value = 0; *str && (*str != 'h') && (*str != 'H'); ++str) {
		unsigned char c = hex2char(*str);
		if (c == HEX_ERROR)
			return -1;	/* error */
		*value <<= 4;
		*value += c;
	}
	return 0;
}

int scan(const char *str, word_t *value)
{
	int len;

	len = strlen(str);
	if (!len)
		return -1;
	if (len > 1) {
		if (str[len-1] == 'h' || str[len-1] == 'H')
			return scan_hex(str, value);
		if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
			return scan_hex(str+2, value);
	}
	return scan_dec(str, value);
}
