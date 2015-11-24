#define CORE_NAME "rma1_irq"

#include <platform.h>
#include <hermit.h>
#include <herrno.h>
#include <boost.h>
#include <arch/system.h>

#include <irq.h>

#define INTA_CTRL			(0xe605807c)

#define GIC_CPU_BASE			(0xc2000000)
#define GIC_CPU_CTRL			(GIC_CPU_BASE + 0x00)
#define GIC_CPU_PRIMASK			(GIC_CPU_BASE + 0x04)
#define GIC_CPU_BINPOINT		(GIC_CPU_BASE + 0x08)
#define GIC_CPU_INTACK			(GIC_CPU_BASE + 0x0c)
#define GIC_CPU_EOI			(GIC_CPU_BASE + 0x10)
#define GIC_CPU_RUNNINGPRI		(GIC_CPU_BASE + 0x14)
#define GIC_CPU_HIGHPRI			(GIC_CPU_BASE + 0x18)

#define GIC_DIST_BASE			(0xc2800000)
#define GIC_DIST_CTRL			(GIC_DIST_BASE + 0x000)
#define GIC_DIST_CTR			(GIC_DIST_BASE + 0x004)
#define GIC_DIST_ENABLE_SET		(GIC_DIST_BASE + 0x100)
#define GIC_DIST_ENABLE_CLEAR		(GIC_DIST_BASE + 0x180)
#define GIC_DIST_PENDING_SET		(GIC_DIST_BASE + 0x200)
#define GIC_DIST_PENDING_CLEAR		(GIC_DIST_BASE + 0x280)
#define GIC_DIST_ACTIVE_BIT		(GIC_DIST_BASE + 0x300)
#define GIC_DIST_PRI			(GIC_DIST_BASE + 0x400)
#define GIC_DIST_TARGET			(GIC_DIST_BASE + 0x800)
#define GIC_DIST_CONFIG			(GIC_DIST_BASE + 0xc00)
#define GIC_DIST_SOFTINT		(GIC_DIST_BASE + 0xf00)

extern unsigned char __vector_start, __vector_end;

static void rma1_gic_ack(int irq)
{
	//u32 mask = 1 << (irq % 32);
	//u32 status = read32(GIC_DIST_PENDING_SET + irq / 32 * 4);
	//hprintf("%s(): %d\n", __func__, irq);
	write32(GIC_CPU_EOI, irq);
	//write32(GIC_DIST_PENDING_SET + irq / 32 * 4, status & ~mask);
}

static void rma1_gic_mask(int irq)
{
	u32 mask = 1 << (irq % 32);
	//hprintf("%s(): %d\n", __func__, irq);
	write32(GIC_DIST_ENABLE_CLEAR + irq / 32 * 4, mask);
}

static void rma1_gic_unmask(int irq)
{
	u32 mask = 1 << (irq % 32);
	//hprintf("%s(): %d\n", __func__, irq);
	write32(GIC_DIST_ENABLE_SET + irq / 32 * 4, mask);
}

#if 0
static void rma1_gic_request_irq(int irq)
{
	//u32 mask = 1 << (irq % 32);
	//hprintf("%s(): %d\n", __func__, irq);
	/* unmask */
	//write32(GIC_DIST_ENABLE_SET + irq / 32 * 4, mask);
	//rma1_gic_unmask(irq);
}

static void rma1_gic_free_irq(int irq)
{
	u32 mask = 1 << (irq % 32);
	hprintf("%s(): %d\n", __func__, irq);
	/* mask */
	write32(GIC_DIST_ENABLE_CLEAR + irq / 32 * 4, mask);
}
#endif

static void rma1_gic_start_all(void)
{
	//hprintf("%s():\n", __func__);
	write32(GIC_DIST_CTRL, 1);
	write32(GIC_CPU_CTRL, 1);
}

static void rma1_gic_stop_all(void)
{
	//hprintf("%s():\n", __func__);
	write32(GIC_DIST_CTRL, 0);
	write32(GIC_CPU_CTRL, 0);
}

