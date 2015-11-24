/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(WIN32)
#include <termios.h>
#include <getopt.h>
#include <unistd.h>
#include "ethutil.h"
#endif


#include "options.h"
#include "util.h"

#ifdef WIN32
void messagebox (const char *str);
void consoleout (const char *str);
#undef consoleout
#define consoleout(str)
#endif

int opt_anon_regions;
int opt_ethernet;
int opt_force_locked;
int opt_terminal;
int opt_verbose;

const char *opt_memory_map_path;
const char *opt_netif = "eth0";
const char *opt_port = "/dev/ttyS0";
const char *opt_remote_mac;
unsigned char opt_remote_mac_bytes [6];

typedef struct command_name command_name_t;
struct command_name {
	const char *name;
	command_type_t command;
};

static command_name_t commands[] =
{
	{ "download", CMD_DOWNLOAD },
	{ "erase", CMD_ERASE },
	{ "help", CMD_HELP },
	{ "map", CMD_MAP },
	{ "terminal", CMD_TERMINAL },
	{ "upload", CMD_UPLOAD },
#ifdef AJ_FIRMUPDATE_SUPPORT
        { "firmupdate", CMD_FIRMUPDATE },
#endif
	{ "md5sum", CMD_MD5SUM},
	{ NULL, 0 },
};


static struct baudrate baudrate_table[] = {
	{ B9600, },
	{ B19200, },
	{ B38400, },
	{ B57600, },
	{ B115200, },
	{ B230400, },
	//{ ,,/* empty */ },
};

struct baudrate *opt_baudrate = &baudrate_table[4]; /* 115200 bps */


#ifndef WIN32
/*
 * The hyphen at the beginning of the short options list causes
 * getopt_long() to treat non-options (command line arguments not
 * beginning with a hyphen) as options with value 1 and with the
 * command line argument in optarg.  This makes it easy to handle
 * intermingled commands and arguments in an order-sensitive manner.
 */
const char short_options[] = "-a:b:ei:o:nr:vVs:";
static struct option long_options[] =
{
	{ "address",		1, 0,			'a' },
	{ "size",		1, 0,			's' },
	{ "anonymous-regions",	0, &opt_anon_regions,	1 },
	{ "baudrate",		1, 0,			'b'},
	{ "ethernet",		0, 0,			'e' },
	{ "force-locked",	0, &opt_force_locked,	1 },
	{ "input-file",		1, 0,			'i' },
	{ "output-file",	1, 0,			'o' },
	{ "memory-map", 	1, 0,			0 },
	{ "netif",		1, 0,			'N' },
	{ "port",		1, 0,			'P' },
	{ "region",		1, 0,			'r' },
	{ "remote-mac",		1, 0,			0 },
	{ "verbose",		0, 0,			'v' },
	{ "version",		0, 0,			'V' },
	{0, 0, 0, 0}
};
#endif

static void do_env(void);
       command_t *new_command(void);
static int set_remote_mac(const char *remotemac);

void print_command(const command_t *command)
{
#ifndef WIN32
	fprintf(stderr, "command: %s\n", command_name(command));
	if (command->input_path)
		fprintf(stderr, "    input: %s\n", command->input_path);
	if (command->output_path)
		fprintf(stderr, "    output: %s\n", command->output_path);
	if (command->region)
		fprintf(stderr, "    region: %s\n", command->region);
#else
	char str[256];
	
	sprintf (str, "command: hermit\n");
	consoleout (str);
	if (command->input_path) {
		sprintf (str, "    input: %s\n", command->input_path);
		consoleout (str);
	}
	if (command->output_path) {
		sprintf (str, "    output: %s\n", command->output_path);
		consoleout (str);
	}
	if (command->region) {
		sprintf (str, "    region: %s\n", command->region);
		consoleout (str);
	}
#endif
}

command_t *new_command(void)
{
	command_t *cmd;

	cmd = zmalloc(sizeof (command_t));
	cmd->type = CMD_INVALID;
	cmd->input_path = "input.bin";
	cmd->output_path = "-";

	return cmd;
}

const char *command_name(const command_t *command)
{
	command_name_t *cmd;
	for (cmd = commands; cmd->name; ++cmd)
		if (cmd->command == command->type)
			return cmd->name;
	return "(BUG!)";
}

static command_type_t command_type_by_name(const char *name)
{
	command_name_t *cn;
	for (cn = commands; cn->name; ++cn) {
		if (!strcmp(name, cn->name))
			return cn->command;
	}
	return CMD_INVALID;
}

static void do_env(void)
{
	char *p;
	if ((p = getenv("HERMIT_NETIF")))
		opt_netif = p;
	if ((p = getenv("HERMIT_PORT")))
		opt_port = p;
}

#ifndef WIN32
struct baudrate * get_speed(struct baudrate *table, unsigned long target)
{
	for (; table->speed; table++)
		if (table->speed == target)
			return table;
	return NULL;
}

