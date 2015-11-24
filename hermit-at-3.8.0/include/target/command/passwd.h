#ifndef _HERMIT_ARMADILLO_PASSWD_H_
#define _HERMIT_ARMADILLO_PASSWD_H_

#include <autoconf.h>
#include <command.h>

#if defined(CONFIG_PASSWD_LOCK)
extern int passwd_authentication(void);
#else
#define passwd_authentication()
#endif

extern const command_t passwd_command;

#endif