static int rma1_gic_get_pendding_irq(void)
{
	//u32 status;
	//int i;
	//int irq = 199;
	//unsigned int gic_irqs = 321;
	//write32(GIC_DIST_CTRL, 0);
	//gic_irqs = read32(GIC_DIST_CTR) & 0x1f;
	//gic_irqs = 321;//(gic_irqs + 1) * 32;

#if 1
	u32 status = read32(GIC_CPU_INTACK);
	int irq = status & 0x3ff;
	//hprintf("INTACK: %p\n", status);
	irq = (irq == 0x3ff ? -1 : irq);
	//hprintf("%s(): %d\n", __func__, irq);
#endif

	//status = read32(GIC_CPU_HIGHPRI);
	//hprintf("HIGHPRI: %d\n", status);

#if 0
	for (i = 32; i < gic_irqs; i += 32) {
		status = read32(GIC_DIST_PENDING_SET + i * 4 / 32);
		hprintf("PENDING_SET: i=%d: %p\n", i, status);
	}
#if 1
	for (i = 32; i < gic_irqs; i += 32) {
		status = read32(GIC_DIST_ACTIVE_BIT + i * 4 / 32);
		hprintf("ACTIVE_BIT: i=%d: %p\n", i, status);
	}
#endif
#endif
	return irq;
}

static struct irq_ops rma1_gic_ops = {
	.request_irq = rma1_gic_unmask,
	.free_irq = rma1_gic_mask,
	.start_all = rma1_gic_start_all,
	.stop_all = rma1_gic_stop_all,
	.get_pendding_irq = rma1_gic_get_pendding_irq,
	.ack = rma1_gic_ack,
	.mask = rma1_gic_mask,
	.unmask = rma1_gic_unmask,
};

void rma1_irq_init(void)
{
	u8 *src, *dst, *end;
	unsigned int gic_irqs;
	int i;

	boost_on(BOOST_LINUX_MODE);

	dev_dbg("VECTOR: %p - %p\n", &__vector_start, &__vector_end);

	/* vector relocate */
	src = &__vector_start;
	dst = (u8 *)0xffff0000;
	end = (u8 *)(0xffff0000 + (&__vector_end - &__vector_start));

	while (dst < end)
		*dst++ = *src++;

	/* register irq_ops */
	irq_ops = &rma1_gic_ops;

	/* Change INT_SEL INTCA->GIC */
	write32(INTA_CTRL, read32(INTA_CTRL) & ~0x2);

	/* gic_dist_init */
	write32(GIC_DIST_CTRL, 0);
	gic_irqs = read32(GIC_DIST_CTR) & 0x1f;
	gic_irqs = (gic_irqs + 1) * 32;

	/*** Set all global interrupts to be level triggered, active low. */
	for (i = 32; i < gic_irqs; i += 16)
		write32(GIC_DIST_CONFIG + i * 4 / 16, 0);

	/*** Set all global interrupts to this CPU only. */
	for (i = 32; i < gic_irqs; i += 4)
		write32(GIC_DIST_TARGET + i * 4 / 4, 0x01010101);

	/*** Set priority on all global interrupts. */
	for (i = 32; i < gic_irqs; i += 4)
		write32(GIC_DIST_PRI + i * 4 / 4, 0xa0a0a0a0);

	/*** Disable all interrupts. */
	for (i = 32; i < gic_irqs; i += 32)
		write32(GIC_DIST_ENABLE_CLEAR + i * 4 / 32, 0xffffffff);

	write32(GIC_DIST_CTRL, 1);

	/* gic_cpu_init */
	write32(GIC_DIST_ENABLE_CLEAR, 0xffff0000);
	write32(GIC_DIST_ENABLE_SET, 0x0000ffff);

	/*** Set priority on PPI and SGI interrupts */
	for (i = 0; i < 32; i += 4)
		write32(GIC_DIST_PRI + i * 4 / 4, 0xa0a0a0a0);

	write32(GIC_CPU_PRIMASK, 0xf0);
	write32(GIC_CPU_CTRL, 1);
}
