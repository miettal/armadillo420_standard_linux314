/*
 * Copyright (c) 2006 Atmark Techno, Inc.  All Rights Reserved.
 */

#ifndef _HERMIT_HOST_ERASE_H
#define _HERMIT_HOST_ERASE_H

#include "target.h"
#include "ttype.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
extern void (*erase_callback_function)(tsize_t total, tsize_t remain);

extern void register_erase_callback_function(void (*func)(tsize_t, tsize_t));
#endif

extern int erase_addr(target_context_t *tc, 
		      taddr_t addr);
extern int erase_region(target_context_t *tc, 
			const char *region);

#ifdef __cplusplus
}
#endif

#endif /* _HERMIT_HOST_ERASE_H */
