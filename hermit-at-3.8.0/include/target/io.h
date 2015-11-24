/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#ifndef _HERMIT_TARGET_IO_H_
#define _HERMIT_TARGET_IO_H_

/* low level accessors assuming that char, short and int are 8, 16 and
 * 32 bytes, respectively. */
#define read8(addr) \
	({ unsigned char __v = (*(volatile unsigned char *) (addr)); __v; })
#define read16(addr) \
        ({ unsigned short __v = (*(volatile unsigned short *) (addr)); __v; })
#define read32(addr) \
        ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

#define write8(addr, b) \
        (void)((*(volatile unsigned char *) (addr)) = (b))
#define write16(addr, b) \
        (void)((*(volatile unsigned short *) (addr)) = (b))
#define write32(addr, b) \
        (void)((*(volatile unsigned int *) (addr)) = (b))

/* low level character-at-a-time I/O */
extern int hgetchar_timedout(unsigned long timeout);
extern int hputchar_timedout(int c, unsigned long timeout);
#define hgetchar()  (hgetchar_timedout(0))
#define hputchar(c) (hputchar_timedout(c, 0))

/* low level block I/O */
extern int hgetblock(unsigned char *dst, int count);

extern int hprint(const char *str);
extern int hprintf(const char *format, ...);
extern int hsprintf(char *buf, const char *format, ...);
#define hsnprintf(b, s, f, args...) hsprintf(b, f, ##args)

#endif /* _HERMIT_TARGET_IO_H_ */
