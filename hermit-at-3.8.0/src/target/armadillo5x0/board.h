#ifndef _HERMIT_TARGET_PLATFORM_BOARD_H_
#define _HERMIT_TARGET_PLATFORM_BOARD_H_

#ifndef __ASSEMBLER__
#include <htypes.h>
#endif

#define FLASH_START		(0xA0000000)
#define FLASH_SIZE		(0x01000000)

#define DRAM_START		(0x80000000)
#define DRAM_SIZE		(0x04000000)

#define DRAM_HERMIT_START	(0x83000000)
#define DRAM_HERMIT_SIZE	(0x00f00000)

#define STACK_START		(STACK_END + STACK_SIZE)
#define STACK_END		(0x83f00000)
#define STACK_SIZE		(0x00100000)

#define CLK_400MHZ		(400)
#define CLK_532MHZ		(532)

#define MPCTL_FPM_400		(0x073533bf)
#define MPCTL_FPM_532		(0x027123df)
#define MPCTL_FPM_DEFAULT	(MPCTL_FPM_400)
#define MPCTL_CKIH_400		(0x000c1c09)
#define MPCTL_CKIH_532		(0x0026280a)
#define MPCTL_CKIH_DEFAULT	(MPCTL_CKIH_400)

/* USB PLL = 240000000 Hz */
#define UPCTL_FPM_240		(0x0af32f33)
#define UPCTL_CKIH_240		(0x000c17fb)

/* Serial PLL = 294912000 Hz */
#define SPCTL_FPM_294		(0x047f2065)
#define SPCTL_CKIH_294		(0x07dc2d53)

/* PDR0: Post Divider Register 0 */
/* 400MHz@0xff831650 MCU:1, MAX:3, IPG:2, NFC:7, HSP:3, PER:4, CSI:512
 * 532MHz@0xff831e58 MCU:1, MAX:4, IPG:2, NFC:7, HSP:3, PER:4, CSI:512
 */
#define PDR0_400MHZ		(0xff831650)
#define PDR0_532MHZ		(0xff831658)
#define PDR0_DEFAULT		(PDR0_400MHZ)

/* PDR1: Post Divider Register 1 (POST,PRE) */
/* ---MHz@0x49FCFE7F SSI1:12,1 SSI2:12,1 FIRI:64,2 USB:2,2
 */
#define PDR1_DEFAULT		(0x49fc160b)

#ifndef __ASSEMBLER__

struct board_private {
	u32 cpu_board_id;
	/* r201 ... i.MX31 CPU Revision 2.0.1 or lator */
#define DRAM_ID_32Mx2		0
#define DRAM_ID_64Mx2		1
#define DRAM_ID_128Mx2		2
#define DRAM_ID_RSVD_3		3 /* reserved */
#define DRAM_ID_32Mx2_r201	4
#define DRAM_ID_64Mx2_r201	5
#define DRAM_ID_RSVD_6		6 /* reserved */
#define DRAM_ID_RSVD_7		7 /* reserved */

	u32 base_board_id;
	u8 jumper1;
	u8 jumper2;
};

#if defined(CONFIG_PLATFORM_ARMADILLO500_DEV)
# include "board-dev.h"
#endif
#if defined(CONFIG_PLATFORM_ARMADILLO500_FX)
# include "board-fx.h"
#endif
#if defined(CONFIG_PLATFORM_ARMADILLO500_ZERO)
# include "board-zero.h"
#endif

#define TARGET_NAME_LEN 256
extern char target_name[TARGET_NAME_LEN];
extern char *target_profile;

extern int change_clk(u32 clk, u32 clkss);

extern int change_serial_channel(unsigned int ch);

#endif
#endif
