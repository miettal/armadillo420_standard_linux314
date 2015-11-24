#define CORE_NAME "boost"

#include <autoconf.h>
#include <hermit.h>
#include <platform.h>
#include <htypes.h>
#include <string.h>
#include <irq.h>

static struct platform_info *pinfo = &platform_info;

#if defined(CONFIG_ARCH_ARM)
#include <boost.h>
#include <map.h>

int mmu_active = 0;

extern void arch_boost_on(addr_t);
extern void arch_boost_off(void);

static void mmu_create_page_table(addr_t base, struct page_table *pt)
{
	volatile unsigned long *table;
	unsigned long offset;
	unsigned long i;

	dev_dbg("P:%p, V:%p, S:%p, O:%p\n",
		pt->paddr, pt->vaddr, pt->size, pt->option);
	offset = base + (pt->vaddr >> 18);
	for (i = 0; i < (pt->size >> 20); i++) {
		table = (volatile unsigned long *)(offset + (i << 2));
		*table = (((pt->paddr & 0xfff00000) + (i << 20)) | pt->option);
		dev_dbg("  table *%p = %p\n", table, *table);
	}
}

static void boost_on_arm(u32 mode)
{
	addr_t table_base = pinfo->map->mmu_table.base;
	struct page_table *pt;
	int nr_pt;
	int i;

	if (!pinfo->get_mmu_page_tables)
		return;

	pinfo->get_mmu_page_tables(pinfo, mode, &pt, &nr_pt);
	memset((void *)table_base, 0, pinfo->map->mmu_table.size);

	for (i=0; i<nr_pt; i++)
		mmu_create_page_table(table_base, &pt[i]);

	arch_boost_on(table_base);
}

static void boost_off_arm(void)
{
	arch_boost_off();
}
#endif

#if defined(CONFIG_ARCH_MICROBLAZE)
#include <arch/cpu.h>
static void boost_on_microblaze(u32 mode)
{
	size_t i;

	for (i = 0; i < cpu_info.icache_size; i += cpu_info.icache_line)
		__invalidate_icache(i);
	if (cpu_info.icache_size)
		__enable_icache();

	for (i = 0; i < cpu_info.dcache_size; i += cpu_info.dcache_line)
		__invalidate_dcache(i);
	if (cpu_info.dcache_size)
		__enable_icache();
}

static void boost_off_microblaze(void)
{
	if (cpu_info.icache_size)
		__disable_icache();

	if (cpu_info.dcache_size)
		__disable_icache();
}
#endif

#if defined(CONFIG_ARCH_PPC)
static void boost_on_ppc(u32 mode)
{
	__invalidate_dcache();
	__enable_dcache(0x80000000);
	__invalidate_icache();
	__enable_icache(0x80000000);
}

static void boost_off_ppc(void)
{
	__disable_dcache();
	__disable_icache();
}
#endif

static int boost_mode = BOOST_OFF;

void boost_on(u32 mode)
{
	if (boost_mode != BOOST_OFF)
		boost_off();

	boost_mode = mode;
	irq_disable_all();

#if defined(CONFIG_ARCH_ARM)
	boost_on_arm(mode);
#endif
#if defined(CONFIG_ARCH_MICROBLAZE)
	boost_on_microblaze(mode);
#endif
#if defined(CONFIG_ARCH_PPC)
	boost_on_ppc(mode);
#endif

	mmu_active = 1;
	irq_enable_all();
}

void boost_off(void)
{
	if (boost_mode == BOOST_OFF)
		return;

	mmu_active = 0;
	irq_disable_all();
#if defined(CONFIG_ARCH_ARM)
	boost_off_arm();
#endif
#if defined(CONFIG_ARCH_MICROBLAZE)
	boost_off_microblaze();
#endif
#if defined(CONFIG_ARCH_PPC)
	boost_off_ppc();
#endif

	boost_mode = BOOST_OFF;
}

void boost_on_save(u32 mode, u32 *flags)
{
	*flags = boost_mode;
	boost_on(mode);
}

void boost_off_save(u32 *flags)
{
	*flags = boost_mode;
	boost_off();
}

void boost_restore(u32 flags)
{
	boost_off();

	if (flags != BOOST_OFF)
		boost_on(flags);
}
