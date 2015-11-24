#include <autoconf.h>
#include <io.h>
#include <herrno.h>
#include <htypes.h>
#include <string.h>
#include <param.h>

char *support_bootdevices[] = {
#if defined(CONFIG_CMD_SETBOOTDEVICE_FLASH)
	"flash",
#endif
#if defined(CONFIG_CMD_SETBOOTDEVICE_HD)
	"hda", "hdb", "hdc", "hdd",
#endif
#if defined(CONFIG_CMD_SETBOOTDEVICE_MMCSD)
	"mmcblk",
#endif
#if defined(CONFIG_CMD_SETBOOTDEVICE_TFTP)
	"tftp",
#endif
	NULL,
};

static void
setbootdevice_errfunc(void)
{
	hprintf("no support devices.");
}

static void
setbootdevice_usage(char **devices)
{
	int i;

	hprintf("Usage: setbootdevice [device]\n\n"
		"  set bootdevice.\n\n"
		"support devices:\n");
	for (i=0; devices[i]; i++)
		hprintf("  %s%s\n", devices[i], i==0 ? "(default)" : "");
}

static int
setbootdevice_cmdfunc(int argc, char **argv)
{
	char **devices = support_bootdevices;
	int i;

	for (i=1; i<argc; i++) {
		if (strcmp(argv[i], "--help") == 0) {
			setbootdevice_usage(devices);
			return 0;
		}
	}

	if (devices == NULL || !devices[0]) {
		errfunc = setbootdevice_errfunc;
		return -H_ECUSTOM;
	}

	/* initialize parameter: bootdevice,
	   using CONFIG_CMD_SETBOOTDEVICE_DEFAULT_DEVICE or devices[0] */
	if (!check_param("@bootdevice")) {
		char *c_argv[2] = {argv[0], NULL};
		if (strlen(CONFIG_CMD_SETBOOTDEVICE_DEFAULT_DEVICE))
			c_argv[1] = CONFIG_CMD_SETBOOTDEVICE_DEFAULT_DEVICE;
		else
			c_argv[1] = devices[0];
		sethermit_param(2, c_argv, "@bootdevice");
	}

	if (argc > 1) {
		for (i=0; devices[i]; i++) {
			int j;
			if (strncmp(argv[1], devices[i], strlen(devices[i]))
			    == 0) {
				char buf[CMDLINE_MAXLEN];
				char *ptr = buf;
				char *cargv[2] = { argv[0], buf };
				memzero(buf, CMDLINE_MAXLEN);
				for (j=1; j<argc; j++) {
					ptr += hsprintf(ptr, "%s%s",
							j==1 ? "" : " ",
							argv[j]);
				}
				return sethermit_param(2, cargv,
						       "@bootdevice");
			}
		}
		return -H_EINVAL;
	} else {
		return sethermit_param(argc, argv, "@bootdevice");
	}
}

const command_t setbootdevice_command =
	{ "setbootdevice", "--help",
	  "linux kernel location", &setbootdevice_cmdfunc };

COMMAND_ABBR(setbootdevice_command, 'l');
