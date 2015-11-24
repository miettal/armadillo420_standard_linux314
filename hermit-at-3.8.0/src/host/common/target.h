/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#ifndef _HERMIT_HOST_TARGET_H
#define _HERMIT_HOST_TARGET_H

#ifdef WIN32
#include <windows.h>
#endif

#include "region.h"
#include "util.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct target_context target_context_t;
typedef void (*target_write_func_t)(target_context_t *tc,
				    const void *data, size_t count);
struct target_context {
	/* target functions */
	target_write_func_t write;

	/* target data */
	const char *medium;
	mem_region_t *memmap;
	unsigned char local_mac [6];
	unsigned char remote_mac [6];
	size_t goal, progress;
	size_t mtu;
#ifndef WIN32
	int portfd;
#else
	HANDLE portfd;
#endif
	int sockfd;
	unsigned flags;
};

/* connections to the target */
extern target_context_t *target_open(const char *port, const char *netif);
extern int target_connect(target_context_t *tc);
extern int target_ping(target_context_t *tc);	/* serial only */
extern void target_close(target_context_t *tc);

/* target side configuration */
extern int target_set_medium(target_context_t *tc, const char *medium);

/* reading input from the target side */
extern int target_confirm_response(target_context_t *tc);
extern int target_get_response(target_context_t *tc);
extern char *target_gets(target_context_t *tc, char *s, int size);
extern void target_per_line(target_context_t *tc, iter_func_t *func,
			    void *arg);

/* writing commands and data to the target side */
extern int target_retry_command_and_data(target_context_t *tc,
					  const char *command,
					  const void *data, size_t count);
extern void target_write_command(target_context_t *tc,
				 const char *command);
extern void target_write_data(target_context_t *tc,
			      const void *data, size_t count);

/* utility functions */
extern void target_add_progress(target_context_t *tc, size_t progress);
extern void target_set_goal(target_context_t *tc, size_t goal);

/* utility: 1 for OK, -1 for NG, 0 for neither */
extern int ok_or_ng(const char *s);

#ifdef __cplusplus
}
#endif

#endif /* _HERMIT_HOST_TARGET_H */

