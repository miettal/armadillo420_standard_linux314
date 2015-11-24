/*
 * Initial C entry point of the boot loader.  Called by the assembly
 * language setup code in boot.S/init.S.
 *
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 *
 * We must have a stack pointer, but there are no other preconditions.
 *
 * The generated text for this file lives in the .hermit.boot section
 * of the loader executable.  Symbols in the .hermit.boot section have
 * relocation addresses equal to their load addresses (see the link
 * map for details), so this section is designed to run from flash at
 * boot time.  In GNU linker terminology, these symbols have VMA ==
 * LMA ('info ld' for definitions).
 *
 * Pretty much all other symbols in the loader have VMAs within SRAM
 * or DRAM (depending on build configuration).  This means that their
 * load addresses (LMAs) are not equal to their VMAs.  Ergo most of
 * the loader must be relocated to RAM before it can run; we even
 * relocate the text segment to RAM, to make it possible to reprogram
 * the loader in flash.  This file handles the relocation.
 *
 * Until the relocation is done, the loader won't be able to access
 * any writeable data (.data segment stuff) or use zeroed global
 * memory (.bss).  Be careful when editing the code here!
 *
 * --miket
 */

#include <autoconf.h>

extern unsigned char __bss_start;
extern unsigned char __bss_end;

#if defined(CONFIG_SYSTEM_NEED_RELOCATE)

#include <htypes.h>

/* see loader.lds */
extern unsigned char __entry_start_src;
extern unsigned char __entry_start;
extern unsigned char __entry_end;
extern unsigned char __text_start_src;
extern unsigned char __text_start;
extern unsigned char __text_end;
extern unsigned char __data_start_src;
extern unsigned char __data_start;
extern unsigned char __data_end;

extern int hmain(void);

typedef void (*entry_t)(void);

void entry(addr_t) __attribute__((__section__(".pre.entry")));
void entry(addr_t entry_point)
{
	unsigned char *src, *dst;

	/* relocate .entry */
	src = &__entry_start_src;
	dst = &__entry_start;
	while (dst < &__entry_end)
		*dst++ = *src++;

#if defined(CONFIG_ARCH_ARM)
	{
		/*
		 * As Thumb inter-working is enable, when jumping from
		 * NOR-flash to DRAM the BLX instruction is used, and
		 * therefore in order to jump to a thumb-function we must
		 * use ADDRESS + 1.
		 */
		entry_t entry_func = (entry_t)(entry_point + 1);

		/* branch into relocated code */
		entry_func();
	}
#endif
}

void entry_post(void) __attribute__((__section__(".post.entry")));
void entry_post(void)
{
	unsigned char *src, *dst;

	/* relocate .text */
	src = &__text_start_src;
	dst = &__text_start;
	while (dst < &__text_end)
		*dst++ = *src++;

	/* relocate .data */
	src = &__data_start_src;
	dst = &__data_start;
	while (dst < &__data_end)
		*dst++ = *src++;

	/* clear .bss */
	dst = &__bss_start;
	while (dst < &__bss_end)
		*dst++ = 0;

	hmain();
}

#else /* CONFIG_SYSTEM_NEED_RELOCATE */

/* see loader.c */
extern void hmain(void);

void
entry(void)
{
	unsigned char *dst;

	/* clear .bss */
	dst = &__bss_start;
	while (dst < &__bss_end)
		*dst++ = 0;

	hmain();
}

#endif
