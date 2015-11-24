#include <platform.h>
#include <command.h>
#include <herrno.h>

static int
info_cmdfunc(int argc, char *argv[])
{
	struct platform_info *pinfo = &platform_info;

	if (pinfo->display_info)
		pinfo->display_info(pinfo);
	else
		return -H_ENOSYS;

	return 0;
}

const command_t info_command =
	{ "info", "", "display board info",
	  &info_cmdfunc };

COMMAND(info_command);
