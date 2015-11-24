/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#include "ethutil.h"

int parsemac(unsigned char *mac, const char *strmac)
{
	/*
	 * byte[] exists for two reasons:
	 * 1. We want to pass integer, not char, pointers to sscanf().
	 * 2. We don't want to modify *mac unless the parse succeeds.
	 */
	unsigned int byte[6];
	int i;

	if (sscanf(strmac, "%x:%x:%x:%x:%x:%x", byte, byte+1, byte+2,
			byte+3, byte+4, byte+5) != 6)
		return -1;
	for (i=0; i<6; ++i)
		mac[i] = (unsigned char) byte[i];
	return 0;
}

void printmac(FILE *f, const unsigned char *mac)
{
	fprintf(f, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1],
		mac[2], mac[3], mac[4], mac[5]);
}

void sprintmac(char *s, const unsigned char *mac)
{
	sprintf(s, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1],
		mac[2], mac[3], mac[4], mac[5]);
}

