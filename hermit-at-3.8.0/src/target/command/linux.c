/*
 * Copyright (c) 2007 Atmark Techno, Inc.  All Rights Reserved.
 */

#include <autoconf.h>
#include <platform.h>
#include <hermit.h>
#include <herrno.h>
#include <io.h>
#include <atag.h>
#include <gunzip.h>
#include <string.h>
#include <boost.h>
#include <ide.h>
#include <param.h>
#include <map.h>
#include <led.h>
#include <command/tftpboot.h>
#include <mmcsd.h>
#include <irq.h>
#include <linux.h>
#include <squashfs.h>

/* for setting the root device */
#define MINORBITS       8
#define MKDEV(ma,mi)    (((ma) << MINORBITS) | (mi))

#define RAMDISK_MAJOR   1	/* major for "RAM disk" */
#define RAMDISK0_MINOR	0	/* b 1 0 == /dev/ram0 */

static boot_device_t defalt_boot_device_table[] = {
	/*			minor	minor	*/
	/* name		major	start	inc	*/
	{"ram",		1,	0,	1,	},
	{"hda",		3,	0,	1,	},
	{"hdb",		3,	64,	1,	},
	{"hdc",		22,	0,	1,	},
	{"hdd",		22,	64,	1,	},
	{"mtdblock",	31,	0,	1,	},
	{"mmcblk0p",	0,	1,	1,	},
};
#define DEFAULT_BOOT_DEVICE (1) /* RAMDISK */

static int major = RAMDISK_MAJOR;
static int minor = RAMDISK0_MINOR;

#define COMMAND_LINE_SIZE 1024

extern unsigned long bytes_out;

struct platform_info *pinfo = &platform_info;

/****************************************************************************
 * linux_cmbfunc
 ****************************************************************************/
#if defined(CONFIG_ARCH_ARM)
/*
 * Linux Kernel Parameters for ARM:
 *   r0: 0
 *   r1: machine type number
 *   r2: ATAG pointer address
 */
typedef void (*kernel_t)(u32, u32, u32);
static int linux_boot_arm(int argc, char *argv[])
{
	kernel_t kernel = (kernel_t)pinfo->map->kernel.base;
	addr_t param_base = pinfo->map->boot_param.base;
	size_t param_size = pinfo->map->boot_param.size;
	u32 machine_nr = pinfo->machine_nr;

	struct tag *tag = (struct tag *)param_base;
	addr_t dram_base = pinfo->map->ram.base;
	char buf[64];
	int i;

	/* zero param block */
	memzero (tag, param_size);

	/* zero tlb block */
	memzero ((void *)pinfo->map->mmu_table.base,
		 pinfo->map->mmu_table.size);

	/* set up core tag */
	tag->hdr.tag = ATAG_CORE;
	tag->hdr.size = tag_size(tag_core);
	tag->u.core.flags = 0;
	tag->u.core.pagesize = 0x1000;
	tag->u.core.rootdev = MKDEV(major, minor);

	for (i=1; ; i++) {
		struct region region;
		hsprintf(buf, "dram-%d", i);
		if (get_memmap_region_by_name(buf, &region))
			break;
		tag = tag_next(tag);
		tag->hdr.tag = ATAG_MEM;
		tag->hdr.size = tag_size(tag_mem32);
		tag->u.mem.size = region.size;
		tag->u.mem.start = dram_base + region.offset;
	}

	if (major == RAMDISK_MAJOR) {
		/* an initial ramdisk image in flash at 0x00700000 */
		tag = tag_next(tag);
		tag->hdr.tag = ATAG_INITRD2;
		tag->hdr.size = tag_size(tag_initrd);
		tag->u.initrd.start = pinfo->map->initrd.base;
		tag->u.initrd.size  = bytes_out;
	}

	if (pinfo->system_rev) {
		tag = tag_next(tag);
		tag->hdr.tag = ATAG_REVISION;
		tag->hdr.size = tag_size(tag_revision);
		tag->u.revision.rev = pinfo->system_rev;
	}

	if (pinfo->serial_num) {
		tag = tag_next(tag);
		tag->hdr.tag = ATAG_SERIAL;
		tag->hdr.size = tag_size(tag_serialnr);
		tag->u.serialnr.low = pinfo->serial_num;
		tag->u.serialnr.high = 0;
	}

	/* the command line arguments */
	if (argc > 1) {
		tag = tag_next(tag);
		tag->hdr.tag = ATAG_CMDLINE;
		tag->hdr.size = (COMMAND_LINE_SIZE + 1 + 4 +
			 sizeof(struct tag_header)) >> 2;

		{
			const char *src;
			char *dst;
			dst = tag->u.cmdline.cmdline;
			memzero (dst, COMMAND_LINE_SIZE);
			while (--argc > 0) {
				src = *++argv;
				hprintf ("Doing %s\n", src);
				while (*src)
					*dst++ = *src++;
				*dst++ = ' ';
			}
			*--dst = '\0';
		}
	}

	tag = tag_next(tag);
	tag->hdr.tag = 0;
	tag->hdr.size = 0;

	/* branch to kernel image */
	kernel(0, machine_nr, param_base);

	/* never get here */
	return 0;
}
#endif

