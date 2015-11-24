#ifndef _HERMIT_MICROBLAZE_CPU_H_
#define _HERMIT_MICROBLAZE_CPU_H_

#include <htypes.h>

struct microblaze_info {
	size_t icache_size;
	size_t icache_line;
	size_t dcache_size;
	size_t dcache_line;
};

extern struct microblaze_info cpu_info;

#endif
