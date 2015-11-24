/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */
#include <stdio.h>
#include <assert.h>
#ifdef WIN32
#define snprintf _snprintf
#endif

#if !defined(WIN32)
#include "eth.h"

#endif
#include "options.h"
#include "crc.h"
#include "flash.h"
#include "memmap.h"
#include "target.h"
#include "util.h"
#include "ttype.h"

#define UNALIGNED_BITS 3

#ifdef AJ_FIRMUPDATE_SUPPORT
#define FIRMWARE_REGION_NAME "hermit"
#endif

typedef enum download_dest { DD_ADDR, DD_BUF } download_dest_t;

static void do_download(target_context_t *tc,
			const void *data, tsize_t count,
			download_dest_t dest, taddr_t addr);
static tsize_t download_data(target_context_t *tc,
			     const void *data, tsize_t count,
			     mem_region_t *dst, taddr_t offset);
static tsize_t download_to_flash(target_context_t *tc,
				 const void *data, tsize_t count,
				 mem_region_t *dst, taddr_t offset);
static tsize_t download_to_ram(target_context_t *tc,
			       const void *data, tsize_t count,
			       mem_region_t *dst, taddr_t offset);

static void do_download(target_context_t *tc,
			const void *data, tsize_t count,
			download_dest_t dest, taddr_t addr)
{
	char cmdbuf [CMDBUF_LENGTH];
	uint32_t crc;
	
	crc = crc32(data, count);
	if (dest == DD_ADDR) {
		snprintf(cmdbuf, sizeof cmdbuf,
		 	"download 0x%08lx %ld 0x%08x",
		 	(unsigned long) addr, (unsigned long) count, crc);
	} else {
		snprintf(cmdbuf, sizeof cmdbuf,
		 	"download buf %ld 0x%08x",
		 	(unsigned long) count, crc);
	}
	target_retry_command_and_data(tc, cmdbuf, data, count);
}

#ifdef WIN32
void (*download_callback_function)(void *arg1,void *arg2) = NULL;

int register_download_callback_function(void *func){
	download_callback_function = func;
	return 0;
}
#endif

/*
 * This function takes just as many arguments as the generic
 * download_data() function, but it relies on that function to perform
 * input filtering for it.  Specifically, at this point we know that
 * all given data is in flash, and that the relevant flash blocks are
 * all within the mem_region_t 'dst' (dst's children should be a block
 * map of this flash part).
 */
static tsize_t download_to_flash(target_context_t *tc,
				 const void *__data, tsize_t count,
				 mem_region_t *dst, taddr_t offset)
{
	char cmdbuf [CMDBUF_LENGTH];
	char *data = (char *)__data;

	tsize_t remain = count;
	taddr_t vma = dst->vma + offset;

	while (remain) {
		mem_region_t *block_mr;
		ssize_t abytes;
		size_t nbytes;

		/* identify (and possibly erase) the flash block */
		block_mr = region_by_vma_or_die(dst->children, vma);
		if ((block_mr->flags & MRF_LOCKED) && !opt_force_locked){
			panic("can't write to locked flash block without --force-locked");
			return -1;
		}
		flash_erase_block_lazy(tc, block_mr);

		/* how many bytes to download? */
		nbytes = block_mr->vma + block_mr->size - vma;
		assert(block_mr->bufsize);
		assert(!(block_mr->bufsize & UNALIGNED_BITS));
		assert(tc->mtu);
		if (nbytes > block_mr->bufsize)
			nbytes = block_mr->bufsize;
		if (nbytes > remain)
			nbytes = remain;
		if (nbytes > tc->mtu)
			nbytes = tc->mtu;

		/* download into flash programming buffer */
		do_download(tc, data, nbytes, DD_BUF, 0);

		/* program one buffer's worth (may not be a complete block) */
		abytes = nbytes;
		if (abytes & UNALIGNED_BITS)
			abytes = (abytes + UNALIGNED_BITS) & ~UNALIGNED_BITS;
		/* program at lma, not vma (current address, not canonical) */
		snprintf(cmdbuf, sizeof cmdbuf, "program 0x%08lx %ld",
			 (unsigned long) (vma - block_mr->vma + block_mr->lma),
			 (long) abytes);
		target_write_command(tc, cmdbuf);
		if(target_confirm_response(tc) == -1){
			return -1;
		}

		/* update per-loop stuff */
		data += nbytes;
		remain -= nbytes;
		vma += nbytes;

#ifdef WIN32
		if(download_callback_function != NULL){
			download_callback_function(&count, &remain);
		}
#endif
	}
	return count - remain;
}

