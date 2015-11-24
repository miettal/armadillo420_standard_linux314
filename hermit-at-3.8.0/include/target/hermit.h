#ifndef _HERMIT_TARGET_HERMIT_H_
#define _HERMIT_TARGET_HERMIT_H_

#include <htypes.h>
#include <io.h>

/* attributes */
#if defined(DEBUG)
#define __noinline	__attribute__((__noinline__))
#else
#define __noinline
#endif
#define __weak		__attribute__((weak))
#define __noreturn	__attribute__((noreturn))
#define __used		__attribute__((__used__))
#define __maybe_unused	__attribute__((__unused__))
#define __section(S)	__attribute__((__section__(#S)))
#define __init		__section(".pre.init") __noinline
#define __initdata	__section(".pre.init.data")

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define BIT(x) (1 << x)

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

#define HERM_ERR	0
#define HERM_INFO	1
#define HERM_DEBUG	2

#if !defined(CORE_NAME)
#define CORE_NAME "hermit"
#endif

#define dev_printf(level, args...) \
	hprintf(CORE_NAME ": " args)
#define dev_printf_r(level, args...) \
	hprintf(args)

#define dev_err(args...)	dev_printf(HERM_ERR, args)
#define dev_err_r(args...)	dev_printf_r(HERM_ERR, args)
#define dev_info(args...)	dev_printf(HERM_INFO, args)
#define dev_info_r(args...)	dev_printf_r(HERM_INFO, args)

#if defined(DEBUG)
#define dev_dbg(args...)	dev_printf(HERM_DEBUG, args)
#define dev_dbg_r(args...)	dev_printf_r(HERM_DEBUG, args)
#else
#define dev_dbg(args...)	({ if (0) dev_printf(HERM_DEBUG, args); 0; })
#define dev_dbg_r(args...)	({ if (0) dev_printf_r(HERM_DEBUG, args); 0; })
#endif

#if defined(VERBOSE_DEBUG)
#define dev_vdbg(args...)	dev_printf(HERM_DEBUG, args)
#define dev_vdbg_r(args...)	dev_printf_r(HERM_DEBUG, args)
#else
#define dev_vdbg(args...)	({ if (0) dev_printf(HERM_DEBUG, args); 0; })
#define dev_vdbg_r(args...)	({ if (0) dev_printf_r(HERM_DEBUG, args); 0; })
#endif

#if defined(TRACE_DEBUG)
#define trace() \
	dev_printf(HERM_DEBUG, "%s()\n", __FUNCTION__)
#else
#define trace() \
	({ if (0) dev_printf(HERM_DEBUG, "%s()\n", __FUNCTION__); 0; })
#endif

#endif
