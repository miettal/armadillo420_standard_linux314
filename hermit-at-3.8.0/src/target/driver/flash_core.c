/*
 * Copyright (c) 2006 Atmark Techno, Inc.  All Rights Reserved.
 *  Masahiro NAKAI <nakai@atmark-techno.com>
 *
 *
 */

#include <buffer.h>
#include <io.h>
#include <herrno.h>
#include <scan.h>
#include <flash.h>
#include <string.h>
#include <arch/byteorder.h>
#include <memdev.h>

#define DLBUFSIZE (8*1024)
u8 dlbuf [DLBUFSIZE];
u32 dlbufsize = DLBUFSIZE;

flash_ops fops;
flash_eb eb;

static flash_protect *protect_table = 0;

int flash_initialize(const int type, const u32 base_addr)
{
	memzero(&fops, sizeof(flash_ops));
	memzero(&eb, sizeof(flash_eb));

	switch(type){
	case FLASH_TYPE_AMD:
		fops.initialize   = flash_amd_initialize;
		fops.program      = flash_amd_program;
		fops.erase        = flash_amd_erase;
		fops.read_byte    = flash_amd_read_byte;
		fops.copy_to_dram = flash_amd_copy_to_dram;
		fops.get_id       = flash_amd_get_id;
		fops.get_size     = flash_amd_get_size;

		fops.create_eraseblock_table =
			flash_amd_create_eraseblock_table;
		break;
	case FLASH_TYPE_INTEL:
		fops.initialize   = flash_intel_initialize;
		fops.program      = flash_intel_program;
		fops.erase        = flash_intel_erase;
		fops.read_byte    = flash_intel_read_byte;
		fops.copy_to_dram = flash_intel_copy_to_dram;
		fops.get_id       = flash_intel_get_id;
		fops.get_size     = flash_intel_get_size;

		fops.create_eraseblock_table =
			flash_intel_create_eraseblock_table;
		break;
	case FLASH_TYPE_SPI:
		fops.initialize   = flash_spi_initialize;
		fops.program      = flash_spi_program;
		fops.erase        = flash_spi_erase;
		fops.read_byte    = flash_spi_read_byte;
		fops.copy_to_dram = flash_spi_copy_to_dram;
		fops.get_id       = flash_spi_get_id;
		fops.get_size     = flash_spi_get_size;

		fops.create_eraseblock_table =
			flash_spi_create_eraseblock_table;
		break;
	default:
		break;
	}

	if (fops.initialize)
		fops.initialize(base_addr);
	if (fops.create_eraseblock_table)
		fops.create_eraseblock_table(base_addr, &eb);

	return 0;
}

int flash_register_protect_table(flash_protect *table)
{
	protect_table = table;
	return 0;
}

int flash_program(const u32 from, const u32 to, const u32 size)
{
	int ret = -1;

	if(fops.program)
		ret = fops.program((u32)from, to, size);

	return ret;
}

static int flash_program_cmdfunc(int argc, char *argv[])
{
	addr_t addr;
	size_t size;

	if (argc != 3)
		return -H_EUSAGE;
	if (scan(*++argv, &addr)) return -H_EADDR;
	if (scan(*++argv, &size)) return -H_EADDR;
	if (addr & UNALIGNED_MASK) return -H_EALIGN;
	if (size & UNALIGNED_MASK) return -H_EALIGN;

	return flash_program((u32)dlbuf, addr, size);
}

const command_t flash_program_command = { "program", "<addr> <size>",
					  "program flash from download buffer",
					  &flash_program_cmdfunc };

COMMAND(flash_program_command);

int flash_erase(const u32 erase_addr)
{
	int ret = -1;

	if(fops.erase)
		ret = fops.erase(erase_addr);

	return ret;
}

static int flash_erase_cmdfunc(int argc, char *argv[])
{
	addr_t addr;
	int i;

	if (argc != 2)
		return -H_EUSAGE;
	if (scan(*++argv, &addr)) return -H_EADDR;
	addr &= ~(addr_t)UNALIGNED_MASK;

#if defined(ARMADILLOJ_CTYPE)
	/* Force Erase protect */
	if(PROTECT_START <= addr && addr <= PROTECT_END){
		return -H_EADDR;
	}
#endif
	if(protect_table){
		for(i=0; protect_table[i].size; i++){
			if(protect_table[i].start <= addr &&
			   addr <= protect_table[i].start +
			   protect_table[i].size - 1)
				return -H_EADDR;
		}
	}

	return flash_erase(addr);
}

const command_t flash_erase_command = { "erase", "<addr>",
					"erase flash block",
					&flash_erase_cmdfunc };

COMMAND(flash_erase_command);

u8 flash_read8(const addr_t addr)
{
	return fops.read_byte(addr);
}

u16 flash_read16(const addr_t addr)
{
	u16 val;

	if (addr & 0x1) {
		hprintf("unaligned addr: %p\n", addr);
		return -1;
	}

	val = fops.read_byte(addr);
	val |= fops.read_byte(addr + 1) << 8;
	return le16_to_cpu(val);
}