#if defined(CONFIG_ARCH_MICROBLAZE)
static int linux_boot_microblaze(int argc, char *argv[])
{
	char cmdline[COMMAND_LINE_SIZE];
	char *ptr = cmdline;
	int i;

	for (i=1; i<argc; i++) {
		hprintf("Doing %s\n", argv[i]);
		ptr += hsprintf(ptr, "%s ", argv[i]);
	}
	*ptr = '\0';

	__asm__ volatile (
		"	lw	r6, r0, %0	\n\t"	\
		"	brad	r6		\n\t"   \
		"	addk	r5, r0, %1	\n\t"   \
		: /* none */
		: "r" (&pinfo->map->kernel.base),
		  "r" (cmdline)
		: "r5", "r6");

	return 0;
}
#endif

#if defined(CONFIG_ARCH_PPC)
#include <arch/cpu.h>
/*
 * Linux Kernel Parameters:
 *   r3: ptr to board info data
 *   r4: initrd_start or 0 if no initrd
 *   r5: initrd_end - unused if r4 is 0
 *   r6: Start of command line string
 *   r7: End   of command line string
 */
typedef void (*kernel_t)(struct linux_param *, unsigned long, unsigned long,
			 unsigned long, unsigned long);

static int linux_boot_ppc(int argc, char *argv[])
{
	kernel_t kernel;
	addr_t linux_param = 0x01000000 - sizeof(struct linux_param);
	char cmdline[COMMAND_LINE_SIZE];
	char *ptr = cmdline;
	int i;

	for (i=1; i<argc; i++) {
		hprintf("Doing %s\n", argv[i]);
		ptr += hsprintf(ptr, "%s ", argv[i]);
	}
	*ptr = '\0';

	memcpy((void *)linux_param, &lp, sizeof(struct linux_param));
	kernel = (kernel_t)pinfo->map->kernel.base;
	kernel((struct linux_param *)linux_param, 0, 0, 0, 0);

	return 0;
}
#endif

static int linux_cmdfunc(int argc, char *argv[])
{
#if defined(CONFIG_ARCH_ARM)
	return linux_boot_arm(argc, argv);
#endif
#if defined(CONFIG_ARCH_MICROBLAZE)
	return linux_boot_microblaze(argc, argv);
#endif
#if defined(CONFIG_ARCH_PPC)
	return linux_boot_ppc(argc, argv);
#endif
	return -H_ENOSYS;
}

static int load_image(char *device, int noinitrd)
{
	int loaded_kernel = 0;
#if defined(CONFIG_SYSTEM_BOOT_REGION_KERNEL)
	int loaded_userland = noinitrd;
#endif
	int ret = -1;
	u32 flags;

#if defined(CONFIG_CMD_SETBOOTDEVICE_HD)
	boost_on_save(BOOST_LINUX_MODE, &flags);
	{
		int i;
		char hd[4] = "hda";
		for (i=0; i<4; i++) {
			hd[2] = 'a' + i;
			if (strncmp(device, hd, 3) == 0) {
				ret = ide_load_kernel(device);
				if (!ret)
					loaded_kernel = 1;
			}
		}
	}
	boost_restore(flags);
#endif /* defined(CONFIG_CMD_SETBOOTDEVICE_HDA) */

#if defined(CONFIG_CMD_SETBOOTDEVICE_MMCSD)
	boost_on_save(BOOST_LINUX_MODE, &flags);

	if (strncmp(device, "mmcblk", 6) == 0) {
		ret = mmcsd_load_kernel(device);
		if (!ret)
			loaded_kernel = 1;
	}

	boost_restore(flags);
#endif /* defined(CONFIG_CMD_SETBOOTDEVICE_MMCSD) */

#if defined(CONFIG_CMD_SETBOOTDEVICE_TFTP)
	if (strncmp(device, "tftp", 4) == 0) {
		char *cargv[16] = { "tftpboot" };
		char *ptr = device;
		int cargc = 1;
		while (1) {
			ptr = strstr(ptr, " ");
			if (!ptr)
				break;
			*ptr = '\0';
			ptr++;
			cargv[cargc++] = ptr;
		}

		ret = tftpboot_command.func(cargc, cargv);
		loaded_kernel = 1;
	}
#endif /* defined(CONFIG_CMD_SETBOOTDEVICE_TFTP) */

	boost_on_save(BOOST_LINUX_MODE, &flags);

	if ((!loaded_kernel && !ret) || strcmp(device, "flash") == 0) {

#if defined(CONFIG_SYSTEM_BOOT_REGION_KERNEL)
		gunzip_object(" kernel",
			      get_flash_region_start_by_name("kernel"),
			      pinfo->map->kernel.base,
			      get_flash_region_size_by_name("kernel"));
#elif defined(CONFIG_SYSTEM_BOOT_REGION_IMAGE)
		gunzip_object("image",
			      get_flash_region_start_by_name("image"),
			      pinfo->map->kernel.base,
			      get_flash_region_size_by_name("image"));
#endif /* select(CONFIG_SYSTEM_BOOT_REGION) */
		ret = 0;
	}

#if defined(CONFIG_SYSTEM_BOOT_REGION_KERNEL)
	if (!loaded_userland && !ret) {
#if defined(CONFIG_FS_SQUASH)
		ret = squashfs_copy_to("ramdisk",
				pinfo->map->initrd.base,
				get_flash_region_start_by_name("userland"),
				get_flash_region_size_by_name("userland"));
		if (ret)
#endif
		gunzip_object("ramdisk",
			      get_flash_region_start_by_name("userland"),
			      pinfo->map->initrd.base,
			      get_flash_region_size_by_name("userland"));
		ret = 0;
	}
#endif /* defined(CONFIG_SYSTEM_BOOT_REGION_IMAGE) */

	boost_restore(flags);

	return ret;
}

