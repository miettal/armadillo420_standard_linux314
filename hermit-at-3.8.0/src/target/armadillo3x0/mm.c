#include <autoconf.h>
#include <hermit.h>
#include <platform.h>
#include <herrno.h>
#include <boost.h>

static struct page_table pt_list_std[] = {
	/* Physical,  Virtual,    Size,       Option */
	/*-------------------------------------------*/
	/* Internal Register */
	{0x80000000, 0x80000000, 0x30000000, 0xc02},
	/* SDRAM */
	{0x00000000, 0x00000000, 0x10000000, 0xc0e},
	/* Flash */
	{0x50000000, 0x50000000, 0x10000000, 0xc0e},
	/* CPLD */
	{0x60000000, 0x60000000, 0x10000000, 0xc02},
};

static struct page_table pt_list_eth[] = {
	/* Physical,  Virtual,    Size,       Option */
	/*-------------------------------------------*/
	/* Internal Register */
	{0x80000000, 0x80000000, 0x30000000, 0xc02},
	/* SDRAM */
	{0x00000000, 0x00000000, 0x00800000, 0xc0e},
	{0x00800000, 0x00800000, 0x01000000, 0xc02},
	{0x01800000, 0x01800000, 0x0e800000, 0xc0e},
	/* Flash */
	{0x50000000, 0x50000000, 0x10000000, 0xc02},
	/* CPLD */
	{0x60000000, 0x60000000, 0x10000000, 0xc02},
};

int armadillo3x0_get_mmu_page_tables(struct platform_info *pinfo, u32 mode,
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