u32 flash_read32(const addr_t addr)
{
	u32 val;

	if (addr & 0x3) {
		hprintf("unaligned addr: %p\n", addr);
		return -1;
	}

	val = fops.read_byte(addr);
	val |= fops.read_byte(addr + 1) << 8;
	val |= fops.read_byte(addr + 2) << 16;
	val |= fops.read_byte(addr + 3) << 24;
	return le32_to_cpu(val);
}

u32 flash_copy_to_dram(const u32 from, const u32 to, const u32 size)
{
	u32 ret = -1;

	if(fops.copy_to_dram)
		ret = fops.copy_to_dram(from, to, size);

	return ret;
}

u32 flash_get_id(const u32 base_addr)
{
	u32 ret = -1;

	if(fops.get_id)
		ret = fops.get_id(base_addr);

	return ret;
}

int flash_get_size(const u32 base_addr)
{
	int ret = -1;

	if(fops.get_size)
		ret = fops.get_size(base_addr);

	return ret;
}

int
flash_get_eraseblock_size(const int eraseblock)
{
	int i;
	int block = eraseblock;

	if (eb.nr_eraseblock < 0 || MAX_NR_ERASEBLOCK < eb.nr_eraseblock)
		return -1;

	for (i=0; i<eb.nr_eraseblock; i++) {
		if (block < (eb.eraseblock[i].num + 1))
			return (eb.eraseblock[i].size * 256);
		block -= eb.eraseblock[i].num + 1;
	}

	return -1;
}

u32
flash_get_eraseblock_addr(const int eraseblock)
{
	int i, j;
	int index = 0;
	u32 addr = eb.base_addr;

	if (eb.nr_eraseblock < 0 || MAX_NR_ERASEBLOCK < eb.nr_eraseblock)
		return -1;

	for (i=0; i<eb.nr_eraseblock; i++) {
		for (j=0; j<eb.eraseblock[i].num + 1; j++) {
			if (index++ == eraseblock)
				return addr;

			addr += (eb.eraseblock[i].size * 256);
		}
	}

	return -1;
}

int
flash_addr_to_eraseblock(const u32 addr)
{
	int i, j;
	int index = 0;
	u32 flashaddr = eb.base_addr;

	if (eb.nr_eraseblock < 0 || MAX_NR_ERASEBLOCK < eb.nr_eraseblock)
		return -1;

	for (i=0; i<eb.nr_eraseblock; i++) {
		for (j=0; j<eb.eraseblock[i].num + 1; j++) {
			if ((flashaddr <= addr) &&
			    (addr < (flashaddr + eb.eraseblock[i].size * 256)))
				return index;

			flashaddr += (eb.eraseblock[i].size * 256);
			index++;
		}
	}

	return -1;
}

int
flash_get_block_string(char *buf, size_t size,
		       addr_t start, addr_t end, int flag)
{
	int start_blk, end_blk;
	int i;
	char *first;
	int blk_size, prev_blk_size = -1;
	int blk_count = 0;
	start_blk = flash_addr_to_eraseblock(start);
	end_blk = flash_addr_to_eraseblock(end);

	buf += hsnprintf(buf, size, "bf:8K bl:");
	first = buf;
	for (i=start_blk; i<=end_blk; i++) {
		blk_size = flash_get_eraseblock_size(i);
		if (prev_blk_size != blk_size) {
			if (prev_blk_size != -1) {
				buf += hsprintf(buf, "%s%dx%dK%s",
						buf != first ? "," : "",
						blk_count, prev_blk_size >> 10,
						flag ? "/l" : "");
			}
			blk_count = 0;
			prev_blk_size = blk_size;
		}
		blk_count++;
	}
	if (prev_blk_size != -1) {
		buf += hsprintf(buf, "%s%dx%dK%s",
				buf != first ? "," : "",
				blk_count, prev_blk_size >> 10,
				flag ? "/l" : "");
	}
	*buf = '\0';

	return 0;
}

static int flash_memdev_read(struct memory_device *memdev,
			     u32 addr, u32 size, void *buf)
{
	memcpy(buf, (void *)addr, size);

	return 0;
}

static int flash_memdev_write(struct memory_device *memdev,
			      u32 addr, u32 size, void *buf)
{
	flash_program((u32)buf, addr, size);

	return 0;
}

static int flash_memdev_erase(struct memory_device *memdev,
			      u32 addr, u32 size)
{
	int loop = 1;

	if (memdev->blksize)
		loop = (size + memdev->blksize - 1) / memdev->blksize;

	while (loop--) {
		flash_erase(addr);
		addr += memdev->blksize;
	}
	return 0;
}

int flash_memdev_init(struct memory_device *memdev)
{
	int eraseblock;

	if (!memdev)
		return -H_EINVAL;

	memdev->name = "flash";
	memdev->type = MEMDEV_TYPE_CHAR;
	memdev->read = flash_memdev_read;
	memdev->write = flash_memdev_write;
	memdev->erase = flash_memdev_erase;
	eraseblock = flash_addr_to_eraseblock(memdev->res.start);
	memdev->blksize = flash_get_eraseblock_size(eraseblock);

	return 0;
}
