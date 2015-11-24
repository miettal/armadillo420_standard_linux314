#define CORE_NAME "tftpboot"

#include <autoconf.h>
#include <hermit.h>
#include <platform.h>
#include <herrno.h>
#include <command.h>
#include <string.h>
#include <io.h>
#include <boost.h>
#include <led.h>
#include <net/net.h>
#include <net/eth_util.h>
#include <net/dhcp.h>
#include <param.h>
#include <gunzip.h>
#include <command/linux.h>
#include "tftp_common.h"

static struct platform_info *pinfo = &platform_info;

static int get_index_by_name(struct tftp_command_info *info, char *name)
{
	int i;
	for (i=0; i<info->nr_files; i++) {
		if (strncmp(info->files[i].region, name, strlen(name)) == 0)
			return i;
	}
	return -1;
}

static void tftpboot_usage(void)
{
	hprintf("Usage: tftpboot [Client-IPaddr] [Server-IPaddr] [option]\n"
		"\n"
		"  download Image-file from TFTP server, and boot linux.\n"
		"\n"
		"option:\n"
		"  --help            : display this message\n"
		"  --blksize         : octets size for TFTP\n"
		"  --fake            : only download from server\n"
		"  --(region)=[file] : e.g. --kernel=linux.bin.gz\n");
}

static int tftpboot_cmdfunc(int argc, char *argv[])
{
	struct tftp_command_info info;
	int region_count = get_flash_region_count();
	int c_argc;
	char *c_argv[64];
	int ret;
	int i;
	{
		struct tftp_file files[region_count];
		u32 flags;
		int index;

		memzero(&info, sizeof(struct tftp_command_info));
		memzero(files, sizeof(struct tftp_file) * region_count);

		info.dev = pinfo->net_dev;
		info.nr_files = region_count;
		info.files = files;

		led_on(LED_RED);

		ret = tftp_parse_args(argc, argv, &info);
		if (ret < 0) {
			return ret;
		} else if (ret == 1) {
			tftpboot_usage();
			return 0;
		}

		dev_info_r("initializing net-device...");
		ret = netdev_init(info.dev);
		if (ret < 0) {
			led_off(LED_RED);
			return -H_EIO;
		}
		dev_info_r("OK\n");

		boost_on_save(BOOST_ETH_MODE, &flags);

#if defined(CONFIG_FUNC_DHCPC)
		if (info.flag & TFTP_FLAG_USING_DHCP) {
			ret = dhcp_get_ipaddr(info.dev, info.client_ipaddr);
			if (ret)
				return -H_EIO;
		}
#endif

		netdev_set_ipaddr(info.dev, info.client_ipaddr);

		dev_info_r("\n");
		dev_info_r("Client%s: ",
			   (info.flag & TFTP_FLAG_USING_DHCP) ? "(DHCP)" : "");
		print_ip(info.client_ipaddr);
		dev_info_r("Server: ");
		print_ip(info.server_ipaddr);
		if (info.flag & TFTP_FLAG_DOWNLOAD_ONLY)
			dev_info_r("Fake Mode: Enable\n");

		for (i=0; i<info.nr_files; i++) {
			struct region region;
			char *name, *file;

			if (!info.files[i].path)
				break;
			name = info.files[i].region;
			file = info.files[i].path;
			ret = get_flash_region_by_name(name, &region);
			if (ret) {
				dev_err("unknown region: %s\n", name);
				return -H_EINVAL;
			}
			info.files[i].flash.base =
				get_flash_region_start_by_name(name);
			info.files[i].flash.size =
				get_flash_region_size_by_name(name);
			dev_info_r("Region(%s): %s\n", name, file);
		}
		dev_info_r("\n");

		ret = tftp_download_file(&info);
		if (ret) {
			boost_restore(flags);
			return ret;
		}

		netdev_exit(info.dev);

		if (info.flag & TFTP_FLAG_DOWNLOAD_ONLY) {
			boost_restore(flags);
			return 0;
		}

		index = get_index_by_name(&info, "kernel");
		if (index != -1) {
			gunzip_object("kernel:net",
				      info.files[index].data.base,
				      pinfo->map->kernel.base,
				      info.files[index].data.size);
		} else {
			gunzip_object("kernel",
				      get_flash_region_start_by_name("kernel"),
				      pinfo->map->kernel.base,
				      get_flash_region_size_by_name("kernel"));
		}

		index = get_index_by_name(&info, "userland");
		if (index != -1) {
			gunzip_object("ramdisk:net",
				      info.files[index].data.base,
				      pinfo->map->initrd.base,
				      info.files[index].data.size);
		}
#if !defined(CONFIG_PLATFORM_ARMADILLO800EVA)
		else {
			gunzip_object("ramdisk",
				      get_flash_region_start_by_name("userland"),
				      pinfo->map->initrd.base,
				      get_flash_region_size_by_name("userland"));
		}
#endif
		boost_restore(flags);
	}

#if defined(CONFIG_CMD_SETENV)
	{
		char *param[64];
		int param_count;
		c_argc = 1;
		param_count = get_param_count();
		get_param(param, param_count);
		for (i=0; i<param_count; i++) {
			if (param[i][0] != '@')
				c_argv[c_argc++] = param[i];
		}
	}
#else
	c_argc = 0;
	c_argv[c_argc] = NULL;
#endif
	boost_off();

	linux_command.func(c_argc, c_argv);

	return ret;
}

const command_t tftpboot_command = {
	.name = "tftpboot",
	.arghelp = "--help",
	.cmdhelp = "netboot",
	.func = tftpboot_cmdfunc,
};
COMMAND(tftpboot_command);
