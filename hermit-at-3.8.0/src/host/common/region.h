/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#ifndef _HERMIT_HOST_REGION_H
#define _HERMIT_HOST_REGION_H

#ifndef WIN32
#include <unistd.h>	/* for ssize_t */
#endif

#include "ttype.h"

#ifdef __cplusplus
extern "C" {
#endif

/* memory region flags */
#define MRF_LOCKED	0x1
#define MRF_ERASED	0x2

typedef enum mem_type {
	MEMTYPE_INVALID = 0,
	MEMTYPE_FLASH,
	MEMTYPE_RAM,
	MEMTYPE_ROM,
} mem_type_t;

extern const char *mem_type_to_text(mem_type_t type);

typedef struct mem_region mem_region_t;
struct mem_region {
	char *name;
	taddr_t vma, lma;
	tsize_t size;
	mem_type_t type;
	unsigned flags;
	unsigned char *data;
	ssize_t datasize;
	size_t bufsize;
	mem_region_t *children, *next;
};

extern mem_region_t *region_new(void);
extern void region_destroy(mem_region_t *mr);
extern void region_print(const mem_region_t *mr);

/* look up a region using various constraints */
extern mem_region_t *region_by_name(const mem_region_t *head,
				    const char *name);
extern mem_region_t *region_by_name_or_die(const mem_region_t *head,
					   const char *name);
extern mem_region_t *region_by_vma(const mem_region_t *head,
				   taddr_t vma);
extern mem_region_t *region_by_vma_or_die(const mem_region_t *head,
					  taddr_t vma);

/* read a file into the data space of a mem_region_t */
extern mem_region_t *slurp_file(const char *filename);
extern mem_region_t *slurp_file_or_die(const char *filename);

/*
 * Useful for avoiding overflow.
 */
#if !defined(WIN32)
static inline taddr_t MR_LAST(const mem_region_t *mr)
#else
static _inline taddr_t MR_LAST(const mem_region_t *mr)
#endif
{
	return mr->vma + mr->size - 1;
}

#if !defined(WIN32)
static inline int MR_CONTAINS(const mem_region_t *mr, taddr_t addr)
#else
static _inline int MR_CONTAINS(const mem_region_t *mr, taddr_t addr)
#endif
{
	return (addr >= mr->vma) && (addr <= MR_LAST(mr));
}
#if defined(WIN32)
#define inline _inline
#endif
static inline int MR_CAN_CONTAIN(const mem_region_t *mr,
				 taddr_t addr, tsize_t size)
{
	return MR_CONTAINS(mr, addr) &&
		MR_CONTAINS(mr, addr + size - 1);
}

#ifdef __cplusplus
}
#endif

#endif /* _HERMIT_HOST_REGION_H */
