#include <platform.h>
#include <command.h>
#include <herrno.h>
#include <string.h>
#include <io.h>
#include <param.h>

static void
setclock_errfunc(void)
{
	hprintf("no support clocks.");
}

static void
setclock_usage(char **clocks)
{
	int i;

	hprintf("Usage: setclock [clock]\n\n"
		"  set CPU frequency.\n\n"
		"support clocks:\n");
	for (i=1; clocks[i]; i++)
		hprintf("  %s%s\n", clocks[i],
			clocks[0] == clocks[i] ? "(default)" : "");
}

static int
setclock_cmdfunc(int argc, char *argv[])
{
	struct platform_info *pinfo = &platform_info;
	char **clocks = pinfo->support_clocks;
	int i;

	for (i=1; i<argc; i++) {
		if (strcmp(argv[i], "--help") == 0) {
			setclock_usage(clocks);
			return 0;
		}
	}

	if (clocks == NULL || !clocks[0]) {
		errfunc = setclock_errfunc;
		return -H_ECUSTOM;
	}

	/* initialize parameter: clock, using default clocks[0] */
	if (!check_param("@clock")) {
		char *c_argv[2] = {argv[0], clocks[0]};
		sethermit_param(2, c_argv, "@clock");
	}

	if (argc > 1) {
		for (i=1; clocks[i]; i++) {
			if (strcmp(argv[1], clocks[i]) == 0) {
				int ret;
				ret = sethermit_param(argc, argv, "@clock");
				if (ret)
					return ret;
				pinfo->change_clock(pinfo, argv[1]);
				return 0;
			}
		}
		return -H_EINVAL;
	} else {
		return sethermit_param(argc, argv, "@clock");
	}
}

const command_t setclock_command =
	{"setclock", "--help", "set cpu frequency", &setclock_cmdfunc};

COMMAND(setclock_command);
