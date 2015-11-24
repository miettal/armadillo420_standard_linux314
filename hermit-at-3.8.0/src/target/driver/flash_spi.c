/*
 * Copyright (c) 2006 Atmark Techno, Inc.  All Rights Reserved.
 *  Yasushi SHOJI <yashi@atmark-techno.com>
 *
 * Commands to program STMicroelectronics M25P64 via Xilinx
 * OPB-SPI. SPI bus and flash command is not separated right now.  we
 * will do so when we support another bus controller and/or flash chip
 * with difference command format.
 */

#include <autoconf.h>
#include <hermit.h>
#include <buffer.h>
#include <io.h>
#include <herrno.h>
#include <scan.h>
#include <flash.h>
#include <arch/flash_spi.h>
#include <string.h>

/* SPI FIFO size */
#define USE_FIFO
#define FIFO_SIZE 16

/* Write In Progress(WIP) wait-timeout */
#define SPI_TIMEOUT 0x100000

/* Proto-type declarations */
static u8 spi_read_status(void);
static inline void spi_set_cs(u32);
static inline void spi_write8(u8);
static inline void spi_write24(u32);
static inline u8 spi_read8(void);
static inline u32 spi_read24(void);

#ifdef FLASH_SPI_DEBUG
static void dump_reg(void)
{
	hprintf("register dump\n");
	hprintf("reset: 0x%x\n", __spi_read32(REG_IPIF_SOFT_RESET));
	hprintf("cr   : 0x%x\n", __spi_read32(REG_SPICR));
	hprintf("sr   : 0x%x\n", __spi_read32(REG_SPISR));
	hprintf("ssr  : 0x%x\n", __spi_read32(REG_SPISSR));
	hprintf("tx   : 0x%x\n", __spi_read32(REG_TX_FIFO_OCY));
	hprintf("rx   : 0x%x\n", __spi_read32(REG_RX_FIFO_OCY));
	hprintf("dgie : 0x%x\n", __spi_read32(REG_DGIE));
	hprintf("isr  : 0x%x\n", __spi_read32(REG_IPISR));
	hprintf("er   : 0x%x\n", __spi_read32(REG_IPIER));
}

static void dump_spi_data(u32 addr, u32 size)
{
	int i;
	u32 spi_addr = addr & 0x00FFFFFF;

	hprintf("data dump: 0x%x\n", addr);

	spi_set_cs(1);
	spi_write8(FAST_READ);
	spi_write24(spi_addr);
	spi_write8(DUMMY);

	for(i=0;i<size;i++){
		if(i%16 == 0){
			hprintf("0x%x: ", spi_addr + i);
		}

		hprintf("%b ", spi_read8());

		if(i%16 == 15){
			hprintf("\n");
		}
	}

	spi_set_cs(0);

	if(i%16 != 0){
		hprintf("\n");
	}
}
#endif

/**
 * select which cs (chip select) line to activate
 */
static inline void spi_set_cs(u32 cs)
{
	__spi_write32(REG_SPISSR, ~cs);
	__spi_write32(REG_SPICR, SPICR_MASTER|SPICR_SPE);
	__spi_write32(REG_SPICR, SPICR_MSSSAE|SPICR_MASTER|SPICR_SPE);
}

static inline int spi_is_rx_empty(void)
{
	return __spi_read32(REG_SPISR) & SPISR_RX_EMPTY;
}

static inline int spi_is_tx_empty(void)
{
	return __spi_read32(REG_SPISR) & SPISR_TX_EMPTY;
}

static inline int spi_is_tx_full(void)
{
	return __spi_read32(REG_SPISR) & SPISR_TX_FULL;
}

static inline void spi_write_data(u8 v)
{
	__spi_write32(REG_SPIDTR, v);
}

static inline u8 spi_read_data(void)
{
	return __spi_read32(REG_SPIDRR);
}

static inline void spi_write8(u8 v)
{
	while (!spi_is_tx_empty());
	spi_write_data(v);
	while (spi_is_rx_empty());
	spi_read_data();
}

static inline u8 spi_read8(void)
{
	while (!spi_is_tx_empty());
	spi_write_data(DUMMY);
	while (spi_is_rx_empty());
	return spi_read_data();
}

#ifdef USE_FIFO

static inline void spi_write24(u32 data)
{
	while (!spi_is_tx_empty());
	spi_write_data((data >> 16) & 0xff);
	spi_write_data((data >>  8) & 0xff);
	spi_write_data( data        & 0xff);
	while (spi_is_rx_empty());
	spi_read_data();
	while (spi_is_rx_empty());
	spi_read_data();
	while (spi_is_rx_empty());
	spi_read_data();
}

