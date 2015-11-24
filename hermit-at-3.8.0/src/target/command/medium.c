/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#include <assert.h>
#include <crc.h>
#include <command.h>
#include <medium.h>
#include <herrno.h>
#include <string.h>
#include <io.h>

medium_t current_medium;

int hgetchar_timedout(unsigned long timeout)
{
	return arch_getchar(timeout);
}

int hputchar_timedout(int c, unsigned long timeout)
{
	return arch_putchar(c, timeout);
}

int hgetblock(unsigned char *dst, int count)
{
	int remain = count;
	if (current_medium == MEDIUM_SERIAL) {
		while (remain--)
			*dst++ = hgetchar();
	}
#ifdef ENABLE_MEDIUM_ETHERNET
	else {
		assert(current_medium == MEDIUM_ETHERNET);
		while (remain > 0) {
			int nread = ether_read(dst, remain);
			assert(nread <= remain);
			dst += nread;
			remain -= nread;
		}
	}
#endif
	return count;
}

/*
 * This works for now, since medium only affects downstream data, not
 * replies from the target to the host.  Once we add upstream media
 * switching, we'll have to implement a delayed media switch that
 * allows the successful command return value to go out along the old
 * medium.
 */
static int medium_cmdfunc(int argc, char *argv[])
{
	medium_t medium;

	if (argc != 2)
		return -H_EUSAGE;
	++argv;
	if (!strcmp(*argv, "serial")) {
		medium = MEDIUM_SERIAL;
	}
#ifdef ENABLE_MEDIUM_ETHERNET
	else if (!strcmp(*argv, "ethernet")) {
		medium = MEDIUM_ETHERNET;
	}
#endif
	else {
		return -H_EINVAL;
	}

	if (medium == current_medium)
		return 0;

	if (medium == MEDIUM_SERIAL) {
	}
#ifdef ENABLE_MEDIUM_ETHERNET
	else if (medium == MEDIUM_ETHERNET) {
		return -H_EIO;
	}
#endif
	current_medium = medium;
	return 0;
}

const command_t medium_command =
	{ "medium", "<medium name>",
	  "select communication medium",
	  &medium_cmdfunc };

COMMAND(medium_command);
