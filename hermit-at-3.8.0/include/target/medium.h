
#ifndef _HERMIT_TARGET_MEDIUM_H_
#define _HERMIT_TARGET_MEDIUM_H_

#include <command.h>

typedef enum medium_type {
	MEDIUM_SERIAL = 0,
#ifdef ENABLE_MEDIUM_ETHERNET
	MEDIUM_ETHERNET,
#endif
} medium_t;

extern medium_t current_medium;

extern const command_t medium_command;

extern int change_console(char *console);
extern int arch_getchar(unsigned long timeout);
extern int arch_putchar(int c, unsigned long timeout);

#ifdef ENABLE_MEDIUM_ETHERNET
extern int ether_read(unsigned char *buf, int bufsize);
#endif

#endif /* _HERMIT_TARGET_MEDIUM_H_ */
