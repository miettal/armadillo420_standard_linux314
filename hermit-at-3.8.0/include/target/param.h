#ifndef _HERMIT_HERMIT_PARAM_H_
#define _HERMIT_HERMIT_PARAM_H_

#include <command.h>
extern void get_param(char **param, int count);
extern int get_param_value(char *label, char *buf, size_t size);
extern int get_param_count(void);
extern int check_param(char *label);
extern void param_partial_write(u32 start, u32 size, u8* buf, u32 erase);
extern int sethermit_param(int argc, char **argv, char *label);

extern const command_t setenv_command;
extern const command_t clearenv_command;

#endif
