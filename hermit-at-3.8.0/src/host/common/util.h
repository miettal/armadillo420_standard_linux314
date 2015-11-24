#ifndef _HERMIT_HOST_UTIL_H
#define _HERMIT_HOST_UTIL_H

#include <stdio.h>
#include <sys/types.h>

#if defined(WIN32)
#include <windows.h>
#define __attribute__(arg) 
#define _fd_type HANDLE
#else 
#define _fd_type int
#endif

#include "ttype.h"

#define LINE_LENGTH 4096
#define CMDBUF_LENGTH 128

#define ERROR_VALUE -1

#ifdef __cplusplus
extern "C" {
#endif

/* name of this executable */
//extern const char *execname;
//extern void set_execname(const char *execpath);
#define EXECNAME "hermit"

extern const char *get_version(void);

/* error reporting variants */
extern void perror_xe(const char *errstr) __attribute__ ((noreturn));
extern void perror_xu(const char *errstr) __attribute__ ((noreturn));

/* iteration over files */
typedef int (iter_func_t)(const char*, void*);
extern void per_line(const char *pathname, iter_func_t *func, void *arg);

/* file descriptors used for output functions */
#if 0
extern FILE *msgout, *panicout, *warnout;
extern void init_util_output(void);	/* call immediately */
#endif

/* debugging/convenience output */
extern int msg(const char *message);		/* verbose mode only */
extern int msgf(const char *format, ...);	/* verbose mode only */

extern void panic(const char *message) __attribute__ ((noreturn));
extern void panicf(const char *format, ...) __attribute__ ((noreturn));

extern int warn(const char *message);
extern int warnf(const char *format, ...);

/* die-upon-failure I/O variants */
extern void xclose(_fd_type fd);
extern ssize_t xread(_fd_type fd, void *buf, size_t count);
extern ssize_t xaread(_fd_type fd, void *buf, size_t count);
extern ssize_t xwrite(_fd_type fd, const void *buf, size_t count);
extern ssize_t xawrite(_fd_type fd, const void *buf, size_t count);

/* die-upon-failure heap allocation variants */
extern void *xmalloc(size_t size);	/* checking malloc() */
extern void *zmalloc(size_t size);	/* checking malloc(), zeroed */
extern char *xstrdup(const char *s);

#ifdef __cplusplus
}
#endif

#endif /* _HERMIT_HOST_UTIL_H */
