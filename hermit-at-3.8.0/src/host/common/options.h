/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#ifndef _HERMIT_CONFIG_H
#define _HERMIT_CONFIG_H

#ifndef WIN32
#include <termios.h>
#endif
#include "ttype.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum command_type {
	CMD_INVALID = 0,
	CMD_DOWNLOAD,
	CMD_ERASE,
	CMD_HELP,
	CMD_MAP,
	CMD_TERMINAL,
	CMD_UPLOAD,
#ifdef AJ_FIRMUPDATE_SUPPORT
        CMD_FIRMUPDATE,
#endif
	CMD_MD5SUM,
} command_type_t;

typedef struct download_command_info download_command_info_t;
struct download_command_info {
	int have_address;
};

typedef struct command command_t;
struct command {
	command_type_t type;
	char *input_path;
	char *output_path;
	char *region;
	taddr_t addr;
	tsize_t size;
	union info_u {
		download_command_info_t download;
	} info;
	command_t *next;
};

#ifdef WIN32
typedef unsigned long speed_t;
#define B9600 9600
#define B19200 19200
#define B38400 38400
#define B57600 57600
#define B115200 115200
#define B230400 230400
#endif

struct baudrate {
	speed_t speed;
};

extern const char *command_name(const command_t *command);
extern void print_command(const command_t *command);
extern command_t *get_commands(int argc, char *argv[]);
#if !defined(WIN32)
extern void usage_and_exit(void) __attribute__ ((noreturn));
#else
extern void usage_and_exit(void);
#endif
extern command_t *new_command(void);

/* making functions for these would be too much work */
extern int opt_anon_regions;
extern int opt_ethernet;
extern int opt_force_locked;
extern int opt_terminal;
extern int opt_verbose;

extern const char *opt_memory_map_path;
extern const char *opt_netif;
extern const char *opt_port;
extern const char *opt_remote_mac;
extern unsigned char opt_remote_mac_bytes[];

extern struct baudrate *opt_baudrate;

#ifdef __cplusplus
}
#endif


#endif /* _HERMIT_CONFIG_H */
