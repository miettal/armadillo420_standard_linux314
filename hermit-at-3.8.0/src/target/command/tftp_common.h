#ifndef _HERMIT_TARGET_PLATFORM_TFTPDL_H_
#define _HERMIT_TARGET_PLATFORM_TFTPDL_H_

#include <htypes.h>
#include <map.h>
#include <net/net.h>

#define TFTP_RETRY (3)

struct tftp_file {
	char region[64];
	struct map flash;

	char *path;
	struct map data;
};

struct tftp_command_info {
	struct net_device *dev;

	u8 client_ipaddr[4];
	u8 server_ipaddr[4];
	u32 flag;
#define TFTP_FLAG_DOWNLOAD_ONLY (1<<0)
#define TFTP_FLAG_USING_DHCP (1<<1)
	char blksize[16];

	int nr_files;
	struct tftp_file *files;
};

extern int tftp_program_file(struct tftp_command_info *info);
extern int tftp_download_file(struct tftp_command_info *info);
extern int tftp_parse_args(int argc, char *argv[],
			   struct tftp_command_info *info);
#endif
