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

static int poke_cmdfunc(int argc, char *argv[])
{
	addr_t addr;
	word_t value;
	size_t width;
	u32 flags;

	if (argc != 3)
		return -H_EUSAGE;
	if (!strcmp(*argv, "poke8"))
		width = 1;
	else if (!strcmp(*argv, "poke16"))
		width = 2;
	else
		width = sizeof (word_t);
	if (scan(*++argv, &addr))
		return -H_EADDR;
	if (scan(*++argv, &value))
		return -H_EINVAL;

	addr &= ~(addr_t)(width - 1);	/* align pointer */
	hprintf("*%p = 0x", addr);
	switch (width) {
	case 1:
		hprintf("%b", value);
		hprintf((value >= 32 && value < 127) ? " '%c'\n" : "\n",
			value);
		boost_off_save(&flags);
		*((uint8_t*)addr) = value;
		boost_restore(flags);
		break;
	case 2:
		hprintf("%h\n", value);
		boost_off_save(&flags);
		*((uint16_t*)addr) = value;
		boost_restore(flags);
		break;
	case (sizeof (word_t)):
		hprintf("%x\n", value);
		boost_off_save(&flags);
		*((word_t*)addr) = value;
		boost_restore(flags);
		break;
	}
	return 0;
}

const command_t poke8_command =
	{ "poke8", "<addr> <value>", "8-bit memory poke", &poke_cmdfunc };
const command_t poke16_command =
	{ "poke16", "<addr> <value>", "16-bit memory poke", &poke_cmdfunc };
const command_t poke_command =
	{ "poke", "<addr> <value>", "word-size memory poke", &poke_cmdfunc };
