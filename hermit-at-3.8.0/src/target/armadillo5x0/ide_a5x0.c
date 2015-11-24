/*
 * Copyright (c) 2007 Atmark Techno, Inc.  All Rights Reserved.
 */

#define CORE_NAME "ide_a5x0"

#include <autoconf.h>
#if defined(CONFIG_COMPACTFLASH_DEBUG)
#define DEBUG
#endif
#include <hermit.h>
#include <platform.h>
#include <herrno.h>
#include <htypes.h>
#include <io.h>
#include <buffer.h>
#include <gunzip.h>
#include <string.h>
#include "board.h"
#include <pcmcia.h>
#include <ide.h>
#include <boost.h>
#include <command/linux.h>
#include <map.h>
#include <delay.h>
#include <arch/ioregs.h>

static void
pcmcia_a5x0_mux_init(void)
{
	trace();
	mxc_set_mux(MUX_PIN(SDBA0),     MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(SDBA1),     MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(PC_CD1),    MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(PC_CD2),    MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(PC_WAIT),   MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(PC_READY),  MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(PC_PWRON),  MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(PC_VS1),    MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(PC_VS2),    MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(PC_BVD1),   MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(PC_BVD2),   MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(PC_RST),    MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(IOIS16),    MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(PC_RW),     MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(PC_POE),    MUX_O_FUNC | MUX_I_FUNC);
	mxc_set_mux(MUX_PIN(xCF_PWREN), MUX_O_GPIO | MUX_I_GPIO);

	mxc_set_gpio(GPIO_PIN(xCF_PWREN), GPIO_OUTPUT, GPIO_HIGH);
}

static void
pcmcia_a5x0_power_on(void)
{
	u32 pipr;
	int i;
	trace();

	mxc_set_gpio(GPIO_PIN(xCF_PWREN), GPIO_OUTPUT, GPIO_LOW);
	for (i=0; i<100; i++) {
		pipr = read32(PCMCIA_IF_BASE_ADDR + PIPR);
		if (pipr & 0x100)
			break;
		mdelay(10);
	}
	mdelay(500);
}

static void
pcmcia_a5x0_power_off(void)
{
	trace();
	mxc_set_gpio(GPIO_PIN(xCF_PWREN), GPIO_OUTPUT, GPIO_HIGH);
}

static int
pcmcia_a5x0_reset(void)
{
	trace();
	write32(PCMCIA_IF_BASE_ADDR + PGCR,
		read32(PCMCIA_IF_BASE_ADDR + PGCR) | PGCR_RESET);
	mdelay(2);

	write32(PCMCIA_IF_BASE_ADDR + PGCR,
		read32(PCMCIA_IF_BASE_ADDR + PGCR) & ~(PGCR_RESET | PGCR_LPMEN));
	write32(PCMCIA_IF_BASE_ADDR + PGCR,
		read32(PCMCIA_IF_BASE_ADDR + PGCR) | PGCR_POE);
	mdelay(2);

	return 0;
}

static int
pcmcia_a5x0_bank_init(void)
{
	trace();

	write32(PCMCIA_IF_BASE_ADDR + POR0,
		read32(PCMCIA_IF_BASE_ADDR + POR0) & ~POR_PV);
	write32(PCMCIA_IF_BASE_ADDR + PBR0, PCMCIA_MEM_BASE_ADDR);
	write32(PCMCIA_IF_BASE_ADDR + POR0, POR_PV | 0x11c);
	return 0;
}

static int
pcmcia_a5x0_hw_init(pcmcia_info_t *info)
{
	trace();

	pcmcia_a5x0_mux_init();
	pcmcia_a5x0_power_off();
	pcmcia_a5x0_reset();
	pcmcia_a5x0_bank_init();

	info->mem_base = PCMCIA_MEM_BASE_ADDR;
	info->attr_base = PCMCIA_MEM_BASE_ADDR;
	info->io_base = PCMCIA_MEM_BASE_ADDR;

	return 0;
}

