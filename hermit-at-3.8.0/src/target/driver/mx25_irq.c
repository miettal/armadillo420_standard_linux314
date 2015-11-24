#define CORE_NAME "mx25_irq"

#include <platform.h>
#include <hermit.h>
#include <herrno.h>
#include <boost.h>
#include <arch/system.h>

#include <mx25_irq.h>
#include <irq.h>

extern unsigned char __vector_start, __vector_end;

static int base = AVIC_BASE_ADDR;

static void mx25_avic_request_irq(int irq)
{
	write32(base + AVIC(INTENNUM), irq);
}

static void mx25_avic_free_irq(int irq)
{
	write32(base + AVIC(INTDISNUM), irq);
}

static void mx25_avic_start_all(void)
{
	write32(base + AVIC(INTCNTL), 0x00000000); /* all enable */
}

static void mx25_avic_stop_all(void)
{
	write32(base + AVIC(INTCNTL), 0x00400000); /* all disable */
}

static int mx25_avic_get_pendding_irq(void)
{
	int i;
	unsigned int irqh, irql;

	irqh = read32(base + AVIC(NIPNDH));
	irql = read32(base + AVIC(NIPNDL));

	for (i=0; i<32; i++)
		if (irql & (1<<i))
			return i;
	for (i=0; i<32; i++)
		if (irqh & (1<<i))
			return i + 32;

	return -1;
}

static struct irq_ops mx25_avic_ops = {
	.request_irq = mx25_avic_request_irq,
	.free_irq = mx25_avic_free_irq,
	.start_all = mx25_avic_start_all,
	.stop_all = mx25_avic_stop_all,
	.get_pendding_irq = mx25_avic_get_pendding_irq,
};

void mx25_irq_init(void)
{
	u8 *src, *dst, *end;

	boost_on(BOOST_LINUX_MODE);

	dev_dbg("VECTOR: %p - %p\n", &__vector_start, &__vector_end);

	src = &__vector_start;
	dst = (u8 *)0xffff0000;
	end = (u8 *)(0xffff0000 + (&__vector_end - &__vector_start));

	while (dst < end)
		*dst++ = *src++;

	irq_ops = &mx25_avic_ops;
}
