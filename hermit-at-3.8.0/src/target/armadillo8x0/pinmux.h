#ifndef _HERMIT_TARGET_PLATFORM_PIMUX_H_
#define _HERMIT_TARGET_PLATFORM_PIMUX_H_

#include <autoconf.h>
#include <arch/pinmux.h>

#if defined(CONFIG_PLATFORM_ARMADILLO800EVA)
#include <platform/pinmux-armadillo800eva.h>
#elif defined(CONFIG_PLATFORM_ARMADILLO810)
#include <platform/pinmux-armadillo810.h>
#elif defined(CONFIG_PLATFORM_ARMADILLO840)
#include <platform/pinmux-armadillo840.h>
#endif

#endif
