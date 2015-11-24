#ifndef _BOARD_H_
#define _BOARD_H_

#include <autoconf.h>

#define FLASH_START		(0x60000000)
#define FLASH_SIZE		(0x00800000)

#define DRAM_START		(0xc0000000)

#if defined(CONFIG_SYSTEM_ENTRYPOINT_RAM)
#define DRAM_HERMIT_START	(0xc0218000)
#else
#define DRAM_HERMIT_START	(0xc5600000)
#endif
#define DRAM_HERMIT_SIZE	(0x00100000)
#define STACK_START		(STACK_END + STACK_SIZE)
#define STACK_END		(0xc5700000)
#define STACK_SIZE		(0x00100000)

#define PBDR_JP2		(0x40)

#endif
