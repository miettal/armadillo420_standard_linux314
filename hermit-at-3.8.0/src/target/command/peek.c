/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */
#include <command.h>
#include <herrno.h>
#include <htypes.h>
#include <io.h>
#include <scan.h>
#include <string.h>
#include <boost.h>

static int peek_cmdfunc(int argc, char *argv[])
{
	/* previous addr and width are remembered for "peeknext" */
	static addr_t addr;
	static size_t width = sizeof (word_t);
	u32 flags;

	if (argc > 2)
		return -H_EUSAGE;
	if (!strcmp(*argv, "peeknext"))
		addr += width;
	else {
		if (!strcmp(*argv, "peek8"))
			width = 1;
		else if (!strcmp(*argv, "peek16"))
			width = 2;
		else
			width = sizeof (word_t);
		if (argc > 1)
			if (scan(*++argv, &addr)) return -H_EADDR;
	}

	addr &= ~(addr_t)(width - 1);	/* align pointer */
	hprintf("*%p == 0x", addr);
	switch (width) {
	case 1:
		{
			uint8_t v;
			boost_off_save(&flags);
			v = *(uint8_t*)addr;
			boost_restore(flags);
			hprintf("%b", v);
			hprintf((v >= 32 && v < 127) ? " '%c'\n" : "\n", v);
		}
		break;
	case 2:
		{
			uint16_t v;
			boost_off_save(&flags);
			v = *(uint16_t*)addr;
			boost_restore(flags);
			hprintf("%h\n", v);
		}
		break;
	case (sizeof (word_t)):
		{
			word_t v;
			boost_off_save(&flags);
			v = *(word_t*)addr;
			boost_restore(flags);
			hprintf("%x\n", v);
		}
		break;
	}
	return 0;
}

const command_t peek8_command =
	{ "peek8", "[<addr>]", "8-bit memory peek", &peek_cmdfunc };
const command_t peek16_command =
	{ "peek16", "[<addr>]", "16-bit memory peek", &peek_cmdfunc };
const command_t peek_command =
	{ "peek", "[<addr>]", "word-size memory peek", &peek_cmdfunc };
const command_t peeknext_command =
	{ "peeknext", 0, "peek next address", &peek_cmdfunc };
