#ifndef _HERMIT_TARGET_TAGS_H
#define _HERMIT_TARGET_TAGS_H
 
/* lifted from linux/include/asm-arm/setup.h */

#include <htypes.h>

#define ATAG_NONE	0x00000000

struct tag_header {
	u32 size;
	u32 tag;
};

#define ATAG_CORE	0x54410001

struct tag_core {
	u32 flags;		/* bit 0 = read-only */
	u32 pagesize;
	u32 rootdev;
};

#define ATAG_MEM		0x54410002

struct tag_mem32 {
	u32	size;
	u32	start;
};

#define ATAG_VIDEOTEXT	0x54410003

struct tag_videotext {
	u8		x;
	u8		y;
	u16		video_page;
	u8		video_mode;
	u8		video_cols;
	u16		video_ega_bx;
	u8		video_lines;
	u8		video_isvga;
	u16		video_points;
};

#define ATAG_RAMDISK	0x54410004

struct tag_ramdisk {
	u32 flags;		/* b0 = load, b1 = prompt */
	u32 size;
	u32 start;
};

#define ATAG_INITRD	0x54410005
#define ATAG_INITRD2	0x54420005

struct tag_initrd {
	u32 start;
	u32 size;
};

#define ATAG_SERIAL	0x54410006

struct tag_serialnr {
	u32 low;
	u32 high;
};

#define ATAG_REVISION	0x54410007

struct tag_revision {
	u32 rev;
};

#define ATAG_VIDEOLFB	0x54410008

struct tag_videolfb {
	u16		lfb_width;
	u16		lfb_height;
	u16		lfb_depth;
	u16		lfb_linelength;
	u32		lfb_base;
	u32		lfb_size;
	u8		red_size;
	u8		red_pos;
	u8		green_size;
	u8		green_pos;
	u8		blue_size;
	u8		blue_pos;
	u8		rsvd_size;
	u8		rsvd_pos;
};

#define ATAG_CMDLINE	0x54410009

struct tag_cmdline {
	char	cmdline[1];
};

#define ATAG_ACORN	0x41000101

struct tag_acorn {
	u32 memc_control_reg;
	u32 vram_pages;
	u8 sounddefault;
	u8 adfsdrives;
};

#define ATAG_MEMCLK	0x41000402

struct tag_memclk {
	u32 fmemclk;
};

struct tag {
	struct tag_header hdr;
	union {
		struct tag_core		core;
		struct tag_mem32	mem;
		struct tag_videotext	videotext;
		struct tag_ramdisk	ramdisk;
		struct tag_initrd	initrd;
		struct tag_serialnr	serialnr;
		struct tag_revision	revision;
		struct tag_videolfb	videolfb;
		struct tag_cmdline	cmdline;

		/*
		 * Acorn specific
		 */
		struct tag_acorn	acorn;

		/*
		 * DC21285 specific
		 */
		struct tag_memclk	memclk;
	} u;
};

struct tagtable {
	u32 tag;
	int (*parse)(const struct tag *);
};

#define tag_member_present(tag,member)				\
	((unsigned long)(&((struct tag *)0L)->member + 1)	\
		<= (tag)->hdr.size * 4)

#define tag_next(t)	((struct tag *)((u32 *)(t) + (t)->hdr.size))
#define tag_size(type)	((sizeof(struct tag_header) + sizeof(struct type)) >> 2)

#endif /* _HERMIT_TARGET_TAGS_H */
