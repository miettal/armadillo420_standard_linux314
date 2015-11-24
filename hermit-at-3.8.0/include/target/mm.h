#ifndef _MM_H_
#define _MM_H_

extern int mm_init(unsigned long base, unsigned long size, unsigned long align);
extern void *hmalloc(unsigned long size);
extern void hfree(void *addr);

#endif
