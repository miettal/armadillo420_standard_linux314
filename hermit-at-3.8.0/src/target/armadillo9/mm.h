#ifndef _HERMIT_TARGET_ARMADILLO9_MM_H_
#define _HERMIT_TARGET_ARMADILLO9_MM_H_

#include <platform.h>
#include <htypes.h>
#include <boost.h>

extern int armadillo9_get_mmu_page_tables(struct platform_info *pinfo,
					  u32 mode, struct page_table **pt,
					  int *nr_pt);
#endif
