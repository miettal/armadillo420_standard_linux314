#ifndef _HERMIT_TARGET_BOOST_H_
#define _HERMIT_TARGET_BOOST_H_

#include <autoconf.h>
#include <htypes.h>
#include <cache.h>

struct page_table {
	unsigned long paddr;
	unsigned long vaddr;
	unsigned long size;
	unsigned long option;
};

#define BOOST_OFF		0x0000
#define BOOST_LINUX_MODE	0x1000
#define BOOST_ETH_MODE		0x2000

extern int mmu_active;

#if defined(CONFIG_HAS_MMU)
void boost_on(u32 mode);
void boost_off(void);
extern void boost_on_save(u32 mode, u32 *flags);
extern void boost_off_save(u32 *flags);
extern void boost_restore(u32 flags);
#else
#define boost_on(mode)
#define boost_off()
#define boost_on_save(mode, flags) ({ if (0) *flags = 0; 0; })
#define boost_off_save(flags) ({ if (0) *flags = 0; 0; })
#define boost_restore(flags) ({ if (0) flags = 0; 0; })
#endif
#endif /* _HERMIT_TARGET_BOOST_H_ */