static inline u32 spi_read24(void)
{
	u32 tmp;

	while (!spi_is_tx_empty());
	spi_write_data(0x00);
	spi_write_data(0x00);
	spi_write_data(0x00);
	while (spi_is_rx_empty());
	tmp         = spi_read_data() << 16;
	while (spi_is_rx_empty());
	tmp        |= spi_read_data() <<  8;
	while (spi_is_rx_empty());
	return tmp |  spi_read_data()      ;
}

static inline void spi_write(const u8 *data, int length)
{
	int n;

	if (length >= FIFO_SIZE)
		n = FIFO_SIZE;
	else
		n = length;
	while (!spi_is_tx_empty());
	while (n--)
		spi_write_data(*data++);
	for (; length > FIFO_SIZE; length--) {
		while (spi_is_rx_empty());
		spi_read_data();
		while (spi_is_tx_full());
		spi_write_data(*data++);
	}
	while (length--) {
		while (spi_is_rx_empty());
		spi_read_data();
	}
}

static inline void spi_read(u8 *data, int length)
{
	int n;

	if (length >= FIFO_SIZE)
		n = FIFO_SIZE;
	else
		n = length;
	while (!spi_is_tx_empty());
	while (n--)
		spi_write_data(0x00);
	for (; length > FIFO_SIZE; length--) {
		while (spi_is_rx_empty());
		*data++ = spi_read_data();
		while (spi_is_tx_full());
		spi_write_data(0x00);
	}
	while (length--) {
		while (spi_is_rx_empty());
		*data++ = spi_read_data();
	}
}

#else

static inline void spi_write24(u32 data)
{
	spi_write8((data>>16) & 0xff);
	spi_write8((data>> 8) & 0xff);
	spi_write8(data       & 0xff);
}

static inline u32 spi_read24(void)
{
	return (spi_read8()<<16) | (spi_read8()<<8) | spi_read8();
}

static inline void spi_write(const u8 *data, int length)
{
	while (length--)
		spi_write8(*data++);
}

static inline void spi_read(u8 *data, int length)
{
	while (length--)
		*data++ = spi_read8();
}

#endif

static void spi_write_enable(void)
{
	spi_set_cs(1);
	spi_write8(WREN);
	spi_set_cs(0);
}

#if 0 /* unused */
static void spi_write_disable(void)
{
	spi_set_cs(1);
	spi_write8(WRDI);
	spi_set_cs(0);
}
#endif

static u32 spi_wait_busy(void)
{
	int i;
	for(i = 0; i < SPI_TIMEOUT; i++){
		if(!(spi_read_status() & RDSR_WIP)) break;
	}

	if(i == SPI_TIMEOUT){
#ifdef FLASH_SPI_DEBUG
		hprintf("wait busy - timeout\n");
#endif
		return -1;
	}
	return 0;
}

static u8 spi_read_status(void)
{
	u8 ret;

	spi_set_cs(1);
	spi_write8(RDSR);
	ret = spi_read8();
	spi_set_cs(0);

	return ret;
}

#if 0 /* unused */
static void spi_write_status(u8 v)
{
	spi_set_cs(1);
	spi_write8(WRSR);
	spi_write8(v);
	spi_set_cs(0);

	spi_wait_busy();
}
#endif

static u32 spi_read_id(void)
{
	u32 ret;

	spi_set_cs(1);
	spi_write8(RDID);
	ret = spi_read24();
	spi_set_cs(0);

	return ret;
}

u8 flash_spi_read_byte(u32 from)
{
	u8 data;

	spi_set_cs(1);
	spi_write8(FAST_READ);
	spi_write24(from & 0x00FFFFFF);
	spi_write8(DUMMY);
	data = spi_read8();
	spi_set_cs(0);

	return data;
}

u32 flash_spi_copy_to_dram(const u32 from, const u32 to, const u32 size)
{
	u8 *ram_addr = (u8 *)to;

	spi_set_cs(1);
	spi_write8(FAST_READ);
	spi_write24(from & 0x00FFFFFF);
	spi_write8(DUMMY);
	spi_read(ram_addr, size);
	spi_set_cs(0);

	return size;
}

#if 0 /* unused */
static int spi_page_verify(const u32 data, const u32 addr, const u32 size)
{
	int i;
	int ret;
	char *tmp = (char *)data;

	if (size > 256) return -1;

	spi_set_cs(1);
	spi_write8(FAST_READ);
	spi_write24(addr & 0x00FFFFFF);
	spi_write8(DUMMY);
	for(i = 0, ret = 0; i < size; i++){
		if (tmp[i] != spi_read8()){
			ret = -1;
			break;
		}
	}
	spi_set_cs(0);

	return ret;
}
#endif

