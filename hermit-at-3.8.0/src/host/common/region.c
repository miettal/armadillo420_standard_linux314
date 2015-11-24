/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "options.h"

#include "ttype.h"
#include "region.h"
#include "util.h"


static void region_print_internal(FILE *f, const mem_region_t *mr, int indent);

static inline void do_indent(FILE *f, int indent)
{
	for (/* nothing */; indent; --indent)
		fputc(' ', f);
}

const char *mem_type_to_text(mem_type_t type)
{
	switch (type) {
	case MEMTYPE_INVALID: return "INVALID";
	case MEMTYPE_FLASH: return "FLASH";
	case MEMTYPE_RAM: return "RAM";
	case MEMTYPE_ROM: return "ROM";
	default: return "unknown!";
	}
}

mem_region_t *region_new(void)
{
	mem_region_t *mr;
	mr = zmalloc(sizeof (mem_region_t));
	mr->datasize = -1;
	mr->bufsize = -1;
	return mr;
}

void region_destroy(mem_region_t *mr)
{
	if (!mr)
		return;
	while (mr) {
		mem_region_t *tmp = mr;
		region_destroy(mr->children);
		mr = mr->next;
		free(tmp);
	}
}

void region_print(const mem_region_t *mr)
{
	region_print_internal(stdout, mr, 0);
}

void region_print_internal(FILE *f, const mem_region_t *mr, int indent)
{
	for (/* nothing */; mr; mr = mr->next) {

		/* don't show anonymous regions unless requested */
		if (strstr(mr->name, "anonymous") && !opt_anon_regions)
			continue;

		do_indent(f, indent);
		fprintf(f, "region: %s\n", mr->name);

		do_indent(f, indent);
		fprintf(f, "  vma: " FTT "\n", PTT(mr->vma));
		do_indent(f, indent);
		fprintf(f, "  lma: " FTT "\n", PTT(mr->lma));
		do_indent(f, indent);
		fprintf(f, "  size: " FTT "\n", PTT(mr->size));

		do_indent(f, indent);
		fprintf(f, "  type: %s\n", mem_type_to_text(mr->type));

		if (mr->datasize > 0) {
			do_indent(f, indent);
			fprintf(f, "  data size: %zd\n", mr->datasize);
		}
		if (mr->bufsize > 0) {
			do_indent(f, indent);
			fprintf(f, "  buffer size: %zu\n", mr->bufsize);
		}

		if (mr->children)
			region_print_internal(f, mr->children, indent + 4);
	}
}

mem_region_t *region_by_name(const mem_region_t *head,
			     const char *name)
{
	for (/* nothing */; head; head = head->next) {
		if (!strcmp(name, head->name))
			return (mem_region_t *) head;
	}
	return NULL;
}

mem_region_t *region_by_name_or_die(const mem_region_t *head,
				    const char *name)
{
	mem_region_t *mr;
	if ((mr = region_by_name(head, name)))
		return mr;
	panicf("no memory region named `%s'", name);
	return 0;
}

mem_region_t *region_by_vma(const mem_region_t *head,
			    taddr_t vma)
{
	for (/* nothing */; head; head = head->next) {
		if (MR_CONTAINS(head, vma))
			return (mem_region_t *) head;
	}
	return NULL;
}

mem_region_t *region_by_vma_or_die(const mem_region_t *head,
				   taddr_t vma)
{
	mem_region_t *mr;
	if ((mr = region_by_vma(head, vma)))
		return mr;
	panicf("no memory region contains address " FTT, PTT(vma));
	return 0;
}

mem_region_t *slurp_file(const char *filename)
{
        FILE *f;
	mem_region_t *mr;

	assert(filename);
#ifndef WIN32
        f = fopen(filename, "r");
#else
        f = fopen(filename, "rb");
#endif
        if (!f) {
		perror("fopen");
		return NULL;
	}
	mr = region_new();
	mr->name = xstrdup(filename);

        fseek(f, 0L, SEEK_END);
        mr->size = ftell(f);
        fseek(f, 0L, SEEK_SET);
	msgf("%s: %lu bytes\n", filename, (off_t) mr->size);

	/* sanity check or C programmer's disease?  you decide! */
	if (mr->size > 128 * 1024 * 1024){
		panicf("%s: File size exceeds 128MiB", filename);
		return NULL;
	}

	/* allocate a little more than needed in case we need to align */
#if !defined(WIN32)
        mr->data = xmalloc(mr->size + (2 * sizeof (long long)));
#else
        mr->data = xmalloc(mr->size + (2 * sizeof (long)));
#endif
        if ((mr->size != fread(mr->data, 1, mr->size, f)) || ferror(f)){
                panicf("%s: Couldn't read whole file", filename);
		return NULL;
	}
        fclose(f);
	return mr;
}

mem_region_t *slurp_file_or_die(const char *filename)
{
	mem_region_t *mr;

	mr = slurp_file(filename);
	if (!mr){
		panicf("couldn't open %s", filename);
		return NULL;
	}
	return mr;
}
