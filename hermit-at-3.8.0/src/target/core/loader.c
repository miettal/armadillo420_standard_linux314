/*
 * Copyright (c) 2007 Atmark Techno, Inc.  All Rights Reserved.
 */

#include <autoconf.h>
#include <platform.h>
#include <init.h>
#include <command.h>
#include <command/linux.h>
#include <command/version.h>
#include <command/passwd.h>
#include <crc.h>
#include <led.h>
#include <medium.h>
#include <string.h>

extern const command_t *__command_list_start;
extern const abbrev_t *__command_abbrev_start;
extern const initcall_t __initcall_start;

static const command_t **commands = &__command_list_start;
static const abbrev_t **abbrevs = &__command_abbrev_start;

#define print_version() ((*version_command.func)(0, 0))

#if defined(CONFIG_AUTOBOOT)
static char *autoboot_args[] = { "autoboot" };
#define autoboot() ((*boot_command.func)(1, autoboot_args))
#else
#define autoboot()
#endif

#if defined(CONFIG_COMPATIBLE_MEDIA_BOOT)
static char *mediaboot_args[] = { "mediaboot" };
#define mediaboot() ((*boot_command.func)(1, mediaboot_args))
#else
#define mediaboot()
#endif

#define hermit_command_loop()                                           \
({                                                                      \
	command_list_t command_list = {commands, abbrevs, "hermit", 0}; \
	do_command_loop(&command_list);                                 \
	for(;;);                                                        \
})

static void do_initcall(void)
{
	initcall_t *func;

	func = (initcall_t *)&__initcall_start;
	while (*func) {
		(*func)();
		func++;
	}
}

int hmain(void)
{
	struct platform_info *pinfo = &platform_info;

	/* Both red and green led is already truned on at boot.S. */

	do_initcall();

	print_version();

	if (pinfo->is_autoboot) {
		if (pinfo->is_autoboot(pinfo))
			autoboot();
		else
			mediaboot();
	}

	led_off(LED_RED);

	if (strcmp(CONFIG_DEFAULT_CONSOLE, "none") == 0)
		change_console(CONFIG_STANDARD_CONSOLE);

	passwd_authentication();

	hermit_command_loop();
}
