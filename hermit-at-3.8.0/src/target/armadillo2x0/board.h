#ifndef _BOARD_H_
#define _BOARD_H_

#include <autoconf.h>

#ifndef __ASSEMBLER__
#include <arch/ioregs.h>
#include <htypes.h>
#endif

#define JP_MODE_BIT 0x80

#define LED_GREEN_ON()  ((!get_board_id()) \
			 ?({write32(GPIO_PEDDR, read32(GPIO_PEDDR) | 0x02); \
			    write32(GPIO_PEDR, read32(GPIO_PEDR) | 0x02);}) \
			 :({write32(GPIO_PEDDR, read32(GPIO_PEDDR) | 0x01); \
			    write32(GPIO_PEDR, read32(GPIO_PEDR) | 0x01);}))
#define LED_GREEN_OFF() ((!get_board_id()) \
			 ?({write32(GPIO_PEDDR, read32(GPIO_PEDDR) | 0x02); \
			    write32(GPIO_PEDR, read32(GPIO_PEDR) & ~0x02);}) \
			 :({write32(GPIO_PEDDR, read32(GPIO_PEDDR) | 0x01); \
			    write32(GPIO_PEDR, read32(GPIO_PEDR) & ~0x01);}))
#define LED_RED_ON()    ((!get_board_id()) \
			 ?({write32(GPIO_PEDDR, read32(GPIO_PEDDR) | 0x01); \
			    write32(GPIO_PEDR, read32(GPIO_PEDR) | 0x01);}) \
			 :({write32(GPIO_PCDDR, read32(GPIO_PCDDR) | 0x80); \
			    write32(GPIO_PCDR, read32(GPIO_PCDR) | 0x80);}))
#define LED_RED_OFF()   ((!get_board_id()) \
			 ?({write32(GPIO_PEDDR, read32(GPIO_PEDDR) | 0x01); \
			    write32(GPIO_PEDR, read32(GPIO_PEDR) & ~0x01);}) \
			 :({write32(GPIO_PCDDR, read32(GPIO_PCDDR) | 0x80); \
			    write32(GPIO_PCDR, read32(GPIO_PCDR) & ~0x80);}))

#define FLASH_START		(0x60000000)
#define FLASH_SIZE		(0x00800000)

#define SRAM_START		(0x80014000)
#define SRAM_SIZE		(0x00001100)

#define DRAM_START		(0xC0000000)
#define DRAM_SIZE		(0x04000000)

#if defined(CONFIG_SYSTEM_ENTRYPOINT_RAM)
#define DRAM_HERMIT_START	(0xC0218000)
#else
#define DRAM_HERMIT_START	(0xC5600000)
#endif
#define DRAM_HERMIT_SIZE	(0x00100000)

#define STACK_START		(STACK_END + STACK_SIZE)
#define STACK_END		(0xc5700000)
#define STACK_SIZE		(0x00100000)

#ifndef __ASSEMBLER__

struct board_private {
	u8 board_id;
#define BOARD_ID_UNKONWN -1
#define BOARD_ID_A210 0
#define BOARD_ID_A220 1
#define BOARD_ID_A230 2
#define BOARD_ID_A240 3
#define BOARD_ID_A210C 4

	u8 dram_id;
#define DRAM_ID_NORMAL 0
#define DRAM_ID_MULTI 1

	u8 jumper;
	u8 tactsw;
};

extern int jumper_mode;
void udelay(unsigned long usec);
void mdelay(unsigned long msec);

#define get_board_id() ((read32(GPIO_PHDR) >> 2) & 0x7)
#ifdef BOOTMODE
#define get_board_sdram32bit() 0
#else
#define get_board_sdram32bit() ((read32(GPIO_PHDR) >> 5) & 0x1)
#endif
extern char *get_board_name(int id);
extern void board_init(void);
extern int check_mode_jumper(void);

#include <command.h>
extern const command_t info_command;
#endif
#endif
