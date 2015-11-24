/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#ifndef _HERMIT_TARGET_COMMAND_H_
#define _HERMIT_TARGET_COMMAND_H_

#include <hermit.h>
#include <htypes.h>

#define CMDLINE_MAXARGS 16
#define CMDLINE_MAXLEN 512

typedef struct command command_t;
struct command {
	const char *name;
	const char *arghelp;
	const char *cmdhelp;
	int (*func)(int argc, char *argv[]);
};

typedef struct abbrev abbrev_t;
struct abbrev {
	const command_t *cmd;
	char abbrev;
};

typedef struct command_list command_list_t;
struct command_list {
	const command_t *const *commands;
	const abbrev_t *const *abbrevs;
	const char *prompt;
	command_list_t *prev;
};

/* keep command abbreviation declarations from getting messy */
#define ABBR(cmd,ab) \
        static const abbrev_t cmd##_abbrev = { &cmd, ab };

#define COMMAND(command) \
	static const command_t *__command_##command \
	__used __section(".command.list") = &command

#define COMMAND_ABBR(command, key) \
	static const command_t *__command_##command \
	__used __section(".command.list") = &command;\
	ABBR(command, key);\
	static const abbrev_t *__abbrev_##command \
	__used __section(".command.abbrev") = &command##_abbrev

extern int do_command_loop(command_list_t *cmdlist);

extern const command_list_t *get_current_commands(void);

/*
 * Error handling.  We would just use an error string, but sometimes
 * you don't want to allocate such buffers in really tight memory
 * situations, so we provide an option; if 'errfunc' is set, call it
 * instead of just printing errmsg.
 */
extern void (*errfunc)(void);
extern union errdata {
	const char *msg;
	word_t word;
} errdata;

#if 0 /* == command template == */
#include <command.h>
static int template_cmdfunc(int argc, char *argv[])
{
        return 0;
}
const command_t template_command = {
	.name = "template",
	.arghelp = "",
	.cmdhelp = "",
	.func = template_cmdfunc
};
COMMAND_ABBR(template_command, 't');
#endif /* == command template == */

#endif /* _HERMIT_TARGET_COMMAND_H_ */
