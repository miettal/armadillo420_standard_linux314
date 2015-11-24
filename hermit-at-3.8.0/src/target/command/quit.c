/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#include <command.h>
#include <herrno.h>

static int quit_cmdfunc(int argc, char *argv[])
{
	return -H_EQUITLOOP;
}

const command_t quit_command =
	{ "quit", 0, "quit current command loop", &quit_cmdfunc };
