#include <stdio.h>
#include <assert.h>

#ifdef WIN32
#define snprintf _snprintf
#endif

#include "util.h"
#include "ttype.h"
#include "target.h"

#define BUF_LEN 128

static int _md5sum_addr(target_context_t * tc, taddr_t addr, tsize_t size)
{
	char buf[BUF_LEN];

	if (target_connect(tc)) {
		panic("can't connect to target");
		return -1;
	}

	snprintf(buf, BUF_LEN - 1, "md5sum 0x%x 0x%x", (unsigned int) addr,
		 (unsigned int) size);
	target_write_command(tc, buf);

	target_gets(tc, buf, sizeof(buf));
	fprintf(stdout, "%s", buf);

	return 0;
}

int md5sum_addr(target_context_t * tc, taddr_t addr, tsize_t size)
{
	assert(tc);

	return _md5sum_addr(tc, addr, size);
}

int md5sum_region(target_context_t * tc,
		  const char *region, tsize_t size)
{
	mem_region_t *mr;

	assert(tc && tc->memmap);
	assert(region);

	mr = region_by_name_or_die(tc->memmap, region);

	if (mr->size < size) {
		warn("md5sum_region: too large size\n");
		return -1;
	}

	return _md5sum_addr(tc, mr->vma, size);
}