static int
pcmcia_a5x0_hw_free(void)
{
	trace();
	pcmcia_a5x0_power_off();  
	return 0;
}

static int
pcmcia_a5x0_set_socket(int power)
{
	trace();
	return 0;
}

static int
pcmcia_a5x0_get_status(void)
{
	u32 pipr;
	trace();
	pipr = read32(PCMCIA_IF_BASE_ADDR + PIPR);

	if (!(pipr & 0x18))
		pcmcia_a5x0_power_on();
	else {
		pcmcia_a5x0_power_off();
		return -H_EIO;
	}

	pipr = read32(PCMCIA_IF_BASE_ADDR + PIPR);
	if (!(pipr & 0x100))
		return -H_EIO;

	return 0;
}

static int
pcmcia_a5x0_set_mem_map(int map)
{
	trace();
	if (map == 1/*ATTR*/) {
		write32(PCMCIA_IF_BASE_ADDR + POR0,
			read32(PCMCIA_IF_BASE_ADDR + POR0) & ~POR_PV);
		write32(PCMCIA_IF_BASE_ADDR + PBR0, PCMCIA_MEM_BASE_ADDR);
		write32(PCMCIA_IF_BASE_ADDR + POR0,
			read32(PCMCIA_IF_BASE_ADDR + POR0) &
			~(0x6000000 | 0xffffff));
		write32(PCMCIA_IF_BASE_ADDR + POR0,
			read32(PCMCIA_IF_BASE_ADDR + POR0) |
			(0x4000000 | POR_PV | 0x54306e));
	} else {
		write32(PCMCIA_IF_BASE_ADDR + POR0,
			read32(PCMCIA_IF_BASE_ADDR + POR0) & ~POR_PV);
		write32(PCMCIA_IF_BASE_ADDR + PBR0, PCMCIA_MEM_BASE_ADDR);
		write32(PCMCIA_IF_BASE_ADDR + POR0,
			read32(PCMCIA_IF_BASE_ADDR + POR0) &
			~(0x6000000 | 0xffffff));
		write32(PCMCIA_IF_BASE_ADDR + POR0,
			read32(PCMCIA_IF_BASE_ADDR + POR0) |
			(0x0000000 | POR_PV | 0x54306e));
	}

	return 0;
}

static int
pcmcia_a5x0_set_io_map(int map)
{
	trace();

	write32(PCMCIA_IF_BASE_ADDR + POR0,
		read32(PCMCIA_IF_BASE_ADDR + POR0) & ~POR_PV);
	write32(PCMCIA_IF_BASE_ADDR + PBR0, PCMCIA_MEM_BASE_ADDR);
	write32(PCMCIA_IF_BASE_ADDR + POR0,
		read32(PCMCIA_IF_BASE_ADDR + POR0) &
		~(0x6000000 | 0xffffff));
	write32(PCMCIA_IF_BASE_ADDR + POR0,
		read32(PCMCIA_IF_BASE_ADDR + POR0) |
		(0x6000000 | POR_PV | 0x30606e));
	return 0;
}

static pcmcia_ops_t a5x0_pcmcia_ops = {
	.hw_init	= pcmcia_a5x0_hw_init,
	.hw_free	= pcmcia_a5x0_hw_free,
	.set_socket	= pcmcia_a5x0_set_socket,
	.get_status	= pcmcia_a5x0_get_status,
	.set_mem_map	= pcmcia_a5x0_set_mem_map,
	.set_io_map	= pcmcia_a5x0_set_io_map,
};

struct ide_device armadillo5x0_pcmcia = {
	.ext_probe	= pcmcia_probe,
	.ext_remove	= pcmcia_remove,
	.ext_priv	= (void *)&a5x0_pcmcia_ops,

	.devid		= 0,
	.reset_retry = 1,

	.data_port	= PCMCIA_MEM_BASE_ADDR,
	.ctrl_port	= PCMCIA_MEM_BASE_ADDR + 0xe,
};
