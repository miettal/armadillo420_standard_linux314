/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#ifndef _HERMIT_HOST_SERIAL_H
#define _HERMIT_HOST_SERIAL_H

#ifdef WIN32
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* return fd of serial connection to loader */
#ifndef WIN32
extern int open_port(const char *dev);
extern void close_port(int portfd);
#define set_serial_timeout(portfd, msec)
#else
extern HANDLE open_port(const char *dev);
extern void close_port(HANDLE portfd);
extern int set_serial_timeout(HANDLE portfd, int msec);
#define TIMEOUT_FLASH  5000
#define TIMEOUT_NORMAL 1000
#endif
extern void serial_terminal(void);

#ifdef __cplusplus
}
#endif

#endif /* _HERMIT_HOST_SERIAL_H */
