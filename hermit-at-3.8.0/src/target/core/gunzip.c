/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */
#include <platform.h>
#include <herrno.h>
#include <assert.h>
#include <io.h>
#include <htypes.h>
#include <string.h>
#include <scan.h>
#include <buffer.h>
#include <htypes.h>
#include <gunzip.h>
#include <map.h>

#define OF(args) args
#define STATIC static

typedef unsigned char  uch;
typedef unsigned short ush;
typedef unsigned long  ulg;

/* sliding window size; must be at least 32K, and a power of two */
#define WSIZE 0x8000

/* diagnostic functions; not used here */
#ifdef DEBUG_GUNZIP
#  define Assert(cond,msg) {if(!(cond)) error(msg);}
#  define Trace(x) fprintf x
#  define Tracev(x) {if (verbose) fprintf x ;}
#  define Tracevv(x) {if (verbose>1) fprintf x ;}
#  define Tracec(c,x) {if (verbose && (c)) fprintf x ;}
#  define Tracecv(c,x) {if (verbose>1 && (c)) fprintf x ;}
#else
#  define Assert(cond,msg)
#  define Trace(x)
#  define Tracev(x)
#  define Tracevv(x)
#  define Tracec(c,x)
#  define Tracecv(c,x)
#endif

static struct platform_info *pinfo = &platform_info;

/* variables accessed/referenced by inflate.c code */
static uch *window;		/* sliding window buffer */
static unsigned outcnt;		/* bytes in output buffer */
       ulg bytes_out;		/* total bytes output */

/* support functions for inflate.c code */
static void flush_window(void);
static void free(void *where);
#define STRING_ERRORS
#ifdef STRING_ERRORS
static void error(const char *message);
#else
#define error(m)
#endif
static void gzip_mark(void **);
static void gzip_release(void **);
static void *malloc(int size);

static uch *hermit_input_ptr;	/* address of next input byte */
static __inline__ uch get_byte(void)
{
	return read8((u32)(hermit_input_ptr++));
}
static __inline__ void put_byte(void)
{
	read8((u32)(--hermit_input_ptr));
}

/* include gunzip implementation from Linux kernel */
#include <inflate.c>

/* variables not used by inflate.c */
#define HERMIT_HEAP_SIZE 0x4000		/* size of heap for mini-allocator */
static ulg hermit_free_mem_ptr;		/* used by mini-allocator below */
static ulg hermit_free_mem_ptr_end;	/* likewise */
static uch *hermit_output_ptr;		/* decompress destination */
static int hermit_progress;		/* < 0 for no progress output;
					   otherwise keeps track of # of
					   times the decompression window
					   has been flushed */

#ifdef STRING_ERRORS
static void error(const char *message)
{
	hprintf("\n\nERROR: %s\n\n -- System halted\n", message);
	while (1);
}
#endif

/* write the contents of the decompression window to output */
static void flush_window(void)
{
	uch *in = window;
	for (bytes_out += outcnt; outcnt; --outcnt) {
		uch ch = *hermit_output_ptr++ = *in++;
		crc = crc_32_tab[((int)crc ^ ch) & 0xff] ^ (crc >> 8);
	}
	if (hermit_progress >= 0)
		hprintf(".");
}

static void *malloc(int size)
{
	void *p;

	assert(size >= 0 || hermit_free_mem_ptr > 0);
	hermit_free_mem_ptr = (hermit_free_mem_ptr + 3) & ~3;	/* align */
	assert(hermit_free_mem_ptr + size <= hermit_free_mem_ptr_end);

	p = (void*) hermit_free_mem_ptr;
	hermit_free_mem_ptr += size;
	return p;
}

static void free(void *where)
{
	/* gzip_mark and gzip_release do the free */
}

static void gzip_mark(void **ptr)
{
	*ptr = (void*) hermit_free_mem_ptr;
}

static void gzip_release(void **ptr)
{
	hermit_free_mem_ptr = (long) *ptr;
}

/* common initialization for the gunzip code */
static void hermit_gunzip_init(void)
{
	addr_t base = pinfo->map->gunzip.base;
	size_t size = pinfo->map->gunzip.size;
	if (size < WSIZE)
		hprintf("ERROR too small map->gunzip size\n");
	window = (uch*)base;
	bytes_out = 0;
	makecrc();
}

int is_gunzip_object (addr_t addr)
{
	if (read8(addr) != 037 ||
	    ((read8(addr + 1) != 0213) &&
	     (read8(addr + 1) != 0236))) {
		return 0;
	} else {
		return 1;
	}
}

void gunzip_object (char * name, addr_t from, addr_t to, size_t size)
{
	if(is_gunzip_object(from) == 0) {
	  unsigned long copy, remain, copied;


		/* copy kernel image to DRAM */
		hprintf("Copying %s", name);

		bytes_out = size;
		remain = bytes_out;
		copied = 0;
		do{
			if(remain >= 256*1024)
				copy = 256*1024;
			else
				copy = remain;
			memcpy((void *)(to + copied),
			       (void *)(from + copied),
			       copy);
			copied += copy;
			remain -= copy;
			hprintf(".");

		}while(remain > 0);

		hprintf("done.\n");
	} else {
		/* initialize variables for decompression */
		hermit_gunzip_init();
		hermit_input_ptr = (uch*) from;
		hermit_free_mem_ptr = WSIZE + (ulg) window;
		hermit_free_mem_ptr_end = hermit_free_mem_ptr + HERMIT_HEAP_SIZE;
		hermit_output_ptr = (uch*) to;

		/* decompress kernel image to DRAM */
		hprintf("Uncompressing %s", name);
		gunzip();
		hprintf("done.\n");
	}
}
