/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#include <command.h>
#include <herrno.h>
#include <io.h>
#include <string.h>

/* command line length limits */
#define MAXARGS CMDLINE_MAXARGS
#define MAXLEN CMDLINE_MAXLEN

/* character constants */
#define LF      '\n'
#define CR      '\r'
#define BS      0x08
#define DEL     0x7F
#define ISWSP(c) ((c)==' ' || (c)=='\t' || (c)=='\r' || (c)=='\n')

/* global data that's part of the API */
void (*errfunc)();
union errdata errdata;

/* static global data */
static command_list_t *top_command_list;
static int interactive = 1;

/* do_command scribbles on cmdline! */
static int do_command(const command_list_t *cmdlist, char *cmdline);
static int do_error(int n);
static void command_loop(const command_list_t *cmdlist);
static void print_prompt(const command_list_t *cmdlist);

/* read and execute commands until one returns -H_EQUITLOOP */
static void command_loop(const command_list_t *cmdlist)
{
	char cmdbuf [MAXLEN];
	int n;

	do {
		if (interactive)
			print_prompt(cmdlist);

		/* read command */
		for (n=0; n < (sizeof cmdbuf)-1; /* nada */) {
			char c = hgetchar();
			if (c == BS || c == DEL) {
				/* backspace and rubout */
				if (n > 0)
					n--;
				else
					continue;

				if (interactive) {
					hputchar(BS);
					hputchar(' ');
					hputchar(BS);
				}
			} else if (c == CR || c == LF) {
				/* end of command line */
				break;
			} else if (c == '!' || c == '~') {
				/* toggle interactive mode */
				interactive = (c == '!') ? 1 : 0;
				n = 0;
				break;
			} else if (c == 0x02) {
				/* test mode */
				if (interactive)
					hputchar(c);
				cmdbuf[n++] = c;
			} else if (c < ' ') {
				/* control character */
				if (interactive) {
					hputchar('^');
					hputchar(c + '@');
					cmdbuf[n++] = '^';
					cmdbuf[n++] = c + '@';
				}
			} else {
				/* ordinary character; echo. */
				if (interactive)
					hputchar(c);
				cmdbuf[n++] = c;
			}
		}
		cmdbuf[n] = '\0';
		if (interactive)
			hprint("\n");

		n = do_command(cmdlist, cmdbuf);
		if (!interactive && ((n == 0) || (n == -H_EQUITLOOP)))
			hprint("+OK\n");

	} while (n != -H_EQUITLOOP);
}

static int do_command(const command_list_t *cmdlist, char *cmdline)
{
	char *argv [MAXARGS];
	const command_t *cmd;
	const abbrev_t *abbr;
	char *p;
	int argc, n;

	/* parse command line; build argv (modifies cmdline) */
	p = cmdline;
	for (argc=0; argc < MAXARGS; /* nada */) {
		while (ISWSP(*p))
			++p;
		if (*p == '\0')
			break;
		argv[argc++] = p;
		while (*p && !ISWSP(*p))
			++p;
		if (*p == '\0')
			break;
		*p++ = '\0';
	}
	if (!argc)
		return 0;

	/* find command; test abbrevs first */
	cmd = 0;
	if (strlen(*argv) == 1) {
		for (n=0; (abbr = cmdlist->abbrevs[n]); ++n) {
			if (abbr->abbrev == (*argv)[0]) {
				cmd = abbr->cmd;
				*argv = (char*) cmd->name;
				break;
			}
		}
	}
	if (!cmd) {
		for (n=0; (cmd = cmdlist->commands[n]); ++n) {
			if (!strcmp(cmd->name, *argv))
				break;
		}
	}
	if (cmd) {
		/* invoke command */
		errfunc = 0;
		errdata.msg = 0;
		n = (cmd->func)(argc, argv);
	} else
		n = -H_ENOCMD;

	if (!n)
		return 0;
	if (n == -H_EQUITLOOP)
		return n;
	if (n == -H_EUSAGE) {
		hprintf("-NG usage: %s %s\n", cmd->name,
			cmd->arghelp ? cmd->arghelp : "");
		return n;
	}

	return do_error(n);
}

static int do_error(int n)
{
	/* others are all simple or externally defined error messages */
	switch ((unsigned char) -n) {
	case H_EADDR: errdata.msg = "invalid address"; break;
	case H_EALIGN: errdata.msg = "alignment failure"; break;
	case H_ECRC: errdata.msg = "CRC mismatch"; break;
	case H_EINVAL: errdata.msg = "invalid argument"; break;
	case H_EIO: errdata.msg = "I/O error"; break;
	case H_ENOCMD: errdata.msg = "command not found"; break;
	case H_EOVERFLOW: errdata.msg = "overflow"; break;
	case H_EUNDERFLOW: errdata.msg = "underflow"; break;
	case H_ENOSYS: errdata.msg = "Function not implemented"; break;
	case H_ETIMEDOUT: errdata.msg = "Timedout"; break;
	case H_EAGAIN: errdata.msg = "Try again"; break;
	default:
		if (errfunc) {
			hprint("-NG ");
			(*errfunc)();
			hprint("\n");
			return n;
		}
		break;
	}
	hprintf("-NG %s\n", errdata.msg);
	return n;
}

int do_command_loop(command_list_t *cmdlist)
{
	cmdlist->prev = top_command_list;
	top_command_list = cmdlist;
	command_loop(cmdlist);
	top_command_list = cmdlist->prev;
	return 0;
}

const command_list_t *get_current_commands(void)
{
	return top_command_list;
}

static void print_prompt_helper(const command_list_t *cmdlist)
{
	if (cmdlist->prev) {
		print_prompt_helper(cmdlist->prev);
		hprintf(":%s", cmdlist->prompt);
	} else
		hprint(cmdlist->prompt);
}

static void print_prompt(const command_list_t *cmdlist)
{
	print_prompt_helper(cmdlist);
	hprint("> ");
}
