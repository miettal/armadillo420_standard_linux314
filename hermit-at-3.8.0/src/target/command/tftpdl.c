#define CORE_NAME "tftpdl"

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
#include "tftp_common.h"

static struct platform_info *pinfo = &platform_info;

static void tftpdl_usage(void)
{
	hprintf("Usage: tftpdl [Client-IPaddr] [Server-IPaddr] [option]\n"
		"\n"
		"  to download from server using TFTP, and to program "
		"on flash-memory.\n"
		"\n"
		"option:\n"
		"  --help            : display this message\n"
		"  --blksize         : octets size for TFTP\n"
		"  --fake            : only download from server\n"
		"  --(region)=[file] : e.g. --kernel=linux.bin.gz\n");
}

static int tftpdl_cmdfunc(int argc, char *argv[])
{
	struct tftp_command_info info;
	int region_count = get_flash_region_count();
	int ret;
	{
		struct tftp_file files[region_count];
		u32 flags;
		int i;

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
			tftpdl_usage();
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

		if (!(info.flag & TFTP_FLAG_DOWNLOAD_ONLY))
			ret = tftp_program_file(&info);

		boost_restore(flags);

		netdev_exit(info.dev);

		led_off(LED_RED);
	}

	return ret;
}

const command_t tftpdl_command = {
	.name = "tftpdl",
	.arghelp = "--help",
	.cmdhelp = "flash programer using tftp",
	.func = tftpdl_cmdfunc,
};
COMMAND(tftpdl_command);
