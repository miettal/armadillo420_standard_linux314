/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#ifndef _HERMIT_HOST_ETHUTIL_H
#define _HERMIT_HOST_ETHUTIL_H

#include <stdio.h>

int parsemac(unsigned char *mac, const char *strmac);
void printmac(FILE *f, const unsigned char *mac);
void sprintmac(char *s, const unsigned char *mac);

#endif /* _HERMIT_HOST_ETHUTIL_H */
