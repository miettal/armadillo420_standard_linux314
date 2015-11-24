#ifndef _HERMIT_TARGET_ARMADILLO5X0_MM_H_
#define _HERMIT_TARGET_ARMADILLO5X0_MM_H_

#include <platform.h>
#include <htypes.h>
#include <boost.h>

extern int armadillo5x0_get_mmu_page_tables(struct platform_info *pinfo,
					    u32 mode, struct page_table **pt,
					    int *nr_pt);
#endif
