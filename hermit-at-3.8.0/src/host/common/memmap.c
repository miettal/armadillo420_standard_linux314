/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

/*
 * This file handles parsing of the memory map (except for flash block
 * map parsing; that's handled in 'flash.c').  A line in the memory
 * map is in the following format:
 *
 * addr1:addr2[@addr3] MEM name op:value [op:value]
 *
 * Where:
 *
 * 'addr1' is the virtual address (vma) of the first byte.
 * 'addr2' is the virtual address of the last byte.
 * 'addr3' (optional) is the load address (lma).
 * 'MEM' is one of FLA, RAM, or ROM.
 * 'name' is the name of the memory region.
 * 'op:value' are options with values.
 */

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "flash.h"
#include "memmap.h"
#include "options.h"
#include "util.h"

static void bad_desc(const char *desc, const char *pos);
static void mem_map_add(target_context_t *tc, mem_region_t *mr);
static int mem_map_add_string(target_context_t *tc,
			      const char *desc);
static int parse_addrs(mem_region_t *mr, const char *addrs);
static int parse_opt(mem_region_t *mr, const char *opt);
static mem_type_t region_type(const char *type_name);

static void bad_desc(const char *desc, const char *pos)
{
	ptrdiff_t i;

	fprintf(stderr, "%s: bad map entry: %s", EXECNAME, desc);
	fprintf(stderr, "%s:  problem here: ", EXECNAME);
	for (i = pos - desc; i; --i)
		fputc(' ', stderr);
	fputs("^\n", stderr);
}

static void mem_map_add(target_context_t *tc, mem_region_t *mr)
{
	/* XXX should add in ascending start order */
	mr->next = tc->memmap;
	tc->memmap = mr;
}

static int parse_addrs(mem_region_t *mr, const char *addrs)
{
	taddr_t last;
	char *p;

	mr->vma = strtoul(addrs, &p, 0);		/* XXX need strtoull */
	if (*p != ':')
		return -1;
	addrs = p + 1;
	last = strtoul(addrs, &p, 0);			/* XXX need strtoull */
	if (*p == '@') {
		/* relocated load address (lma) is present */
		addrs = p + 1;
		mr->lma = strtoul(addrs, &p, 0);	/* XXX need strtoull */
	} else
		mr->lma = mr->vma;
	if (*p)
		return -1;
	mr->size = last - mr->vma + 1;
	return 0;
}

static int parse_opt(mem_region_t *mr, const char *opt)
{
	assert(mr);
	assert(opt);

	if (!strncmp(opt, "bf:", 3)) {
		/* buffer size */
		char *p;
		mr->bufsize = strtoul(opt + 3, &p, 0);
		if (*p == 'K')
			mr->bufsize *= 1024;
		else if (*p == 'M')
			mr->bufsize *= 1024 * 1024;
		else if (*p)
			return -1;
	} else if (!strncmp(opt, "bl:", 3)) {
		/* flash block map */
		if (flash_parse_map(mr, opt + 3) < 0) {
			warnf("unable to parse flash block map `%s'\n", opt);
			return -1;
		}
	} else{
		warnf("unrecognized memory map option `%s'\n", opt);
		return -1;
	}
	return 0;
}

static mem_type_t region_type(const char *type_name)
{
	if (!strcmp(type_name, "FLA"))
		return MEMTYPE_FLASH;
	if (!strcmp(type_name, "RAM"))
		return MEMTYPE_RAM;
	if (!strcmp(type_name, "ROM"))
		return MEMTYPE_ROM;
	warnf("unrecognized region type `%s'\n", type_name);
	return MEMTYPE_INVALID;
}	

/*
 * This is where we actually parse the memory map lines.
 */
static int mem_map_add_string(target_context_t *tc,
			      const char *desc)
{
	mem_region_t *mr = NULL;
	char *ptr = NULL, *s = NULL, *tok;
	int i = 0;

	mr = region_new();

	/* strtok_r modifies its first argument, so we duplicate
	   'desc' before using it */
	for (s = xstrdup(desc);
#ifndef WIN32
	     (tok = strtok_r(ptr ? NULL : s, " \t\n", &ptr));
#else
	     (tok = strtok(ptr ? NULL : s, " \t\n"));
#endif
	     /* nada */) {
#ifdef WIN32
		ptr = (ptr ? ptr : s) + strlen (tok) + 1;
#endif

		/* order of tokens is fixed */
		switch (i++) {
		case 0:
			if (parse_addrs(mr, tok) < 0)
				goto failure;
			break;
		case 1:
			mr->type = region_type(tok);
			if (mr->type == MEMTYPE_INVALID)
				goto failure;
			break;
		case 2:
			/* name is always valid */
			mr->name = xstrdup(tok);
			break;
		default:
			/* there can be any number of options */
			if (parse_opt(mr, tok) < 0)
				goto failure;
		}
	}
	if (i < 3)
		goto failure;
	mem_map_add(tc, mr);
	free(s);
	return 0;

failure:
	if (tok)
		bad_desc(desc, desc + (tok - s));
	else
		bad_desc(desc, desc + strlen(desc));
	free(mr);
	free(s);
	return -1;
}

/*
 * This allows mem_map_add_string() to be iterated by per_line().
 */
static int mem_map_add_iter(const char *desc, void *arg)
{
	return mem_map_add_string(arg, desc);
}

void mem_map_read(target_context_t *tc)
{
	region_destroy(tc->memmap);
	tc->memmap = NULL;
	msg("reading memory map from target...\n");
	target_per_line(tc, &mem_map_add_iter, tc);
	msg("done reading memory map.\n");
}

void mem_map_read_from_file(target_context_t *tc,
			    const char *pathname)
{
	msgf("reading memory map from file %s\n", pathname);
	region_destroy(tc->memmap);
	tc->memmap = NULL;
	per_line(pathname, &mem_map_add_iter, tc);
}

