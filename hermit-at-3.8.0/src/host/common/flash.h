/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#ifndef _HERMIT_HOST_FLASH_H
#define _HERMIT_HOST_FLASH_H

#include "region.h"
#include "target.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void flash_erase_block(target_context_t *tc, mem_region_t *mr);
extern void flash_erase_block_lazy(target_context_t *tc, mem_region_t *mr);
extern int flash_parse_map(mem_region_t *mr, const char *map);

#ifdef __cplusplus
}
#endif

#endif /* _HERMIT_HOST_FLASH_H */