static int spi_page_program(const u32 data, const u32 addr, const u32 size)
{
	unsigned char *tmp = (unsigned char *)data;

	if (size > 256) return -1;

	spi_write_enable();

	spi_set_cs(1);
	spi_write8(PP);
	spi_write24(addr & 0x00FFFFFF);
	spi_write(tmp, size);
	spi_set_cs(0);

	spi_wait_busy();

	return 0;
}

int flash_spi_program(const u32 from, const u32 to, const u32 size)
{
	int i;
	u32 remain = size;
	u32 program_size;

	if(from & UNALIGNED_MASK) return -H_EALIGN;
	if(to & UNALIGNED_MASK) return -H_EALIGN;

	for(i = 0; remain > 0; i++){
		if(remain > 256){
			program_size = 256;
		}else{
			program_size = remain;
		}

		spi_page_program(from + i * 256, to + i * 256, program_size);

		remain -= program_size;
	}

	return 0;
}

static int spi_sector_erase(u32 sector)
{
	spi_write_enable();

	spi_set_cs(1);
	spi_write8(SE);
	spi_write24(sector & 0x00FFFFFF);
	spi_set_cs(0);

	spi_wait_busy();

	return 0;
}

int flash_spi_erase(const u32 erase_addr)
{
	u32 addr = (erase_addr & ~UNALIGNED_MASK);

	spi_sector_erase(addr);

	return 0;
}

u32 flash_spi_get_id(const u32 base_addr)
{
	return spi_read_id();
}

#if 0 /* unused */
static int spi_set_protect_mode(u8 mode)
{
	if((mode < 0) || (mode > 0x7) return -1;

	spi_write_enable();
	spi_write_status((mode & 0x7) << 2);

	return 0;
}

static u32 spi_get_protect_mode(void)
{
	return (spi_read_status() >> 2) & 0x7;
}
#endif /* unused */

int flash_spi_get_size(const u32 base_addr)
{
	u32 id = flash_spi_get_id(0);
	u16 mid, did;

	mid = (id >> 16) & 0xffff;
	did = id & 0xffff;

	switch(mid){
	case FLASH_MID_ST:
		switch(did){
		case FLASH_DID_ST_M25P64:
			return FLASH_8MiB;
		default:
			return -1;
		}
		break;
	default:
		return -1;
	}
}

int flash_spi_initialize(const u32 base_addr)
{
	/* disable/reset everything */
	/* __spi_write32(REG_IPIF_SOFT_RESET); */
	__spi_write32(REG_SPICR,
		      SPICR_MTI|SPICR_RX_FIFO_RESET|SPICR_TX_FIFO_RESET);

	/* disable interrupt, and ack all to clear any bits */
	__spi_write32(REG_DGIE, 0x0);
	__spi_write32(REG_IPIER, 0x0);
	__spi_write32(REG_IPISR, __spi_read32(REG_IPISR));

	/* deselect all slaves */
	__spi_write32(REG_SPISSR, 0xffffffff);

	/* re-enable */
	__spi_write32(REG_SPICR, SPICR_MSSSAE|SPICR_MASTER|SPICR_SPE);

	return 0;
}


#ifdef FLASH_SPI_DEBUG

static int flash_spidump_cmdfunc(int argc, char *argv[])
{
	addr_t addr;
	size_t size;

	if (argc != 3)
		return -H_EUSAGE;
	if (scan(*++argv, &addr)) return -H_EADDR;
	if (scan(*++argv, &size)) return -H_EADDR;
	if (addr & UNALIGNED_MASK) return -H_EALIGN;
	if (size & UNALIGNED_MASK) return -H_EALIGN;

	dump_spi_data(addr, size);

	return 0;
}

const command_t flash_spidump_command = { "spidump", "",
					  "spi-flash dump",
					  &flash_spidump_cmdfunc };

COMMAND(flash_spidump_command);

#endif

static eraseblock_t chip_eb[] = {
#if defined(CONFIG_FLASH_SPI_M25P64)
	{ 128, 256 },
#endif
};

int flash_spi_create_eraseblock_table(const u32 base_addr, flash_eb *eb)
{
	eb->nr_eraseblock = ARRAY_SIZE(chip_eb);
	if (eb->nr_eraseblock < 0)
		return -1;

	eb->base_addr = base_addr;
	memcpy(eb->eraseblock, &chip_eb,
	       sizeof(eraseblock_t) * eb->nr_eraseblock);

	return 0;
}
