/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reseved.
 */

#ifndef _HERMIT_HOST_MEMMAP_H
#define _HERMIT_HOST_MEMMAP_H

#include "target.h"
#include "ttype.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void mem_map_print(const target_context_t *tc);
extern void mem_map_read(target_context_t *tc);
extern void mem_map_read_from_file(target_context_t *tc,
				   const char *pathname);

#ifdef __cplusplus
}
#endif

#endif /* _HERMIT_HOST_MEMMAP_H */
