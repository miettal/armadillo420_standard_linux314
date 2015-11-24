#ifndef __CACHE_H__
#define __CACHE_H__

/* Define MSR enable bits for instruction and data caches */
#define ICACHE_MSR_BIT (1 << 5)
#define DCACHE_MSR_BIT (1 << 7)

#define __enable_icache()						\
	__asm__ __volatile__ ("	msrset	r0, %0;				\
				nop;"					\
				: 					\
				: "i" (ICACHE_MSR_BIT)			\
				: "memory")

#define __disable_icache()						\
	__asm__ __volatile__ ("	msrclr r0, %0;				\
				nop;"					\
				: 					\
				: "i" (ICACHE_MSR_BIT)			\
				: "memory")

#define __invalidate_icache(addr)					\
	__asm__ __volatile__ ("						\
				wic	%0, r0"				\
				:					\
				: "r" (addr))

#define __enable_dcache()						\
	__asm__ __volatile__ (" msrset	r0, %0;				\
				nop;"					\
				: 					\
				: "i" (DCACHE_MSR_BIT)			\
				: "memory")

#define __disable_dcache()						\
	__asm__ __volatile__ (" msrclr	r0, %0;				\
				nop;"					\
				: 					\
				: "i" (DCACHE_MSR_BIT)			\
				: "memory")

#define __invalidate_dcache(addr)					\
	__asm__ __volatile__ ("						\
				wdc	%0, r0"				\
				:					\
				: "r" (addr))

#endif /* __CACHE_H__ */