command_t *get_commands(int argc, char *argv[])
{
	command_t *head, *tail;
	char *p;
	int c;

	do_env();
	//set_execname(*argv);
	head = tail = new_command();

	while (1) {
		unsigned long tmp;
		int longindex;

		c = getopt_long(argc, argv, short_options,
				long_options, &longindex);
		if (c < 0)
			break;
		switch (c) {
		case 0:
			if (!strcmp(long_options[longindex].name, "memory-map"))
				opt_memory_map_path = optarg;
			else if (!strcmp(long_options[longindex].name, "remote-mac"))
				if(set_remote_mac(optarg) == -1){
					return NULL;
				}
			break;
		case 1:
			/*
			 * This is a command (an argument not prefixed
			 * by a hyphen).
			 */
			if (command_type_by_name(optarg) == CMD_INVALID) {
				fprintf(stderr, "invalid command: %s\n", optarg);
				break;
			}
			if (head->type != CMD_INVALID) {
				/*
				 * Command-specific options given
				 * before the first command are
				 * applied to the first command.
				 */
				tail->next = new_command();
				tail = tail->next;
			}
			tail->type = command_type_by_name(optarg);
			break;
		case 'a':
			/* XXX need strtoull() */
			tail->addr = strtoul(optarg, &p, 0);
			if (*p != '\0'){
				panicf("invalid address: `%s'\n", optarg);
				return NULL;
			}
			/* XXX should check command */
			tail->info.download.have_address = 1;
			break;
		case 's':
			tail->size = strtoul(optarg, &p, 0);
			if (*p != '\0'){
				panicf("invalid size: `%s'\n", optarg);
				return NULL;
			}
			break;
		case 'b':
			tmp = strtoul(optarg, &p, 10);
			opt_baudrate = get_speed(baudrate_table, tmp);
			if (!opt_baudrate) {
				panicf("unsupported baudrate: `%lu'\n", tmp);
				return NULL;
			}
			break;
		case 'e':
			opt_ethernet = 1;
			break;
		case 'i':
			tail->input_path = optarg;
			break;
		case 'o':
			tail->output_path = optarg;
			break;
		case 'N':
			opt_netif = optarg;
			break;
		case 'P':
			opt_port = optarg;
			break;
		case 'r':
			tail->region = optarg;
			break;
		case 'v':
			opt_verbose = 1;
			break;
		case 'V':
			puts(get_version());
			exit(EXIT_SUCCESS);
		default:
			usage_and_exit();
		}
	}
	if (head->type == CMD_INVALID)
		usage_and_exit();
	return head;
}
#endif

#ifndef WIN32
static int set_remote_mac(const char *remotemac)
{
	if (parsemac(opt_remote_mac_bytes, remotemac)){
		panicf("can't parse specified remote MAC: `%s'", remotemac);
		return -1;
	}
	opt_remote_mac = remotemac;
	return 0;
}
#endif

void usage_and_exit(void)
{
#ifndef WIN32
	printf("Usage: hermit [options] command [command options]\n"
	       "Available commands: download, erase, help, go, map, terminal, upload, md5sum\n"
#ifdef AJ_FIRMUPDATE_SUPPORT
               "Armadillo-J command: firmupdate\n"
#endif
	       "Multiple commands may be given.\n"
	       "General options (defaults) [environment]:\n"
	       "	-e, --ethernet\n"
	       "	-i, --input-file <path>\n"
	       "	--netif <ifname> (eth0) [HERMIT_NETIF]\n"
	       "	--memory-map <path>\n"
	       "	--port <dev> (/dev/ttyS0) [HERMIT_PORT]\n"
	       "	-o, --output-file <path>\n"
	       "	--remote-mac <MAC address>\n"
	       "	-v, --verbose\n"
	       "	-V, --version\n"
	       "Download/Erase options:\n"
	       "	-a, --address <addr>\n"
	       "	-b, --baudrate <baudrate>\n"
	       "	--force-locked\n"
	       "	-r, --region <region name>\n"
	       "Memory map options:\n"
	       "	--anonymous-regions\n"
	       "Md5sum  options:\n"
	       "	-a, --address <addr>\n"
	       "	-r, --region <region name>\n"
	       "	-s, --size <size>\n"
		);
#else
	consoleout ("Usage: hermit [options] command [command options]\n"
	       "Available commands: download, help, go, map, terminal, upload\n"
	       "Multiple commands may be given.\n"
	       "General options (defaults) [environment]:\n"
	       "	-e, --ethernet\n"
	       "	-i, --input-file <path>\n"
	       "	--netif <ifname> (eth0) [HERMIT_NETIF]\n"
	       "	--memory-map <path>\n"
	       "	--port <dev> (/dev/ttyS0) [HERMIT_PORT]\n"
	       "	-o, --output-file <path>\n"
	       "	--remote-mac <MAC address>\n"
	       "	-v, --verbose\n"
	       "	-V, --version\n"
	       "Download/Erase options:\n"
	       "	-a, --address <addr>\n"
	       "	-b, --baudrate <baudrate>\n"
	       "	--force-locked\n"
	       "	-r, --region <region name>\n"
	       "Memory map options:\n"
	       "	--anonymous-regions\n"
		);
#endif
	exit(EXIT_FAILURE);
}