/****************************************************************************
 * boot_cmdfunc
 ****************************************************************************/
static int boot_cmdfunc(int argc, char *argv[])
{
	char bootdevice[CMDLINE_MAXLEN];
	int noinitrd = 0;
	int c_argc = 1;
	char *c_argv[64];
	int ret;
	int i;
	int mode = 0; /* NORMAL_BOOT */
	static boot_device_t *boot_device_table = defalt_boot_device_table;

	if (!strcmp(argv[0], "mediaboot"))
		mode = 1; /* MEDIA_BOOT */
	else if (strcmp(argv[0], "autoboot"))
		if (pinfo->prepare_normal_boot)
			pinfo->prepare_normal_boot(pinfo);

	led_on(LED_RED);

#if defined(CONFIG_CMD_SETENV)
	if (argc < 2) {
		char *param[64];
		int param_count;
		param_count = get_param_count();
		get_param(param, param_count);
		for (i=0; i<param_count; i++) {
			if (param[i][0] != '@')
				c_argv[c_argc++] = param[i];
		}
	} else
#endif /* defined(CONFIG_CMD_SETENV) */
	{
		c_argc = argc;
		for (i=0; i<c_argc; i++)
			c_argv[i] = argv[i];
	}

	if (pinfo->adjust_boot_args)
		pinfo->adjust_boot_args(pinfo, &c_argc, c_argv);

	for (i=1; i<c_argc; i++) {
		int j;
		if (!strcmp(c_argv[i], "noinitrd"))
			noinitrd = 1;
		else if (!strncmp(c_argv[i], "root=", 5)) {
			int ptr = 5;
			if (!strncmp(&c_argv[i][ptr], "/dev/", 5))
				ptr += 5;
			for (j=0; j<ARRAY_SIZE(boot_device_table); j++) {
				char name_len =
				  strlen(boot_device_table[j].name);
				if (!strncmp(&c_argv[i][ptr],
					     boot_device_table[j].name,
					     name_len)) {
				  u8 dev_minor =
				    (u8)c_argv[i][ptr + name_len] - '0';
				  dev_minor = (dev_minor > 10) ? 0 : dev_minor;

				  major = boot_device_table[j].major;
				  minor = (boot_device_table[j].minor_start +
					   (boot_device_table[j].minor_inc *
					    dev_minor));
				}
			}
		}
	}

	ret = 1;
	if (mode != 1) {
#if defined(CONFIG_CMD_SETBOOTDEVICE)
		char param[CMDLINE_MAXLEN];
		ret = get_param_value("@bootdevice", param, CMDLINE_MAXLEN);
		if (ret == 0)
			hsprintf(bootdevice, "%s", param);
#endif /* defined(CONFIG_CMD_SETBOOTDEVICE) */
		if (pinfo->adjust_boot_device) {
			pinfo->adjust_boot_device(pinfo, bootdevice);
			ret = 0;
		}
		if (ret)
			hsprintf(bootdevice, "flash");
		ret = load_image(bootdevice, noinitrd);
		if (ret) {
			hprintf("failed image loading from '%s' device.\n",
				bootdevice);
			return ret;
		}
	} else {
#if defined(CONFIG_COMPATIBLE_MEDIA_BOOT)
		char *bdevice[] = {
			"hda", "hdb", "hdc", "hdd",
			NULL
		};
		for (i=0; ; i++) {
			if (bdevice[i] == NULL)
				return 0;

			ret = load_image(bdevice[i], noinitrd);
			if (!ret)
				break;
		}
#endif
	}

	irq_disable_all();
	boost_off();

	linux_cmdfunc(c_argc, c_argv);

	return 0;
}

/****************************************************************************
 * commands
 ****************************************************************************/
const command_t linux_command =
	{ "linux", "<linux options>", "start Linux", &linux_cmdfunc };
const command_t boot_command =
	{ "boot", "", "boot default Linux kernel and ramdisk", &boot_cmdfunc };

COMMAND(linux_command);
COMMAND_ABBR(boot_command, 'b');
