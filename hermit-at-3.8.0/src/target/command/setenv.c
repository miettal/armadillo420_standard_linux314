#include <param.h>

static int
setenv_cmdfunc(int argc, char **argv)
{
	return sethermit_param(argc, argv, "setenv");
}

static int
clearenv_cmdfunc(int argc, char **argv)
{
	return sethermit_param(argc, argv, "clearenv");
}

const command_t setenv_command =
	{ "setenv", "<linux options>", "set linux boot options",
	  &setenv_cmdfunc };

const command_t clearenv_command =
	{ "clearenv", "", "clear linux boot options", &clearenv_cmdfunc };

COMMAND(setenv_command);
COMMAND(clearenv_command);
