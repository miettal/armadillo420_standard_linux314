/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#include <buffer.h>
#include <crc.h>
#include <herrno.h>
#include <htypes.h>
#include <io.h>
#include <scan.h>
#include <command.h>
#include <string.h>

/*
 * The download command can be used in two ways: it can download
 * directly to RAM, or it can download to a buffer in the loader,
 * which can later be written to flash.  If the <addr> argument of the
 * download command is 'buf', the buffer is chosen.
 */
static int download_cmdfunc(int argc, char *argv[])
{
	unsigned char *dst;
	addr_t addr;
	size_t count, hostcrc, nread;

	/* crc is optional */
	if (argc < 3 || argc > 4)
		return -H_EUSAGE;

	/* special case download to buffer */
	++argv;
	if (!strcmp(*argv, "buf"))
		dst = dlbuf;
	else if (scan(*argv, &addr))
		return -H_EADDR;
	else
		dst = (unsigned char*) addr;

	/* buffer download is the only case which allows a size check */
	if (scan(*++argv, &count))
		return -H_EADDR;
	if (dst == dlbuf && count > dlbufsize)
		return -H_EOVERFLOW;

	if (argc == 4)
		if (scan(*++argv, &hostcrc))
			return -H_EINVAL;

	/* read block to destination */
	hprintf("+GO\n");
	nread = hgetblock(dst, count);
	if (nread > count)
		return -H_EOVERFLOW;
	if (nread < count)
		return -H_EUNDERFLOW;
	if (argc == 4) {
		uint32_t crc = crc32(dst, count);
		if (crc != (uint32_t) hostcrc)
			return -H_ECRC;
	}
	return 0;
}

const command_t download_command =
	{ "download", "<addr> <count> [<crc>]",
	  "download data to memory", &download_cmdfunc };

COMMAND(download_command);
