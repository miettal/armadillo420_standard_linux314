#define CORE_NAME "passwd"

#include <hermit.h>
#include <herrno.h>
#include <io.h>
#include <delay.h>
#include <command.h>
#include <string.h>
#include <param.h>

#define LF	'\n'
#define CR	'\r'
#define BS	0x08
#define DEL	0x7f

#define HLABEL			"@passwd"
#define HLABEL_MISS		"@passwdmiss"
#define PASSWD_MAX		(64 - 1)
#define PASSWD_RETRY_WAIT	5000 /* milliseconds */

extern char *md5_crypt(const char *key, const char *salt);

static int errno = 0;
enum {
	ERR_UNCHANGE = 1,
	ERR_TOOLONG,
};

#define err_exit(e)	({ errno = e; return -H_ECUSTOM; })

static int get_passwd(const char *msg, char *buf)
{
	int i;

	hprintf("%s: ", msg);
	memset(buf, 0, PASSWD_MAX);
	for (i=0; i<=PASSWD_MAX; ) {
		char c = hgetchar();
		if (c == BS || c == DEL) {
			if (i > 0)
				i--;
			continue;
		} else if (c == CR || c == LF) {
			break;
		}
		buf[i++] = c;
	}
	hprintf("\n");
	if (i == (PASSWD_MAX + 1))
		err_exit(ERR_TOOLONG);

	buf[i] = '\0';

	return 0;
}

static void passwd_errfunc(void)
{
	char *msg = NULL;
	switch (errno) {
	case ERR_UNCHANGE:	msg = "Password unchanged"; break;
	case ERR_TOOLONG:	msg = "Password too long"; break;
	default:		msg = ""; break;
	}
	hprintf("%s", msg);
}

static int passwd_cmdfunc(int argc, char *argv[])
{
	char *c_argv[2] = {argv[0], NULL};
	char *hash;
	char cur_passwd[PASSWD_MAX];
	char new_passwd1[PASSWD_MAX];
	char new_passwd2[PASSWD_MAX];
	char saved_hash[64], cur_hash[64], new_hash[64];
	char miss[16];
	int ret;

	trace();

	errfunc = passwd_errfunc;

	memset(miss, 16, 0);
	ret = get_param_value(HLABEL_MISS, miss, 16);
	if (!ret && miss[0]) {
		if (miss[0] != '0')
			mdelay(PASSWD_RETRY_WAIT);
	}

	ret = get_param_value(HLABEL, saved_hash, 64);
	if (!ret && saved_hash[0]) {
		dev_dbg("saved_hash: %s\n", saved_hash);

		ret = get_passwd("(current) PASSWORD", cur_passwd);
		if (ret)
			return ret;
		dev_dbg("cur_passwd: %s\n", cur_passwd);

		memset(cur_hash, 0, 64);
		hash = md5_crypt(cur_passwd, "");
		if (hash)
			strcpy(cur_hash, hash);

	} else {
		memset(saved_hash, 0, 64);
	}

	if (strcmp(argv[0], "auth") == 0) {
		if (saved_hash[0] && (!cur_hash[0] ||
				      strcmp(saved_hash, cur_hash))) {
			c_argv[1] = "1";
			sethermit_param(2, c_argv, HLABEL_MISS);
			err_exit(ERR_UNCHANGE);
		}
		return 0;
	}

	ret = get_passwd("Enter new PASSWORD", new_passwd1);
	if (ret)
		return ret;
	dev_dbg("new_passwd: %s\n", new_passwd1);

	ret = get_passwd("Retype new PASSWORD", new_passwd2);
	if (ret)
		return ret;
	dev_dbg("new_passwd: %s\n", new_passwd2);

	memset(new_hash, 0, 64);
	hash = md5_crypt(new_passwd1, "");
	if (hash)
		strcpy(new_hash, hash);

	if (saved_hash[0] && cur_hash[0])
		dev_dbg("cur_hash: %s\n", cur_hash);
	if (new_hash[0])
		dev_dbg("new_hash: %s\n", new_hash);

	if (saved_hash[0] && (!cur_hash[0] || strcmp(saved_hash, cur_hash))) {
		c_argv[1] = "1";
		sethermit_param(2, c_argv, HLABEL_MISS);
		err_exit(ERR_UNCHANGE);
	}

	if (strcmp(new_passwd1, new_passwd2))
		err_exit(ERR_UNCHANGE);

	if (new_passwd1[0] == '\0')
		c_argv[1] = "";
	else
		c_argv[1] = new_hash;
	ret = sethermit_param(2, c_argv, HLABEL);
	if (ret)
		return -H_EIO;
	c_argv[1] = "0";
	sethermit_param(2, c_argv, HLABEL_MISS);

	hprintf("Password updated successfully\n");

	return 0;
}

const command_t passwd_command = {
	.name = "passwd",
	.arghelp = "",
	.cmdhelp = "change password",
	.func = passwd_cmdfunc
};
COMMAND(passwd_command);

int passwd_authentication(void)
{
	char *argv[1] = {"auth"};
	char hash[64];
	int ret;

	ret = get_param_value(HLABEL, hash, 64);
	if (!ret && hash[0])
		while (passwd_command.func(1, argv));

	return 0;
}
