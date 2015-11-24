#ifndef _HERMIT_TARGET_INIT_H_
#define _HERMIT_TARGET_INIT_H_

#ifndef __ASSEMBLER__

typedef void (*initcall_t)(void);

#define initcall(level, fn)				\
	static const initcall_t __initcall_##fn		\
	__attribute__((__used__))			\
	__attribute__((__section__(".initcall" level ".init"))) = fn

#define arch_initcall(fn)	initcall("1", fn)
#define subsys_initcall(fn)	initcall("2", fn)
#define latearch_initcall(fn)	initcall("3", fn)
#define device_initcall(fn)	initcall("4", fn)
#define late_initcall(fn)	initcall("5", fn)

#endif

#define INIT_CALLS_LEVEL(level) *(.initcall##level##.init)
#define INIT_CALLS				\
	__initcall_start = .;			\
	INIT_CALLS_LEVEL(1)			\
	INIT_CALLS_LEVEL(2)			\
	INIT_CALLS_LEVEL(3)			\
	INIT_CALLS_LEVEL(4)			\
	INIT_CALLS_LEVEL(5)			\
	__initcall_end = .;			\
	LONG(0)
#endif