static tsize_t download_to_ram(target_context_t *tc,
			       const void *__data, tsize_t count,
			       mem_region_t *dst, taddr_t offset)
{
	char *data = (char *)__data;
	tsize_t remain = count;
	taddr_t lma = dst->lma + offset;

	while (remain) {
		size_t nbytes = remain;
		if (nbytes > tc->mtu)
			nbytes = tc->mtu;
		do_download(tc, data, nbytes, DD_ADDR, lma);

		/* update per-loop stuff */
		data += nbytes;
		remain -= nbytes;
		lma += nbytes;

#ifdef WIN32
		if(download_callback_function != NULL){
			download_callback_function(&count, &remain);
		}
#endif
	}
	return count - remain;
}

/*
 * Handle the actual downloading of data to the device.  The data must
 * already have been parceled into regions by a higher level; this
 * function assumes the given data fits into the given memory region.
 *
 * RAM is easy; we don't have to do any target-side buffering, so we
 * can just do the whole thing in one pass.
 *
 * Flash is a little more complicated, so we farm it out to a separate
 * function.
 */
static tsize_t download_data(target_context_t *tc,
			     const void *data, tsize_t count,
			     mem_region_t *dst, taddr_t offset)
{
	assert(data);
	if (!count)
		return 0;
	assert(dst);
	assert(offset < dst->size);

	msgf("download: " FTT " bytes:\n", PTT(count));
	msgf("    VMA (virt address): " FTT "\n", PTT(dst->vma + offset));
	msgf("    LMA (load address): " FTT "\n", PTT(dst->lma + offset));
	msgf("    region: %s (type %s)\n", dst->name,
		mem_type_to_text(dst->type));

	/* this initializes progress reporting for target_write_data() */
	target_set_goal(tc, count);

	switch (dst->type) {
	case MEMTYPE_RAM:
		return download_to_ram(tc, data, count, dst, offset);
	case MEMTYPE_FLASH:
		if (!dst->children){
			panicf("flash region %s has no block map\n", dst->name);
			return -1;
		}
		return download_to_flash(tc, data, count, dst, offset);
	case MEMTYPE_ROM:
		panic("can't download to ROM");
		return -1;
	default:
		panicf("invalid memory type: `%d'", dst->type);
		return -1;
	}
	return 0;
}

/*
 * This function can handle spanning multiple memory regions.
 */
int download_to_addr(target_context_t *tc,
		     const mem_region_t *src,
		     tsize_t addr)
{
	mem_region_t *mr;
	tsize_t remain;

	assert(tc && tc->memmap);
	assert(src && src->name);

	msgf("downloading %s at address " FTT "\n", src->name, PTT(addr));

	remain = src->size;
	while (remain) {
		tsize_t offset, nbytes;
		mr = region_by_vma_or_die(tc->memmap, addr);
		offset = addr - mr->vma;
		nbytes = download_data(tc, src->data, remain, mr, offset);
		if (!nbytes) {
			warn("download_to_addr: no data downloaded\n");
			return -1;
		}
		addr += nbytes;
		remain -= nbytes;
	}
	return 0;
}

/*
 * This fails if the data is not entirely contained within the
 * specified memory region; use download_data() for the general case.
 */
int download_to_region(target_context_t *tc,
		       const mem_region_t *src,
		       const char *region)
{
	int ret;
	mem_region_t *mr;

	assert(tc && tc->memmap);
	assert(src && src->name);
	assert(region);

	if (opt_verbose)
		printf("downloading %s into region %s\n", src->name, region);
	mr = region_by_name_or_die(tc->memmap, region);
	if (!MR_CAN_CONTAIN(mr, mr->vma, src->size)){
		panicf("%s: region too small for download", region);
		return -1;
	}
	ret = download_data(tc, src->data, src->size, mr, 0);
	if(ret == 0){
		warn("download_to_region: no data downloaded\n");
		return -1;
	}else if(ret == -1){
		return -1;
	}
	return 0;
}

#ifdef AJ_FIRMUPDATE_SUPPORT

/*
 * These funcitons are firmware update version of download_to_region,
 * download_data and download_to_flash.
 *
 * Because we are assuming a few things about target this function is
 * different in the concept compare to other functions in the host
 * side of hermit, which doesn't assume anything about target side.
 *
 * What we do here:
 *
 *  - erase the Hermit region
 *    (which is 64KiB; this doesn't include hermit config area)
 *  - download and program whole program except the first FIRMUPDATE_FTU byte
 *  - download and program the first FIRMUPDATE_FTU byte
 */

#define FIRMUPDATE_MTU 65536
#define FIRMUPDATE_FTU 1024

