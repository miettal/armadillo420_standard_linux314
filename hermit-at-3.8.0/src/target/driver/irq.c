#define CORE_NAME "irq"

#include <hermit.h>
#include <herrno.h>
#include <htypes.h>
#include <init.h>
#include <string.h>
#include <irq.h>

#define NR_IRQS		(64)
#define __IRQ_UNDEFINED	(-1)

struct irq_ops *irq_ops;

static struct irq_desc irq_desc[NR_IRQS];

static struct irq_desc *irq_to_desc(int irq)
{
	int i;
	for (i = 0; i < NR_IRQS; i++)
		if (irq_desc[i].irq == irq)
			return &irq_desc[i];

	return NULL;
}

#define new() irq_to_desc(__IRQ_UNDEFINED)

int request_irq(int irq, irq_handler_t handler, void *data)
{
	struct irq_desc *desc = irq_to_desc(irq);

	if (desc) {
		dev_err("irq:%d was already requested!\n", irq);
		return -H_EINVAL;
	}

	desc = new();
	desc->irq = irq;
	desc->handler = handler;
	desc->data = data;

	irq_ops->request_irq(irq);

	return 0;
}

void free_irq(int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);

	irq_ops->free_irq(irq);

	if (desc) {
		memset(desc, 0, sizeof(struct irq_desc));
		desc->irq = __IRQ_UNDEFINED;
	}
}

void irq_handler(void)
{
	struct irq_desc *desc;
	int irq;

	irq = irq_ops->get_pendding_irq();
	if (irq_ops->mask)
		irq_ops->mask(irq);

	dev_dbg("IRQ[%d]:\n", irq);
	desc = irq_to_desc(irq);
	if (desc && desc->handler) {
		desc->handler(irq, desc->data);
		if (irq_ops->ack)
			irq_ops->ack(irq);
	} else
		dev_err("unknown irq!!\n");

	if (irq_ops->unmask)
		irq_ops->unmask(irq);
}

void common_handler(void)
{
	dev_err("no support exp\n");
}

void irq_enable_all(void)
{
	if (irq_ops)
		irq_ops->start_all();
}

void irq_disable_all(void)
{
	if (irq_ops)
		irq_ops->stop_all();
}

static void irq_init(void)
{
	int i;
	for (i = 0; i < NR_IRQS; i++)
		irq_desc[i].irq = __IRQ_UNDEFINED;
}
subsys_initcall(irq_init);
