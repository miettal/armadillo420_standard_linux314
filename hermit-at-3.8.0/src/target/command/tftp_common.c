#define CORE_NAME "tftp_common"

#include <autoconf.h>
#include <hermit.h>
#include <platform.h>
#include <herrno.h>
#include <ctype.h>
#include <string.h>
#include <io.h>
#include <flash.h>
#include <net/tftp.h>
#include "tftp_common.h"

static struct platform_info *pinfo = &platform_info;

#if defined(CONFIG_CMD_TFTPDL)
static int program_file(struct tftp_file *file)
{
	addr_t dst, src;
	size_t filesize, blksize;
	size_t programed = 0;
	int block;
	int ret;
	int i;

	dev_info_r("programing: %s\n", file->region);
	dev_dbg("  path: %s\n", file->path);

	block = flash_addr_to_eraseblock(file->flash.base);

	if (file->flash.base != flash_get_eraseblock_addr(block))
		return -1;

	dst = file->flash.base;
	src = file->data.base;
	filesize = file->data.size;
	dev_dbg("  dst: %p\n", dst);
	dev_dbg("  src: %p\n", src);
	dev_dbg("  filesize: %p\n", filesize);

	while (programed < filesize) {
		dev_info_r("#");

		blksize = flash_get_eraseblock_size(block);
		if (programed + blksize > filesize) {
			int padding;
			u8 *buf;
			blksize = filesize - programed;

			padding = ((blksize % 4) ? (4 - (blksize % 4)) : 0);
			buf = (u8 *)(file->data.base + filesize);
			for (i=0; i<padding; i++)
				*buf++ = 0xff;
			blksize += padding;
		}

		dev_dbg("ERASE: %p\n", dst);
		dev_dbg("PROGRAM: src(%p) -- size(%p) -> flash(%p)\n",
			   src, blksize, dst);

		ret = flash_erase(dst);
		if (ret != 0) {
			dev_err("erase error\n");
			return ret;
		}
		ret = flash_program(src, dst, blksize);
		if (ret != 0) {
			dev_err("program error\n");
			return ret;
		}
		programed += blksize;
		src += blksize;
		dst += blksize;
		block++;
	}

	dev_info_r("\n\n");

	return 0;
}

int tftp_program_file(struct tftp_command_info *info)
{
	int i;
	int ret;

	for (i=0; i<info->nr_files; i++) {
		if (info->files[i].path) {
			ret = program_file(&info->files[i]);
			if (ret < 0)
				return ret;
		}
	}

	dev_info_r("completed!!\n\n");

	return 0;
}
#endif

int tftp_download_file(struct tftp_command_info *info)
{
	addr_t ram_addr = pinfo->map->free.base;
	int retry;
	int ret;
	int i;

	for (i=0; i<info->nr_files; i++) {
		if (info->files[i].path) {
			retry = TFTP_RETRY;
			while (retry--) {
				ret = tftp_get(info->dev, info->server_ipaddr,
					       info->files[i].path,
					       &info->files[i].data.size,
					       ram_addr,
					       info->files[i].flash.size,
					       info->blksize);
				if (ret == -H_ETIMEDOUT) {
				  dev_err_r("No response from server.\n");
				  netdev_init(info->dev);
				  continue;
				}
				if (ret)
					return ret;
				break;
			}
			if (retry < 0) {
				dev_err_r("Failed download.\n");
				return -H_EIO;
			}
			if (info->files[i].data.size >
			    info->files[i].flash.size) {
				dev_err("too large filesize\n");
				return -H_EIO;
			}
			info->files[i].data.base = ram_addr;
			ram_addr += info->files[i].flash.size;
		}
	}
	return 0;
}

static int parse_ipaddr(const char *buf, unsigned char *ipaddr)
{
	unsigned long num;
	char *ptr = (char *)buf;
	int i;

	for (i=0; i<4; i++) {
		if (is_decimal(*ptr)) {
			num = strtol(ptr, &ptr, 0);
			if (num > 255)
				return -H_EINVAL;
			ipaddr[i] = (unsigned char)num;
			if (i == 3 && *ptr == '\0')
				return 0;
			if (*ptr != '.')
				return -H_EINVAL;
			ptr++;
		} else
			return -H_EINVAL;
	}
	return 0;
}

int tftp_parse_args(int argc, char *argv[], struct tftp_command_info *info)
{
	char *ptr;
	int i;
	int ret;
	int index;

	for (i=1; i<argc; i++)
		if (strcmp("--help", argv[i]) == 0)
			return 1;

	if (argc < 3)
		return -H_EINVAL;

#if defined(CONFIG_FUNC_DHCPC)
	if (strcmp("dhcp", argv[1]) == 0) {
		info->flag |= TFTP_FLAG_USING_DHCP;
	} else
#endif
	{
		ret = parse_ipaddr(argv[1], info->client_ipaddr);
		if (ret)
			return ret;
	}

	ret = parse_ipaddr(argv[2], info->server_ipaddr);
	if (ret)
		return ret;

	for (i=3, index=0; i<argc; i++) {
		if (strncmp(argv[i], "--blksize=", 10) == 0) {
			strncpy(info->blksize, &argv[i][10], 15);
			continue;
		}
		if (strncmp(argv[i], "--fake", 7) == 0) {
			info->flag |= TFTP_FLAG_DOWNLOAD_ONLY;
			continue;
		}
		if (strncmp(argv[i], "--", 2) != 0)
			return -H_EINVAL;

		ptr = strstr(&argv[i][2], "=");
		if (!ptr)
			return -H_EINVAL;
		strncpy(info->files[index].region, &argv[i][2],
			ptr - &argv[i][2]);
		info->files[index].path = ptr + 1;
		index++;
	}

	return 0;
}
