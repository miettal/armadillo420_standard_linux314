#include <autoconf.h>
#include <platform.h>
#include <led.h>

static struct platform_info *pinfo = &platform_info;

void led_on(u32 leds)
{
	if (pinfo->led_on)
		pinfo->led_on(pinfo, leds);
}

void led_off(u32 leds)
{
	if (pinfo->led_off)
		pinfo->led_off(pinfo, leds);
}
