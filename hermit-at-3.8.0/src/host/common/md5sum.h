#ifndef _HERMIT_HOST_MD5SUM_H
#define _HERMIT_HOST_MD5SUM_H

#include "ttype.h"
#include "target.h"

#ifdef __cplusplus
extern "C" {
#endif

	int md5sum_addr(target_context_t * tc, taddr_t addr, tsize_t size);

	int md5sum_region(target_context_t * tc,
			  const char *region, tsize_t size);

#ifdef __cplusplus
}
#endif
#endif
