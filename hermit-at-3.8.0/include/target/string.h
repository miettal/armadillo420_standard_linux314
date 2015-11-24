/*
 * Copyright (c) 2001 Blue Mug, Inc.  All Rights Reserved.
 */

#ifndef _HERMIT_TARGET_STR_H
#define _HERMIT_TARGET_STR_H

#include <htypes.h>

extern int memcmp(const void *__s1, const void *__s2, size_t __n);
extern void *memcpy(void *__dest, const void *__src, size_t __n);
extern void *memset(void *__src, int __c, size_t __n);
extern void memzero(void *s, unsigned int n);

extern int strcmp(const char *cs, const char *ct);
extern int strncmp(const char *s1, const char *s2, size_t n);
extern size_t strlen(const char *s);
extern char *strcpy(char *dest, const char *src);
extern char *strncpy(char *dest, const char *src, size_t count);
extern char *strpbrk(const char *cs, const char *ct);
extern char *strstr(const char *s1, const char *s2);
extern unsigned long strtol(const char *ptr, char **endp, int base);

#endif /* _HERMIT_TARGET_STR_H */
