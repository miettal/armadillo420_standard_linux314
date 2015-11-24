/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#include <command.h>
#include <command/frob.h>

static const command_t *frob_commands[] = {
	&help_command,
	&peek_command,
	&peek8_command,
	&peek16_command,
	&peeknext_command,
	&poke_command,
	&poke8_command,
	&poke16_command,
	&quit_command,
	0
};

ABBR(help_command, '?')
ABBR(peek_command, 'p')
ABBR(peek8_command, '8')
ABBR(peeknext_command, 'n')
ABBR(poke_command, 'k')
ABBR(quit_command, 'q')

static const abbrev_t *frob_abbrevs[] = {
	&help_command_abbrev,
	&peek_command_abbrev,
	&peek8_command_abbrev,
	&peeknext_command_abbrev,
	&poke_command_abbrev,
	&quit_command_abbrev,
	0
};

static command_list_t frob_command_list =
	{ frob_commands, frob_abbrevs, "frob", 0 };

static int frob_cmdfunc(int argc, char *argv[])
{
	do_command_loop(&frob_command_list);
	return 0;
}

const command_t frob_command =
	{ "frob", 0, "commands for frobbing memory", &frob_cmdfunc };

COMMAND_ABBR(frob_command, 'f');
