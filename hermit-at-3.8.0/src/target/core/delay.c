#include <autoconf.h>
#include <platform.h>
#include <delay.h>

static struct platform_info *pinfo = &platform_info;

void udelay(u32 usecs)
{
	if (pinfo->udelay)
		pinfo->udelay(pinfo, usecs);
}

void mdelay(u32 msecs)
{
	if (pinfo->mdelay)
		pinfo->mdelay(pinfo, msecs);
	else {
		while (msecs--)
			udelay(1000);
	}
}
