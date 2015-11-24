#include <platform.h>

static struct platform_info *pinfo = &platform_info;

void get_mac_address(u8 *addr)
{
	if (pinfo->get_mac_address)
		pinfo->get_mac_address(pinfo, addr);
}
