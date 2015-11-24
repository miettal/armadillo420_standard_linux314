/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#ifndef _HERMIT_TARGET_SCAN_H_
#define _HERMIT_TARGET_SCAN_H_

#include <htypes.h>

#define HEX_ERROR (0xFF)

extern unsigned char hex2char(unsigned char c);
extern int scan_dec(const char *str, word_t *value);
extern int scan_hex(const char *str, word_t *value);

/*
 * If str begins with '0x' or '0X', or ends with 'h' or 'H', assume
 * hex.  Otherwise, read a decimal value.
 */
extern int scan(const char *str, word_t *value);

#endif /* _HERMIT_TARGET_SCAN_H_ */
