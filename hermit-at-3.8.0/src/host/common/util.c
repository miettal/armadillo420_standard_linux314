#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#if !defined(WIN32)
#include <unistd.h>
#endif

#include "options.h"
#include "util.h"

#include "console.h"

#if !defined(WIN32)
const char *get_version(void)
{
	return "Hermit-At v" HERMIT_VERSION;
}
#endif

static inline void _perror_x(const char *errstr)
{
	char str[1024];
	char *errmsg;
	
	errmsg = strerror(errno);

	sprintf(str, "%s: %s: %s\n", EXECNAME, errstr, errmsg);
	console_err(str);
	messagebox(EXECNAME, str);
}

void perror_xe(const char *errstr)
{
	_perror_x(errstr);
	exit(EXIT_FAILURE);
}

void perror_xu(const char *errstr)
{
	_perror_x(errstr);
	usage_and_exit();
}

void per_line(const char *pathname, iter_func_t *func, void *arg)
{
	char buf [LINE_LENGTH];
	FILE *f;

	assert(pathname);
	assert(func);
	if (!(f = fopen(pathname, "r")))
		perror_xe(pathname);
	while (fgets(buf, sizeof buf, f)) {
		if (func(buf, arg) < 0)
			break;
	}
	if (ferror(f))
		perror_xe(pathname);
	fclose(f);
}

int msg(const char *message)
{
	if(!opt_verbose)
		return 0;

	console_msg(message);
	return strlen(message);
}

int msgf(const char *format, ...)
{
	va_list ap;
	char str[1024];

	if (!opt_verbose)
		return 0;

	va_start(ap, format);
	vsprintf(str, format, ap);
	va_end(ap);

	console_msg(str);
	return strlen(str);
}

void panic(const char *message)
{
	char str[1024];

	sprintf(str, "%s: panic: %s\n", EXECNAME, message);
	console_err(str);
	messagebox(EXECNAME, str);

#if !defined(WIN32)
	exit(EXIT_FAILURE);
#endif
}

void panicf(const char *format, ...)
{
	va_list ap;
	char str[1024];
	int ptr;

	ptr = sprintf(str, "%s: panic: ", EXECNAME);

	va_start(ap, format);
	ptr += vsprintf(&str[ptr], format, ap);
	va_end(ap);
	sprintf(&str[ptr], "\n");
	
	console_err(str);
	messagebox(EXECNAME, str);

#if !defined(WIN32)
	exit(EXIT_FAILURE);
#endif
}

int warn(const char *message)
{
	char str[1024];

	sprintf(str, "%s: warning: %s\n", EXECNAME, message);
	console_warn(str);
	return strlen(str);
}

int warnf(const char *format, ...)
{
	va_list ap;
	char str[1024];
	int ptr;

	ptr = sprintf(str, "%s: warning: ", EXECNAME);
	
	va_start(ap, format);
	ptr += vsprintf(&str[ptr], format, ap);
	va_end(ap);
	sprintf(&str[ptr], "\n");

	console_warn(str);
	return strlen(str);
}

/* close or die */
#ifndef WIN32
void xclose(int fd)
{
	if (close(fd))
		perror_xe("close");
}
#else
void xclose(HANDLE fd)
{
	if (!CloseHandle(fd))
		perror_xe("CloseHandle");
}
#endif

/* read or die */
#ifndef WIN32
ssize_t xread(int fd, void *buf, size_t count)
{
	ssize_t retval;
	do {
		retval = read(fd, buf, count);
	} while ((retval < 0) && (errno == EINTR));
	if (retval < 0)
		perror_xe("read");
	return retval;
}
#else
ssize_t xread(HANDLE fd, void *buf, size_t count)
{
	DWORD retval;
	int ret = ReadFile(fd, buf, count, &retval, NULL);
	return (ret == 0) ? ERROR_VALUE :retval;
}
#endif

/* read entire buffer or die */
#ifndef WIN32
ssize_t xaread(int fd, void *__buf, size_t count)
#else
ssize_t xaread(HANDLE fd, void *__buf, size_t count)
#endif
{
	char *buf = (char *)__buf;
	int nread = 1, remain;
	for (remain = count; nread && remain; remain -= nread){
		nread = xread(fd, buf, remain);
		if(nread == ERROR_VALUE) break;
		buf += nread;
	}
	return count;
}

/* write or die */
#ifndef WIN32
ssize_t xwrite(int fd, const void *buf, size_t count)
{
	ssize_t retval;
	do {
		retval = write(fd, buf, count);
	} while ((retval < 0) && (errno == EINTR));
	if (retval < 0)
		perror_xe("write");
	return retval;
}
#else
ssize_t xwrite(HANDLE fd, const void *buf, size_t count)
{
	DWORD retval;
	WriteFile(fd, buf, count, &retval, NULL);
return retval;
}
#endif

/* write entire buffer or die */
#ifndef WIN32
ssize_t xawrite(int fd, const void *__buf, size_t count)
#else
ssize_t xawrite(HANDLE fd, const void *__buf, size_t count)
#endif
{
	char *buf = (char *)__buf;
	int nwritten, remain;
	for (remain = count; remain; remain -= nwritten)
		buf += (nwritten = xwrite(fd, buf, remain));
	return count;
}

/* malloc or die */
void *xmalloc(size_t size)
{
	void *value = malloc(size);
	if (!value) {
#ifndef WIN32
		fprintf(stderr, "%s: virtual memory exhausted\n", EXECNAME);
#else
		char str[256];
		sprintf (str, "hermit: virtual memory exhausted");
		messagebox(EXECNAME, str);
#endif
		exit(7);
	}
	return value;
}

/* zero-malloc or die */
void *zmalloc(size_t size)
{
	void *value = calloc(size, 1);
	if (!value) {
#ifndef WIN32
		fprintf(stderr, "%s: virtual memory exhausted\n", EXECNAME);
#else
		char str[256];
		sprintf (str, "hermit: virtual memory exhausted");
		messagebox(EXECNAME, str);
#endif
		exit(7);
	}
	return value;
}

char *xstrdup(const char *s)
{
	char *value = strdup(s);
	if (!value) {
#ifndef WIN32
		fprintf(stderr, "%s: virtual memory exhausted\n", EXECNAME);
#else
		char str[256];
		sprintf (str, "hermit: virtual memory exhausted");
		messagebox(EXECNAME, str);
#endif
		exit(7);
	}
	return value;
}
