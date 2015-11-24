#define CORE_NAME "mx25_wdt"

#include <autoconf.h>
#include <hermit.h>
#include <arch/system.h>
#include <arch/clock.h>
#include <irq.h>
#include <init.h>

#define IRQ_EPIT2 27

static int mx25_wdt_ping(int irq, void *data)
{
	u32 epit = EPIT2_BASE_ADDR;
	u32 wdog = WDOG_BASE_ADDR;

	trace();

	write16(wdog + WDOG(WSR), 0x5555);
	write16(wdog + WDOG(WSR), 0xAAAA);

	write32(epit + EPIT(SR), 0x01);
	write32(epit + EPIT(LR), 0xffffffff);

	return 0;
}

void mx25_wdt_init(void)
{
	u32 base = EPIT2_BASE_ADDR;

	trace();

	mx25_per_clock_enable(1);
	mx25_ipg_clock_enable(11);

	write32(base + EPIT(CR), 0x0102041a);
	write32(base + EPIT(SR), 0x01);
	write32(base + EPIT(CMPR), 0xffffffff - 1000000); /* 1s timer */

	write32(base + EPIT(CR), 0x0102041f);

	request_irq(IRQ_EPIT2, mx25_wdt_ping, NULL);
}

device_initcall(mx25_wdt_init);
