#ifndef __CACHE_H__
#define __CACHE_H__

#define XREG_SPR_ICCR	0x3fb
#define XREG_SPR_DCCR	0x3fa
#define XREG_SPR_DCWR	0x3ba
#define XREG_SPR_SGR 	0x3b9
#define XREG_SPR_CCR0	0x3b3

#define DCWR_VAL	0x0
#define SGR_VAL		0x0

#define DCACHE_SIZE	(16 * 1024)

#define stringify(s)	tostring(s)
#define tostring(s)	#s

#define mtspr(rn, v)	__asm__ __volatile__(				\
				"mtspr	" stringify(rn) ", %0\n"	\
				: : "r" (v)				\
			)
#define mfspr(rn)	({						\
				unsigned int rval;			\
				__asm__ __volatile__(			\
					"mfspr	%0, " stringify(rn) "\n"\
					: "=r" (rval)			\
				);					\
				rval;					\
			})
#define iccci		__asm__ __volatile__(	"iccci	0, 0\n")
#define isync		__asm__ __volatile__(	"isync\n")
#define dccci(adr)	__asm__ __volatile__(	"dccci	0, %0\n" : : "r" (adr))
#define dcbf(adr)	__asm__ __volatile__(	"dcbf	0, %0\n" : : "r" (adr))
#define dcbi(adr)	__asm__ __volatile__(	"dcbi	0, %0\n" : : "r" (adr))
#define dcread(adr)	({						\
				register unsigned int rval;		\
				__asm__ __volatile__(			\
					"dcread	%0, 0, %1\n"		\
					: "=r" (rval) : "r" (adr)	\
				);					\
				rval;					\
			})
#define sync		__asm__ __volatile__("sync\n")

static inline void __enable_dcache(unsigned int regions)
{
	mtspr(XREG_SPR_SGR,  SGR_VAL);
	mtspr(XREG_SPR_DCWR, DCWR_VAL);
	mtspr(XREG_SPR_DCCR, regions);
	isync;
}

static inline void __disable_dcache(void)
{
	register unsigned int address0;
	register unsigned int ccr0;
	register unsigned int tag;
	register unsigned int i;
	unsigned int dccr;

	dccr = mfspr(XREG_SPR_DCCR);
	if (dccr != 0) {
		ccr0 = mfspr(XREG_SPR_CCR0);
		ccr0 = (ccr0 & 0xfffffffe) | 0x10;
		i = 0;
		while (i < 2) {
			mtspr(XREG_SPR_CCR0, ccr0);
			address0 = 0;
			while (address0 < (DCACHE_SIZE / 2)) {
				tag = dcread(address0);
				if ((tag & 0x30) == 0x30)
					dcbf((tag & 0xfffff000) | address0);
				address0 += 32;
			}
			ccr0 |= 1;
			i++;
		}
		sync;
		mtspr(XREG_SPR_DCCR, 0);
		isync;
	}
	else {
		address0 = 0;
		while (address0 < DCACHE_SIZE / 2) {
			dccci(address0);
			address0 += 32;
		}
	}
}

static inline void __invalidate_dcache_line(unsigned int adr)
{
	dcbi(adr);
	sync;
	isync;
}

static inline void __invalidate_dcache(void)
{
	register unsigned int address0;

	for (address0 = 0; address0 < (DCACHE_SIZE / 2); address0 += 32)
		__invalidate_dcache_line(address0);
}

static inline void __enable_icache(unsigned int regions)
{
	mtspr(XREG_SPR_SGR,  SGR_VAL);
	iccci;
	mtspr(XREG_SPR_ICCR, regions);
	isync;
}

static inline void __disable_icache(void)
{
	iccci;
	mtspr(XREG_SPR_ICCR, 0);
	isync;
}

static inline void __invalidate_icache(void)
{
	iccci;
	isync;
}

#endif /* __CACHE_H__ */
