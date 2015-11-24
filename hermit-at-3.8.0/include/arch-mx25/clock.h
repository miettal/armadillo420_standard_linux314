#ifndef _HERMIT_ARCH_MX25_CLOCK_H_
#define _HERMIT_ARCH_MX25_CLOCK_H_

#define mx25_ahb_clock_enable(gate) mx25_per_clock_enable((gate) + 16)
extern void mx25_per_clock_enable(int gate);
extern void mx25_ipg_clock_enable(int gate);

#endif
