/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */
#include <herrno.h>
#include <io.h>
#include <scan.h>
#include <command.h>

static int upload_cmdfunc(int argc, char *argv[])
{
	addr_t addr;
	size_t size;
	const unsigned char *p;

	if (argc != 3)
		return -H_EUSAGE;
	if (scan(*++argv, &addr)) return -H_EADDR;
	if (scan(*++argv, &size)) return -H_EADDR;
	p = (const unsigned char*) addr;
	while (size--)
		hputchar(*p++);
	return 0;
}

const command_t upload_command =
	{ "upload", "<addr> <size>",
	  "upload data from memory",
	  &upload_cmdfunc };

COMMAND(upload_command);
