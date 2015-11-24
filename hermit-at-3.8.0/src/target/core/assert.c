/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#include <assert.h>
#include <io.h>

void do_assert(const char *assertion, const char *file, int line)
{
	hprintf("-NG assertion failure at %s:%d: %s\n", file, line, assertion);
	while(1);	/* time to reboot */
}
