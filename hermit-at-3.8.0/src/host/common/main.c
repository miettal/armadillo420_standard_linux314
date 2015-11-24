/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(WIN32)
#include <sys/poll.h>
#include <termios.h>
#endif

#include <time.h>
#if !defined(WIN32)
#include <unistd.h>
#endif

#include "crc.h"
#include "download.h"
#include "erase.h"
#include "eth.h"
#include "ethutil.h"
#include "flash.h"
#include "memmap.h"
#include "options.h"
#include "region.h"
#include "serial.h"
#include "target.h"
#include "util.h"
#include "md5sum.h"

#define REQ_CONNECT		0x1
#define REQ_MEM_MAP		0x2
#define REQ_LOCAL_MAC		0x4

static void drop_root(void);
static int require_flags(target_context_t *tc, unsigned flags);

static void drop_root(void)
{
	/* drop root unless we're actually running as root
	   (i.e. real UID is zero in addition to effective) */
#if !defined(WIN32)
	if (geteuid() == 0 && getuid() != 0) {
		msg("dropping root privileges\n");
		if (setuid(getuid()))
			perror_xe("setuid");
		assert(getuid());
		assert(geteuid());
	}
#endif
}

static int require_flags(target_context_t *tc, unsigned flags)
{
	/* filter out already present flags */
	flags &= ~tc->flags;

	if (flags & REQ_CONNECT) {
		if (target_connect(tc)){
			panic("can't connect to target");
			return -1;
		}
		flags |= REQ_CONNECT;

	} else if (flags & REQ_MEM_MAP) {
		if (opt_memory_map_path) {
			mem_map_read_from_file(tc, opt_memory_map_path);
		} else {
			require_flags(tc, REQ_CONNECT);
			target_write_command(tc, "memmap");
			mem_map_read(tc);
		}
		flags |= REQ_MEM_MAP;

	} else if (flags & REQ_LOCAL_MAC) {
		const char zeroes [6] = { 0, };
		assert(opt_ethernet);
		assert(memcmp(tc->local_mac, zeroes, 6));
		flags |= REQ_LOCAL_MAC;
	}

	/* XXX need to handle more than one request! */
	tc->flags |= flags;
	return 0;
}

static int restore_interactive(target_context_t *tc)
{
	char c = '!';

	if (strcmp(tc->medium, "serial")){
		if(target_set_medium(tc, "serial") == -1){
			return -1;
		}
	}
	xwrite(tc->portfd, &c, 1);
	return 0;
}
#include "console.h"
int main(int argc, char *argv[])
{
	command_t *cmd;
	mem_region_t *mr;
	target_context_t *tc;

#if !defined(WIN32)
	cmd = get_commands(argc, argv);
	if(cmd == NULL) return -1;
#endif

	/* open raw ethernet socket if desired, then drop root */
#if !defined(WIN32)
	if (opt_ethernet)
		eth_raw_socket();
#endif
	drop_root();

	/* miscellaneous initialization */

	init_crc32();

	/* open a connection to the target */
	if (!(tc = target_open(opt_port, opt_ethernet ? opt_netif : NULL))){
		panic("couldn't open connection to target");
		return -1;
	}

	while (cmd) {
		command_t *tmp;

		if (opt_verbose)
			print_command(cmd);

		switch (cmd->type) {

#ifdef AJ_FIRMUPDATE_SUPPORT
                case CMD_FIRMUPDATE:
			mr = slurp_file_or_die(cmd->input_path);
			if(require_flags(tc, REQ_MEM_MAP) == -1){
				return -1;
			}
                        firmupdate(tc, mr);
			break;
#endif

		case CMD_DOWNLOAD:
			if (cmd->region && cmd->info.download.have_address){
				panic("can't specify both region and address");
				return -1;
			}

			mr = slurp_file_or_die(cmd->input_path);
			if(require_flags(tc, REQ_MEM_MAP) == -1){
				return -1;
			}
			if (cmd->region)
				download_to_region(tc, mr, cmd->region);
			else if (cmd->info.download.have_address)
				download_to_addr(tc, mr, cmd->addr);
			else{
				warn("download: must specify address or region\n");
				return -1;
			}
			break;

		case CMD_ERASE:
			if (cmd->region && cmd->info.download.have_address){
				panic("can't specify both region and address");
				return -1;
			}

			if(require_flags(tc, REQ_MEM_MAP) == -1){
				return -1;
			}
			if (cmd->region)
				erase_region(tc, cmd->region);
			else if (cmd->info.download.have_address)
				erase_addr(tc, cmd->addr);
			else{
				warn("erase: must specify address or region\n");
				return -1;
			}
			break;

		case CMD_MAP:
			if(require_flags(tc, REQ_MEM_MAP) == -1){
				return -1;
			}
			region_print(tc->memmap);
			return 0;

		case CMD_TERMINAL:
			if(restore_interactive(tc) == -1){
				return -1;
			}
#if !defined(WIN32)
			serial_terminal();
#endif
			return 0;

		case CMD_MD5SUM:
			if (cmd->region && cmd->info.download.have_address){
				panic("md5sum: can't specify both region and address");
				return -1;
			}

			if (cmd->size == 0) {
				warn("md5sum: must specify size\n");
				return -1;
			}

			if (cmd->region){
				if(require_flags(tc, REQ_MEM_MAP) == -1){
					return -1;
				}

				md5sum_region(tc, cmd->region, cmd->size);
			} else if (cmd->info.download.have_address) {
				md5sum_addr(tc, cmd->addr, cmd->size);
			} else{
				warn("md5sum: must specify address or region\n");
				return -1;
			}
			break;

		default:
			warn("unsupported command\n");
			return -1;
		}

		tmp = cmd;
		cmd = cmd->next;
		/*
		 * We don't free paths in the command because we don't
		 * know whether or not they're dynamically allocated.
		 * Thus we leak a little memory.  It's not like this
		 * is a long-running program...
		 */
		free(tmp);
	}

	restore_interactive(tc);
	return 0;
}

