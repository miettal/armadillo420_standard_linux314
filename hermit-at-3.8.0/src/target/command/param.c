#define CORE_NAME "param"

#include <autoconf.h>
#if defined(CONFIG_CMD_SETPARAM_DEBUG)
#define DEBUG
#endif
#include <hermit.h>
#include <init.h>
#include <platform.h>
#include <herrno.h>
#include <flash.h>
#include <string.h>
#include <io.h>
#include <buffer.h>
#include <map.h>
#include <memdev.h>

static struct memory_device *param_dev;

static struct platform_info *pinfo = &platform_info;

#define MAX_PARAM_SIZE		(0x00008000)

struct param_info {
	char buf[MAX_PARAM_SIZE];
	size_t bufsize;
	addr_t current;
	addr_t next;
	size_t count;
};
static struct param_info param;

#define param_read8(addr)					\
({								\
	u8 _v;							\
	if (param_dev->type == MEMDEV_TYPE_CHAR)		\
		memdev_read(param_dev, addr, 1, &_v);		\
	else							\
		_v = read8(pinfo->map->param.base + addr);	\
	_v;							\
})
#define param_read32(addr)					\
({								\
	u32 _v;							\
	if (param_dev->type == MEMDEV_TYPE_CHAR)		\
		memdev_read(param_dev, addr, 4, &_v);		\
	else							\
		_v = read32(pinfo->map->param.base + addr);	\
	_v;							\
})
#define param_erase(addr, size)						\
({									\
	if (param_dev->type == MEMDEV_TYPE_CHAR)			\
		memdev_erase(param_dev, addr, size);			\
	else								\
		memset((void *)pinfo->map->param.base + addr, 0xff, size); \
})
#define param_program(src, dst, size)					\
({									\
	if (param_dev->type == MEMDEV_TYPE_CHAR)			\
		memdev_write(param_dev, dst, size, (void *)src);	\
	else								\
		memcpy((void *)(pinfo->map->param.base + dst),		\
		       (void *)src, size);				\
})

static addr_t _get_current_param_start(void)
{
	addr_t offset, next;

	for (offset = 0, next = 0;
	     offset < pinfo->map->param.size;
	     offset += next) {
		next = param_read32(offset);
		memcpy(&param.buf[offset], &next, sizeof(addr_t));

		if (next == 0xffffffff) {
			offset += sizeof(addr_t);
			break;
		}

		if (next == 0x00000000 || next & 0x3 ||
		    offset + next >= pinfo->map->param.size)
			goto format_err;
	}
	if (offset >= pinfo->map->param.size)
		goto format_err;

	return offset;

format_err:
	dev_dbg("%s(): FORMAT Error!!\n", __func__);
	return -H_EIO;
}

static addr_t _get_next_param_start(void)
{
	addr_t offset, next;

	for (offset = param.current, next = 0;
	     offset < pinfo->map->param.size;
	     offset += sizeof(addr_t)) {
		next = param_read32(offset);
		memcpy(&param.buf[offset], &next, sizeof(addr_t));

		if (next == 0xffffffff) {
			offset += sizeof(addr_t);
			break;
		}
	}
	if (offset >= pinfo->map->param.size)
		return -1;

	return offset;
}

size_t _get_param_count(void)
{
	addr_t offset;
	size_t count;
	u8 c;
	int detect;

	for (offset = param.current, count = 0, detect = 1;
	     offset < pinfo->map->param.size;
	     offset++) {
		c = param_read8(offset);
		param.buf[offset] = c;
		switch (c) {
		case 0x00:
			detect = 1;
			break;
		case 0xff:
			return count;
		default:
			if (detect) {
				count++;
				detect = 0;
			}
			break;
		}
	}

	return 0;
}

static int param_consistency_error(void)
{
	addr_t current = _get_current_param_start();
	if (param.current != current)
		return 1; /* Need param_clear(1) */
	return 0;
}

