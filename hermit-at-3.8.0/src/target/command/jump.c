/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#include <command.h>
#include <herrno.h>
#include <scan.h>

/* this usually works OK */
#define JUMP(addr) { ((void(*)(void))(addr))(); }

static int jump_cmdfunc(int argc, char *argv[])
{
	addr_t addr;

	if (argc != 2)
		return -H_EUSAGE;
	if (scan(*++argv, &addr))
		return -H_EADDR;
	addr &= ~(addr_t)UNALIGNED_MASK;
	JUMP(addr);
	/* should never get here */
	return -H_EINVAL;
}

const command_t jump_command =
	{ "jump", "<addr>",
	  "jump to a specified address", &jump_cmdfunc };

COMMAND(jump_command);
