#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#if defined(WIN32)
#define inline _inline
#endif


//+++ ConsoleOut ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
#if defined(_CONSOLE)
#define console_msg(str)    _console_message(str)
#define console_warn(str)   _console_warning(str)
#define console_err(str)    _console_error(str)
#define console_perror(str) perror(str)
#else
static inline int dummy_console(const char *str){return 0;}
static inline int dummy_perror(const char *str){return 0;}
#define console_msg(str)    dummy_console(str)
#define console_warn(str)   dummy_console(str)
#define console_err(str)    dummy_console(str)
#define console_perror(str) dummy_perror(str)
#endif


//+++ MessageBox ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
#if defined(WIN32)
#define messagebox(title, str) _messagebox_using_sdk(title, str)
#else
static inline void dummy_messagebox(const char *title,const char *str){}
#define messagebox(title, str) dummy_messagebox(title, str)
#endif

int _console_message(const char *str);
int _console_warning(const char *str);
int _console_error(const char *str);

#if defined(WIN32)
void _messagebox_using_sdk(const char *title,const char *str);
#endif
#endif


