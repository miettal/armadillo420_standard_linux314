/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#ifndef _HERMIT_TARGET_ASSERT_H_
#define _HERMIT_TARGET_ASSERT_H_

#ifdef NDEBUG
#define assert(expr) ((void) 0)
#else
#define __STRING(x) #x
extern void do_assert(const char *assertion, const char *file, int line);
#define assert(expr) \
	((void) ((expr) ? 0 : \
		(do_assert(__STRING(expr), __FILE__, __LINE__), 0)))
#endif

#endif /* _HERMIT_TARGET_ASSERT_H_ */