static tsize_t firmupdate_to_flash(target_context_t *tc,
                                   const void *data, tsize_t count,
                                   mem_region_t *dst, taddr_t offset)
{
	char cmdbuf [CMDBUF_LENGTH];

	tsize_t remain = count;
	taddr_t vma = dst->vma + offset;

        mem_region_t *block_mr;
        tsize_t abytes, nbytes;

        void *data_ftu;
        taddr_t vma_ftu;

        /* identify the flash block */
        block_mr = region_by_vma_or_die(dst->children, vma);

        /* check all constrain before proceed */
        assert(block_mr->bufsize >= FIRMUPDATE_MTU);
        assert(!(block_mr->bufsize & UNALIGNED_BITS));

        /* change the mtu to FIRMUPDATE_MTU */
        tc->mtu = FIRMUPDATE_MTU;

        /* how many bytes to download? */
        nbytes = block_mr->vma + block_mr->size - vma;

        /* skip first FIRMUPDATE_FTU byte */
        data_ftu    = (void *)data;
        vma_ftu     = vma;
        nbytes     -= FIRMUPDATE_FTU;
        data       += FIRMUPDATE_FTU;
        vma        += FIRMUPDATE_FTU;
        remain     -= FIRMUPDATE_FTU;

        if (nbytes > block_mr->bufsize)
                nbytes = block_mr->bufsize;
        if (nbytes > remain)
                nbytes = remain;
        if (nbytes > tc->mtu)
                nbytes = tc->mtu;

        /* first erase the region */
        flash_erase_block(tc, block_mr);
        /* download into flash programming buffer */
        do_download(tc, data, nbytes, DD_BUF, 0);

        /* program one buffer's worth (may not be a complete block) */
        abytes = nbytes;
        if (abytes & UNALIGNED_BITS)
                abytes = (abytes + UNALIGNED_BITS) & ~UNALIGNED_BITS;
        /* program at lma, not vma (current address, not canonical) */
        snprintf(cmdbuf, sizeof cmdbuf, "program 0x%08lx %ld",
                 (unsigned long) (vma - block_mr->vma + block_mr->lma),
                 (long) abytes);
        target_write_command(tc, cmdbuf);
        if(target_confirm_response(tc) == -1){
		return -1;
	}

        /* now download and program the first FIRMUPDATE_FTU byte */
        do_download(tc, data_ftu, FIRMUPDATE_FTU, DD_BUF, 0);
        snprintf(cmdbuf, sizeof cmdbuf, "program 0x%08lx %ld",
                 (unsigned long) (vma_ftu - block_mr->vma + block_mr->lma),
                 (long) FIRMUPDATE_FTU);
        target_write_command(tc, cmdbuf);
        if(target_confirm_response(tc) == -1){
		return -1;
	}

	return nbytes + FIRMUPDATE_FTU;
}

static tsize_t firmupdate_data(target_context_t *tc,
                               const void *data, tsize_t count,
                               mem_region_t *dst, taddr_t offset)
{
	assert(data);
	if (!count)
		return 0;
	assert(dst);
	assert(offset < dst->size);

	msgf("download: " FTT " bytes:\n", PTT(count));
	msgf("    VMA (virt address): " FTT "\n", PTT(dst->vma + offset));
	msgf("    LMA (load address): " FTT "\n", PTT(dst->lma + offset));
	msgf("    region: %s (type %s)\n", dst->name,
		mem_type_to_text(dst->type));

	/* this initializes progress reporting for target_write_data() */
	target_set_goal(tc, count);

	switch (dst->type) {
	case MEMTYPE_FLASH:
		if (!dst->children){
			panicf("flash region %s has no block map\n", dst->name);
			return -1;
		}
		return firmupdate_to_flash(tc, data, count, dst, offset);
	default:
		panicf("invalid memory type: `%d'", dst->type);
		return -1;
	}
}

int firmupdate(target_context_t *tc,
               const mem_region_t *src)
{
	mem_region_t *mr;

	assert(tc && tc->memmap);
	assert(src && src->name);

	if (opt_verbose)
		printf("updating firmware with %s\n", src->name);
	mr = region_by_name_or_die(tc->memmap, FIRMWARE_REGION_NAME);
	if (!MR_CAN_CONTAIN(mr, mr->vma, src->size)){
		panicf("%s: region too small for download", FIRMWARE_REGION_NAME);
		return -1;
	}
	if (!firmupdate_data(tc, src->data, src->size, mr, 0)) {
		warn("download_to_region: no data downloaded\n");
		return -1;
	}
	return 0;
}
#endif
