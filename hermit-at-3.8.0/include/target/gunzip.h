/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#ifndef _HERMIT_TARGET_GUNZIP_H
#define _HERMIT_TARGET_GUNZIP_H

#include <command.h>

extern void gunzip_object (char * name, addr_t from, addr_t to, size_t size);
extern int  is_gunzip_object (addr_t addr);

#endif /* _HERMIT_TARGET_GUNZIP_H */
