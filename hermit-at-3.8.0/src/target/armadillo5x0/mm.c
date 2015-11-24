#include <autoconf.h>
#include <hermit.h>
#include <platform.h>
#include <boost.h>

static struct page_table pt_list[] = {
	/* Physical,  Virtual,    Size,       Option */
	/*-------------------------------------------*/
	/* Internal Register */
	{0x43f00000, 0x43f00000, 0x3c100000, 0xc02},
	/* SDRAM */
	{0x80000000, 0x80000000, 0x08000000, 0xc0e},
	/* Flash */
	{0xa0000000, 0xa0000000, 0x02000000, 0xc02},
	/* CS3 (SMSC911x) */
	{0xb2000000, 0xb2000000, 0x02000000, 0xc02},
	/* PCMCIA */
	{0xb8000000, 0xb8000000, 0x00100000, 0xc02},
	{0xbc000000, 0xbc000000, 0x00100000, 0xc02},
};

int armadillo5x0_get_mmu_page_tables(struct platform_info *pinfo, u32 mode,
				     struct page_table **pt, int *nr_pt)
{
	*pt = (struct page_table *)&pt_list;
	*nr_pt = ARRAY_SIZE(pt_list);

	return 0;
}
