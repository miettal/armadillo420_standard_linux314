#ifndef _HERMIT_POWERPC_CPU_H_
#define _HERMIT_POWERPC_CPU_H_

/* 
 * we need linux param (board info) struct for ppc. this is how we pass info
 * from bootloader to linux kernel in ppc arch. this structure must match
 * with bd_t in arch/ppc/platform/suzaku.h
 */
struct linux_param {
	unsigned int memsize;		/* DRAM installed, in bytes */
	unsigned char enetaddr[6];	/* FIXME: we don't need this */
	unsigned int intfreq;		/* Processor speed, in Hz */
	unsigned int busfreq;		/* Bus speed, in Hz */
	unsigned int pci_busfreq;	/* PCI Bus speed, in Hz */
};

extern struct linux_param lp;

#endif
