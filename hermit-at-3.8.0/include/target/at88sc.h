#ifndef _HERMIT_TARGET_AT88SC_H_
#define _HERMIT_TARGET_AT88SC_H_

#include <io.h>

extern void at88sc_init(void);
extern int at88sc_read(int offset, u8 *buf, size_t count);
#endif
