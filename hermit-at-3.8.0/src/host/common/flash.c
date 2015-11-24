/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#ifdef WIN32
#define snprintf _snprintf
#endif

#include "flash.h"
#include "util.h"

static void bad_map(const char *map, const char *pos);

/*
 * Print out the failing block map, with a caret '^' underneath the
 * part that failed to parse.
 */
static void bad_map(const char *map, const char *pos)
{
	ptrdiff_t i;

	fprintf(stderr, "%s: bad block map: %s\n", EXECNAME, map);
	fprintf(stderr, "%s:  problem here: ", EXECNAME);
	for (i = pos - map; i; --i)
		fputc(' ', stderr);
	fputs("^\n", stderr);
}

void flash_erase_block(target_context_t *tc, mem_region_t *mr)
{
	char cmdbuf [CMDBUF_LENGTH];

	assert(tc);
	assert(mr);

	/* use load memory address for actual references */
	snprintf(cmdbuf, sizeof cmdbuf, "erase 0x%08lx",
		 (unsigned long) mr->lma);
	target_write_command(tc, cmdbuf);
	target_confirm_response(tc);
	mr->flags |= MRF_ERASED;
}

void flash_erase_block_lazy(target_context_t *tc, mem_region_t *mr)
{
	assert(mr);
	if (mr->flags & MRF_ERASED)
		return;
	flash_erase_block(tc, mr);
}

/*
 * Flash map is a string like "2x16K/l,6x16K,63x128K", where "2x16K"
 * means "two blocks of size 16 Kbytes each", and the "/l" is a flag
 * meaning "locked" (used for boot blocks which can't be programmed
 * without special intervention).  Possible size suffixes are none
 * (bytes), "K" (kilobytes), and "M" (megabytes).  Parse with a little
 * state machine, and add the blocks as a mem_region_t chain that's a
 * child of the given one.
 */
int flash_parse_map(mem_region_t *mr, const char *map)
{
	enum state { S_NBLOCKS, S_SIZE, S_FLAGS, S_END } state = S_NBLOCKS;
	mem_region_t *cur = mr;
	int nblocks = 0;
	tsize_t size = 0;
	taddr_t vma, lma;
	unsigned flags = 0;
	const char *p;

	assert(mr);
	vma = mr->vma;
	lma = mr->lma;
	assert(map);

	/* clear out prior flash blocks */
	region_destroy(mr->children);
	mr->children = NULL;

	p = map;
	goto loop;
	do {
		++p;
	loop:
		if (isspace(*p))
			continue;

		switch (state) {

		case S_NBLOCKS:
			if (isdigit(*p)) {
				nblocks = (nblocks * 10) + (*p - '0');
			} else if (*p == 'x') {
				state = S_SIZE;
			} else {
				bad_map(map, p);
				goto failure;
			}
			break;

		case S_SIZE:
			switch (*p) {
			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6': case '7':
			case '8': case '9':
				size = (size * 10) + (*p - '0');
				break;
			case 'K':
				size *= 1024;
				break;
			case 'M':
				size *= (1024 * 1024);
				break;
			case '/':
				state = S_FLAGS;
				break;
			case ',': case '\0':
				state = S_END;
				break;
			default:
				bad_map(map, p);
				goto failure;
			}
			break;

		case S_FLAGS:
			switch (*p) {
			case 'l':
				flags |= MRF_LOCKED;
				break;
			case ',': case '\0':
				state = S_END;
				break;
			default:
				bad_map(map, p);
				goto failure;
			}
			break;

		default:
			assert(0);
		}

		if (state == S_END) {
			/*
			 * Create one mem_region_t for each flash
			 * block, and link them into a chain.
			 */
			while (nblocks--) {
				mem_region_t *tmp = region_new();
				tmp->name = xstrdup("(anonymous flash block)");
				tmp->type = MEMTYPE_FLASH;
				tmp->size = size;
				tmp->flags = flags;
				tmp->bufsize = mr->bufsize;

				tmp->vma = vma;
				vma += size;
				tmp->lma = lma;
				lma += size;

				if (cur == mr) {
					mr->children = tmp;
					cur = tmp;
				} else {
					cur->next = tmp;
					cur = tmp;
				}
			}
			if(flags != 0) mr->flags |= flags;
			state = S_NBLOCKS;
			nblocks = 0;
			size = 0;
			flags = 0;
		}

	} while (*p);

	return 0;

failure:
	region_destroy(mr->children);
	mr->children = NULL;
	return -1;
}
