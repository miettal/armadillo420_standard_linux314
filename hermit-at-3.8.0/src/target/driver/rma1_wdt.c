#define CORE_NAME "rma1_wdt"

#include <autoconf.h>
#include <hermit.h>
#include <init.h>
#include <irq.h>
#include <arch/system.h>

void (*callback_rebooted_by_watchdog)(void) = NULL;

static int rma1_wdt_ping(int irq, void *data)
{
	/* kicking the dog */
	write16(RWTCNT0, 0x5a00);

	/* interrupt clear & setting next timeout */
	write16(TMU(0, TCR01), 0x0000);
	write32(TMU(0, TCNT01), read32(TMU(0, TCOR01)));
	write16(TMU(0, TCR01), 0x20);

	return 0;
}

void rma1_wdt_init(void)
{
	u32 usecs = 1000 * 1000; /* 1s */
	u32 count;
	u8 tstr;

	if (callback_rebooted_by_watchdog && (read8(RWTCSRA0) & 0x10))
		callback_rebooted_by_watchdog();

	request_irq(199 + 32, rma1_wdt_ping, NULL);

	/* initializing timer */
	/*** SUB = 50MHz, TCNT_CLK = 12.5MHz */
	count = (12 * usecs) + (usecs / 2) + (usecs & 1);

	write16(TMU(0, TCR01), 0x20);
	write32(TMU(0, TCOR01), count);
	write32(TMU(0, TCNT01), count);
	tstr = read8(TMU(0,TSTR));
	write8(TMU(0, TSTR), tstr | 0x2);
}

device_initcall(rma1_wdt_init);