#define param_clear() _param_clear(0)
#define param_clear_force() _param_clear(1)
static void _param_clear(int force)
{
	static int init = 0;

	dev_dbg("param_clear: %s\n", force ? "force" : "");

	if (init == 0 || force) {
		param.bufsize = (pinfo->map->param.size > MAX_PARAM_SIZE ?
				 MAX_PARAM_SIZE : pinfo->map->param.size);
		memset(param.buf, 0xff, param.bufsize);

		param.current = _get_current_param_start();
		dev_dbg("p.current = %p\n", param.current);
		if (param.current == -1)
			goto broken;

		param.next = _get_next_param_start();
		dev_dbg("p.next = %p\n", param.next);
		if (param.next == -1)
			goto broken;

		param.count = _get_param_count();
		dev_dbg("p.count = %d\n", param.count);
		if (param.count == -1)
			goto broken;

		init = 1;
	} else {
		if (param_consistency_error()) {
			param_clear_force();
		}
	}
	return;
broken:
	param_erase(0, pinfo->map->param.size);
	param_clear_force();
}

static void param_partial_write(addr_t start, size_t size,
				void *buf, int erase)
{
	if (erase) {
		param_erase(0, pinfo->map->param.size);
		param_clear_force();
		start = 0;
	}

	param_program((addr_t)buf, start, size + (size & 1));

	if (param_dev->type == MEMDEV_TYPE_BLOCK)
		memdev_write(param_dev, 0, pinfo->map->param.size,
			     (void *)pinfo->map->param.base);
}

void get_param(char **list, size_t count)
{
	char *ptr;
	int i;

	if (param_consistency_error())
		param_clear_force();

	for (i=0, ptr = &param.buf[param.current]; i<count; i++) {
		list[i] = ptr;
		ptr += strlen(ptr);
		ptr++;
	}
}

int get_param_value(char *label, char *buf, size_t size)
{
	size_t label_len = strlen(label);
	size_t count;
	int i;

	if (param_consistency_error())
		param_clear_force();

	count = param.count;
	{
		char *list[count];
		get_param(list, count);
		for (i=0; i<count; i++) {
			if (strncmp(list[i], label, label_len) == 0 &&
			    list[i][label_len] == '=') {
				size_t copy_len;
				copy_len = strlen(list[i]) - (label_len + 1);
				copy_len = copy_len > size ? size : copy_len;
				strncpy(buf, &list[i][label_len+1], copy_len);
				buf[copy_len] = '\0';
				return 0;
			}
		}
	}
	return -1;
}

size_t get_param_count(void)
{
	if (param_consistency_error())
		param_clear_force();

	return param.count;
}

int check_param(char *label)
{
	size_t count;
	int i;

	if (param_consistency_error())
		param_clear_force();

	count = param.count;
	{
		char *list[count];
		get_param(list, count);
		for (i=0; i<count; i++)
			if (strncmp(list[i], label, strlen(label)) == 0)
				return 1;
	}
	return 0;
}

#if defined(CONFIG_CMD_SETPARAM_DEBUG)
static int param_cmdfunc(int argc, char *argv[])
{
	int count;
	int i;

	param_clear();

	count = param.count;
	{
		char *list[count];
		get_param(list, count);
		for (i=0; i<count; i++)
			dev_dbg("%d: %s\n", i + 1, list[i]);
	}

	return 0;
}
#endif

static int setparam_cmdfunc(int argc, char **argv)
{
	u32 current_offset = param.current;
	u32 next_offset = param.next;
	u32 args_len;
	char buf[CMDLINE_MAXLEN];
	char *ptr = buf;
	int erase = 0;
	int i;

	param_clear();

	dev_dbg("Current Start Offset: %p\n", current_offset);
	dev_dbg("Next Start Offset: %p\n", next_offset);

	for (i=1; i<argc; i++) {
		strcpy(ptr, argv[i]);
		ptr += strlen(argv[i]);
		*(ptr++) = '\0';
	}

	for (; (u32)ptr%4;)
		*(ptr++) = 0xff; /* Padding */

	args_len = (u32)ptr - (u32)buf;

	if (next_offset + args_len > param.bufsize) {
		erase = 1;
		next_offset = sizeof(addr_t);
	} else {
		/* Set Jump Record */
		addr_t jump;
		jump = next_offset - current_offset;
		param_partial_write(current_offset - sizeof(addr_t),
				    sizeof(addr_t),
				    &jump,
				    0);
	}

	/* Program New Param */
	param_partial_write(next_offset,
			    args_len,
			    buf,
			    erase);

	return 0;
}

