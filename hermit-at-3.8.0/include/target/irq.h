#ifndef _HERMIT_TARGET_IRQ_H_
#define _HERMIT_TARGET_IRQ_H_

#include <autoconf.h>

extern struct irq_ops *irq_ops;

typedef int (*irq_handler_t)(int, void *);

struct irq_ops {
	void (*request_irq)(int irq);
	void (*free_irq)(int irq);
	void (*start_all)(void);
	void (*stop_all)(void);
	int (*get_pendding_irq)(void);
	void (*ack)(int irq);
	void (*mask)(int irq);
	void (*unmask)(int irq);
};

struct irq_desc {
	int irq;
	irq_handler_t handler;
	void *data;
};

#if defined(CONFIG_IRQ)
extern int request_irq(int irq, irq_handler_t handler, void *data);
extern void free_irq(int irq);
extern void irq_enable_all(void);
extern void irq_disable_all(void);
#else
#define request_irq(irq, handler, data)
#define free_irq(irq)
#define irq_enable_all()
#define irq_disable_all()
#endif
#endif
