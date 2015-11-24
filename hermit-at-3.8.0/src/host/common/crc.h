/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#ifndef _HERMIT_HOST_CRC_H
#define _HERMIT_HOST_CRC32_H

#if defined(WIN32)
typedef unsigned int uint32_t;
#else
#include <inttypes.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void init_crc32(void);
extern uint32_t crc32(const unsigned char *buf, unsigned size);

#ifdef __cplusplus
}
#endif

#endif /* _HERMIT_HOST_CRC_H */
