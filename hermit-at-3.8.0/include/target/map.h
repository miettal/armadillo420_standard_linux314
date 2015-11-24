
#ifndef _HERMIT_TARGET_MEMMAP_H_
#define _HERMIT_TARGET_MEMMAP_H_

#include <htypes.h>

struct map {
	addr_t base;
	size_t size;
};

struct region {
	char name[256];
	size_t size;
	addr_t offset;
	int flag;
#define R_RO (1<<0)
#define R_LK (1<<1)
};

struct memory_map {
	struct map flash;
	struct map ram;

	/* */
	struct map param;

	/* */
	struct map boot_param;
	struct map mmu_table;
	struct map kernel;
	struct map initrd;
	struct map gunzip;
	struct map free;
};

extern char *get_current_flash_map(void);
extern int parse_region(char *ptr, char **retptr, struct region *region);
extern int get_memmap_region_by_name(char *name, struct region *region);
extern size_t get_memmap_region_size_by_name(char *name);
extern addr_t get_memmap_region_start_by_name(char *name);
extern int get_flash_region_by_name(char *name, struct region *region);
extern size_t get_flash_region_size_by_name(char *name);
extern addr_t get_flash_region_start_by_name(char *name);
extern int get_flash_region_count(void);

#endif /* _HERMIT_TARGET_MEMMAP_H_ */
