/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#include <command.h>
#include <herrno.h>
#include <io.h>
#include <string.h>

/* search for abbrev using pointer equivalence */
static char find_abbrev(const command_list_t *cmdlist,
			const command_t *cmd)
{
	const abbrev_t *abbr;
	int n;

	for (n=0; (abbr = cmdlist->abbrevs[n]); ++n) {
		if (abbr->cmd == cmd)
			return abbr->abbrev;
	}
	return 0;
}

int help_cmdfunc(int argc, char *argv[])
{
	const command_list_t *cmdlist = get_current_commands();
	const command_t *cmd;
	const char *name = 0;
	int found, i, spaces;
	char abbrev;

	if (argc > 2)
		return -H_EUSAGE;
	if (argc == 2)
		name = *++argv;

	for (found=0, i=0; (cmd = cmdlist->commands[i]); ++i) {
		if (name && strcmp(name, cmd->name))
			continue;
		found = 1;
		spaces = 30;
		if ((abbrev = find_abbrev(cmdlist, cmd)))
			spaces -= hprintf("%c ", abbrev);
		else
			spaces -= hprint("  ");
		spaces -= hprint(cmd->name);
		if (cmd->arghelp)
			spaces -= hprintf(" %s", cmd->arghelp);
		for (/* nothing */; spaces > 0; --spaces)
			hputchar(' ');
		hprintf(": %s\n", cmd->cmdhelp);
	}
	if (name && !found)
		return -H_ENOCMD;
	return 0;
}

const command_t help_command =
	{ "help", "[<command name>]",
	  "help on Hermit commands", &help_cmdfunc };

COMMAND_ABBR(help_command, '?');
