/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#ifndef _HERMIT_HOST_DOWNLOAD_H
#define _HERMIT_HOST_DOWNLOAD_H

#include "target.h"
#include "ttype.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
extern void (*download_callback_function)(void *arg1, void *arg2);

extern int register_download_callback_function(void *func);
#endif

extern int download_to_addr(target_context_t *tc,
			    const mem_region_t *src,
			    taddr_t addr);
extern int download_to_region(target_context_t *tc,
			      const mem_region_t *src,
			      const char *region);

#ifdef AJ_FIRMUPDATE_SUPPORT
int firmupdate(target_context_t *tc,
               const mem_region_t *src);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _HERMIT_HOST_DOMNLOAD_H */
