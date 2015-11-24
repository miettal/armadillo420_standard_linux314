/**
 * A command to calculate md5 hash for given area.
 */
#include <platform.h>
#include <buffer.h>
#include <io.h>
#include <herrno.h>
#include <scan.h>
#include <command.h>
#include <flash.h>
#include <boost.h>
#include <string.h>
#include <map.h>
#include "md5.h"

static struct platform_info *pinfo = &platform_info;

static int in_flash_region(addr_t addr)
{
	addr_t start = pinfo->map->flash.base;
	addr_t end = start + pinfo->map->flash.size - 1;
	return (start <= addr && addr < end) ? 1 : 0;
}

static int calc_md5(addr_t base, size_t len, void *resblock)
{
	struct md5_ctx ctx;
	char buffer[BLOCKSIZE];
	addr_t p;
	addr_t end = base + len;
	size_t remain;

	md5_init_ctx(&ctx);

	for (p = base; p + BLOCKSIZE <= end; p += BLOCKSIZE) {
		memcpy(buffer, (void *)p, BLOCKSIZE);
		md5_process_block(buffer, BLOCKSIZE, &ctx);
	}

	remain = end - p;
	if (remain > 0) {
		/* process partial block */
		memcpy(buffer, (void *)p, remain);
		md5_process_bytes(buffer, remain, &ctx);
	}

	md5_finish_ctx(&ctx, resblock);

	return 0;
}

static int md5sum_cmdfunc(int argc, char *argv[])
{
	addr_t addr;
	size_t size;
	unsigned char val[16];
	int ret;
	u32 flags;

	if (argc != 3)
		return -H_EUSAGE;
	if (scan(*++argv, &addr))
		return -H_EADDR;
	if (scan(*++argv, &size))
		return -H_EINVAL;
	if (in_flash_region(addr) && (addr & 1))
		return -H_EALIGN;

	boost_on_save(BOOST_LINUX_MODE, &flags);
	ret = calc_md5(addr, size, val);
	boost_restore(flags);

	if (ret != 0)
		return ret;

	hprintf("%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b%b\n",
		val[0], val[1], val[2], val[3],
		val[4], val[5], val[6], val[7],
		val[8], val[9], val[10], val[11],
		val[12], val[13], val[14], val[15]);

	return 0;
}

const command_t md5sum_command = { "md5sum", "<addr> <size>",
	"calculate md5 hash",
	&md5sum_cmdfunc
};

COMMAND(md5sum_command);
