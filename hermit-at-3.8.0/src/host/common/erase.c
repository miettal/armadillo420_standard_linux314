/*
 * Copyright (c) 2006 Atmark Techno, Inc.  All Rights Reserved.
 */
#include <stdio.h>
#include <assert.h>

#include "options.h"
#include "crc.h"
#include "flash.h"
#include "memmap.h"
#include "target.h"
#include "util.h"
#include "ttype.h"

#define UNALIGNED_BITS 3

static tsize_t erase(target_context_t *tc, tsize_t count,
		     mem_region_t *dst, taddr_t offset);
static tsize_t erase_flash(target_context_t *tc, tsize_t count,
			   mem_region_t *dst, taddr_t offset);

#ifdef WIN32
void (*erase_callback_function)(tsize_t total, tsize_t remain) = NULL;

void register_erase_callback_function(void (*func)(tsize_t, tsize_t))
{
	erase_callback_function = func;
}
#endif

static tsize_t erase_flash(target_context_t *tc, tsize_t count,
			   mem_region_t *dst, taddr_t offset)
{
	tsize_t remain = count;
	taddr_t vma = dst->vma + offset;

	while (remain) {
		mem_region_t *block_mr;
		size_t nbytes;

		/* identify (and possibly erase) the flash block */
		block_mr = region_by_vma_or_die(dst->children, vma);
		if ((block_mr->flags & MRF_LOCKED) && !opt_force_locked){
			panic("can't write to locked flash block without --force-locked");
			return -1;
		}
		flash_erase_block(tc, block_mr);

		/* how many bytes to erase? */
		nbytes = block_mr->size;

		/* update per-loop stuff */
		remain -= nbytes;
		vma += nbytes;

#ifdef WIN32
		if (erase_callback_function)
			erase_callback_function(count, remain);
#endif
	}
	return count - remain;
}

static tsize_t erase(target_context_t *tc, tsize_t count,
		     mem_region_t *dst, taddr_t offset)
{
	if (!count)
		return 0;
	assert(dst);
	assert(offset < dst->size);

	msgf("erase: " FTT " bytes:\n", PTT(count));
	msgf("    VMA (virt address): " FTT "\n", PTT(dst->vma + offset));
	msgf("    LMA (load address): " FTT "\n", PTT(dst->lma + offset));
	msgf("    region: %s (type %s)\n", dst->name,
		mem_type_to_text(dst->type));

	switch (dst->type) {
	case MEMTYPE_FLASH:
		if (!dst->children){
			panicf("flash region %s has no block map\n", dst->name);
			return -1;
		}
		return erase_flash(tc, count, dst, offset);
	default:
		panicf("invalid memory type: `%d'", dst->type);
		return -1;
	}
	return 0;
}

int erase_addr(target_context_t *tc,
	       taddr_t addr)
{
	mem_region_t *mr, *blk;
	tsize_t offset, nbytes;

	assert(tc && tc->memmap);

	printf("erasing at address " FTT "\n", PTT(addr));

	mr = region_by_vma_or_die(tc->memmap, addr);
	blk = region_by_vma_or_die(mr->children, addr);
	offset = addr - mr->vma;
	nbytes = erase(tc, blk->size, mr, offset);
	if (!nbytes) {
		warn("erase_addr: no erased\n");
		return -1;
	}
	return 0;
}

int erase_region(target_context_t *tc,
		 const char *region)
{
	int ret;
	mem_region_t *mr;

	assert(tc && tc->memmap);
	assert(region);

	printf("erasing region %s\n", region);
	mr = region_by_name_or_die(tc->memmap, region);
	ret = erase(tc, mr->size, mr, 0);
	if(ret == 0){
		warn("erase_region: no eraseed\n");
		return -1;
	}else if(ret == -1){
		return -1;
	}
	return 0;
}
