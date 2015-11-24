/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#include <herrno.h>
#include <htypes.h>
#include <io.h>

#define va_list __builtin_va_list
#define va_start(v,l) __builtin_va_start(v,l)
#define va_arg(v,l) __builtin_va_arg(v,l)
#define va_end(v) __builtin_va_end(v)

static void __putchar(char *buf, char **rbuf, int c)
{
	if (buf) {
		*buf = (char)c;
		if (*rbuf)
			*rbuf = buf + 1;
	} else
		hputchar(c);
}

static int __dprint(char *buf, char **rbuf, int dec)
{

	int order = 1000000000;	/* for 32 bits */
	int count = 0, started;

	if (dec < 0) {
		++count; __putchar(buf, &buf, '-');
		dec = -dec;
	}

	for (started = 0; order >= 10; order /= 10) {
		if (dec >= order) {
			int digit = dec / order;
			++count; __putchar(buf, &buf, digit + '0');
			dec -= digit * order;
			started = 1;
		} else if (started) {
			++count; __putchar(buf, &buf, '0');
		}
	}
	++count; __putchar(buf, &buf, dec + '0');
	if (buf && *rbuf)
		*rbuf = buf;
	return count;
}

static void __xputchar(char *buf, char **rbuf, char c)
{
	__putchar(buf, &buf, (c<10) ? (c+'0') : (c-10+'a'));
	if (buf && *rbuf)
		*rbuf = buf;
}

static int __xprint(char *buf, char **rbuf, word_t hex, int nbytes)
{
	int shift;
	for (shift = nbytes * 8 - 4; shift >= 0; shift -= 4)
		__xputchar(buf, &buf, (hex >> shift) & 0xF);
	if (buf && *rbuf)
		*rbuf = buf;
	return nbytes * 2;
}

static int __strprint(char *buf, char **rbuf, const char *s)
{
	int count;
	for (count=0; *s; ++count) {
		if (*s == '\n')
			++count, __putchar(buf, &buf, '\r');
		__putchar(buf, &buf, *s++);
	}
	if (buf && *rbuf)
		*rbuf = buf;
	return count;
}

inline int hprint(const char *s)
{
	return __strprint(NULL, NULL, s);
}

static int __snprintf(char *buf, size_t size, const char *format,
		       va_list args)
{
	int count = 0;
	enum state { NORMAL, LITERAL, FORMAT } state = NORMAL;
	int long_mult = 0;

	for (; *format; ++format) {
		switch (state) {

		case NORMAL:
			if (*format == '\n') {
				/* translate CR to CRLF */
				__putchar(buf, &buf, '\r');
				__putchar(buf, &buf, '\n');
				count += 2;
			} else if (*format == '\\') {
				state = LITERAL;
			} else if (*format == '%') {
				state = FORMAT;
				long_mult = 0;
			} else
				++count, __putchar(buf, &buf, *format);
			break;

		case LITERAL:
			++count, __putchar(buf, &buf, *format);
			state = NORMAL;
			break;

		case FORMAT:
			switch (*format) {
			case 'b':
				count += __xprint(buf, &buf,
						  va_arg(args, word_t), 1);
				break;
			case 'c':
				++count, __putchar(buf, &buf,
						   va_arg(args, int));
				break;
			case 'd':
				switch (long_mult) {
				case 0:
				case 1:
					count += __dprint(buf, &buf,
							  va_arg(args, int));
					break;
				case 2:
					count += __dprint(buf, &buf,
							  va_arg(args, s64));
					break;
				}
				break;
			case 'h':
				count += __xprint(buf, &buf,
						  va_arg(args, word_t), 2);
				break;
			case 'l':
				long_mult++;
				continue;
			case 'p':
				count += __strprint(buf, &buf, "0x");
				long_mult = 0;
				/* FALL THROUGH */
			case 'x':
				switch (long_mult) {
				case 0:
				case 1:
					count += __xprint(buf, &buf,
							  va_arg(args, word_t),
							  sizeof(word_t));
					break;
				case 2:
					count += __xprint(buf, &buf,
							  va_arg(args, u64),
							  sizeof(u64));
					break;
				}
				break;
			case 's':
				count += __strprint(buf, &buf,
						  va_arg(args, char*));
				break;
			case '%':
				++count, __putchar(buf, &buf, '%');
				break;
			default:
				break;
			}
			state = NORMAL;
			break;
		}
	}

	if (buf)
		*buf = '\0';

	return count;
}

int hprintf(const char *format, ...)
{
	va_list args;
	int i;

	va_start(args, format);
	i = __snprintf(NULL, ~0, format, args);
	va_end(args);
	return i;
}

int hsprintf(char *buf, const char *format, ...)
{
	va_list args;
	int i;

	va_start(args, format);
	i = __snprintf(buf, ~0, format, args);
	va_end(args);
	return i;
}
