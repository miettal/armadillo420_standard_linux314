/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#ifndef _HERMIT_HOST_ETH_H
#define _HERMIT_HOST_ETH_H

#include "target.h"

#define ETH_DOWNLOAD_BLOCK 1490

void eth_raw_socket(void);	/* called before dropping root */
int eth_open(target_context_t *tc, const char *netif);
void eth_send_frame(target_context_t *tc, const void *buf,
		    size_t count, unsigned seq);
void eth_close(target_context_t *tc);

#endif /* _HERMIT_HOST_ETH_H */
