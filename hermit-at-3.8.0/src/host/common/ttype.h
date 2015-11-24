/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#ifndef _HERMIT_HOST_TTYPE_H
#define _HERMIT_HOST_TTYPE_H

#include <limits.h>

#if defined(WIN32)
typedef int ssize_t;
#endif

#if !defined(WIN32)
/*
 * Hermit should be capable of downloading to a 64-bit target
 * from a 32-bit host, so we use these types to represent
 * addresses and sizes of memory regions on the target.
 */
typedef unsigned long long taddr_t;
typedef unsigned long long tsize_t;

/* used in implementation of FTT/PTT, below */
static inline int ttype_format_size(unsigned long long tt)
{
	return (tt > (unsigned long long) ULONG_MAX) ? 16 : 8;
}
#else
typedef unsigned long taddr_t;
typedef unsigned long tsize_t;

static _inline int ttype_format_size(unsigned long tt)
{
	return (tt > (unsigned long) ULONG_MAX) ? 16 : 8;
}
#endif

/*
 * These are used for printing target values.  They use 8 bytes
 * for values which don't exceed 32-bit precision, and 16 bytes
 * for values which do (the point is to allow 64-bit targets,
 * while avoiding annoyance to those using 32-bits tarkets by
 * making them wade through seas of zeroes to read addresses).
 *
 * Use FTT in a printf format and PTT in the parameter list,
 * like so:
 *
 *	printf("size is " FTT " bytes\n", PTT(tsize));
 */
#define FTT "0x%.*llx"
#define PTT(x) ttype_format_size(x), (x)

#endif /* _HERMIT_HOST_TTYPE_H */