#if defined(CONFIG_CMD_SETPARAM_DEBUG)
const command_t param_command = {
	.name = "param",
	.arghelp = "",
	.cmdhelp = "display hermit parameters",
	.func = param_cmdfunc,
};
COMMAND(param_command);

const command_t setparam_command = {
	.name = "setparam",
	.arghelp = "<hermit param>",
	.cmdhelp = "set hermit parameters",
	.func = setparam_cmdfunc,
};
COMMAND(setparam_command);
#endif

int
sethermit_param(int argc, char **argv, char *label)
{
	int param_count;
	int count = 0;
	int c_argc = 1;
	int label_len = strlen(label);
	int i;
	int setenv = 0, clearenv = 0;

	param_clear();

	if (strcmp(label, "setenv") == 0)
		setenv = 1;
	else if (strcmp(label, "clearenv") == 0)
		clearenv = 1;

	param_count = get_param_count();

	{
	  char *param[param_count];
	  get_param(param, param_count);

	  /* Print Option */
	  if (setenv || clearenv) {
	    for (i=0; i<param_count; i++) {
	      if (param[i][0] != '@'){
		if (argc < 2 && setenv) {
		  dev_info_r("%d: %s\n", (count++) + 1, param[i]);
		} else {
		  count++;
		}
	      }
	    }
	  } else {
	    for (i=0; i<param_count; i++) {
	      if (strncmp(param[i], label, label_len) == 0) {
		if (argc < 2) {
		  /* label_len + "1" is '=' */
		  dev_info_r("%s: %s\n",
			  &label[1], &param[i][label_len + 1]);
		} else {
		  count = 1;
		}
		break;
	      }
	    }
	  }

	  /* Renew Option */
	  if (argc > 1 || clearenv) {
	    char *c_argv[(param_count - count) + argc];
	    char device[CMDLINE_MAXLEN];
	    c_argv[0] = ""; /* set dummy */

	    if (setenv | clearenv) {
	      /* Set other param */
	      for (i=0; i<param_count; i++)
		if (param[i][0] == '@')
		  c_argv[c_argc++] = param[i];
	      if (setenv) {
		/* Set new param */
		for (i=1; i<argc; i++)
		  c_argv[c_argc++] = argv[i];
	      }
	    } else {
	      /* Set other param */
	      for (i=0; i<param_count; i++)
		if (strncmp(param[i], label, label_len) != 0)
		  c_argv[c_argc++] = param[i];
	      /* Set new label param */
	      strcpy(device, label);
	      device[label_len] = '=';
	      for (i=0; i<strlen(argv[1]); i++)
		device[label_len + i + 1] = argv[1][i];
	      device[label_len + i + 1] = '\0';
	      c_argv[c_argc++] = device;
	    }

#if defined(DEBUG)
	    dev_dbg("NEW_PARAMETER:\n");
	    for (i=1; i<c_argc; i++)
	      dev_dbg_r("%d: %s\n", i, c_argv[i]);
#endif

	    setparam_cmdfunc(c_argc, c_argv);
	  }
	}

	return 0;
}

void param_init(void)
{
	param_dev = find_memdev("hermit/param");
	if (!param_dev) {
		hprintf("param device not found\n");
		return;
	}
	if (param_dev->type == MEMDEV_TYPE_BLOCK) {
		memdev_read(param_dev, 0, pinfo->map->param.size,
			    (void *)pinfo->map->param.base);
		memcpy(param.buf, (void *)pinfo->map->param.base,
		       pinfo->map->param.size);
	}
}
subsys_initcall(param_init);
