#include <platform.h>
#include <hermit.h>
#include <herrno.h>
#include <boost.h>

struct page_table pt_list_std[] = {
	/* Physical,  Virtual,    Size,       Option */
	/*-------------------------------------------*/
	/* SDRAM */
	{0xc0000000, 0xc0000000, 0x10000000, 0xc0e},
	/* Flash */
	{0x60000000, 0x60000000, 0x10000000, 0xc02},
	/* Internal Register */
	{0x80000000, 0x80000000, 0x10000000, 0xc02},
};

struct page_table pt_list_eth[] = {
	/* Physical,  Virtual,    Size,       Option */
	/*-------------------------------------------*/
	/* SDRAM */
	{0xc0000000, 0xc0000000, 0x02000000, 0xc0e},
	{0xc4800000, 0xc4800000, 0x00800000, 0xc02},
	{0xc5000000, 0xc5000000, 0x01000000, 0xc0e},
	/* Flash */
	{0x60000000, 0x60000000, 0x10000000, 0xc02},
	/* Internal Register */
	{0x80000000, 0x80000000, 0x10000000, 0xc02},
};

int armadillo2x0_get_mmu_page_tables(struct platform_info *pinfo, u32 mode,
				     struct page_table **pt, int *nr_pt)
{
	switch (mode) {
	case BOOST_ETH_MODE:
		*pt = (struct page_table *)&pt_list_eth;
		*nr_pt = ARRAY_SIZE(pt_list_eth);
		break;
	case BOOST_LINUX_MODE:
		*pt = (struct page_table *)&pt_list_std;
		*nr_pt = ARRAY_SIZE(pt_list_std);
		break;
	default:
		return -H_EINVAL;
	}
	return 0;
}
