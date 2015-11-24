/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#include <version.h>
#include <command.h>
#include <herrno.h>
#include <io.h>

extern char *target_profile;

const char hermit_version[]
	__attribute__ ((section(".hermit.version"))) = HERMIT_VERSION;

static int version_cmdfunc(int argc, char *argv[])
{
	hprintf("Hermit-At v%s (%s) compiled at %s\n",
		hermit_version, target_profile, BUILD_TIME);
	return 0;
}

const command_t version_command =
	{ "version", 0, "print Hermit version", &version_cmdfunc };

COMMAND_ABBR(version_command, 'V');
