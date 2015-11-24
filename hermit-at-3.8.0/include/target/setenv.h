#ifndef _HERMIT_SETENV_H_
#define _HERMIT_SETENV_H_

#include <command.h>

int get_option_count (void);
void get_options (char *argv[]);
extern const command_t clearenv_command;
extern const command_t setenv_command;
#endif
