/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#ifndef _HERMIT_TARGET_BUFFER_H_
#define _HERMIT_TARGET_BUFFER_H_

#include <htypes.h>

/*
 * These variables are present cross-platform, so they are declared here.
 * The size of the buffer is allowed to vary from platform to platform,
 * though, so each platform should define them using lines like these:
 *
 * #define DLBUFSIZE (8*1024)
 * unsigned char dlbuf [DLBUFSIZE];
 * word_t dlbufsize = DLBUFSIZE;
 */
extern u8 dlbuf[];
extern u32 dlbufsize;

#endif /* _HERMIT_TARGET_BUFFER_H_ */
