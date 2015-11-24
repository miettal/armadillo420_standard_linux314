/*
 * Copyright (c) 2001 Atmark Techno, Inc.  All Rights Reserved.
 */

#define CORE_NAME "flash_intel"

#include <autoconf.h>
#include <hermit.h>
#include <arch/byteorder.h>
#include <buffer.h>
#include <io.h>
#include <herrno.h>
#include <scan.h>
#include <flash.h>
#include <string.h>
#include "flash_memcpy.h"

#define FLASH_TIMEOUT 0x40000000

/* Device Command */
#define CMD_READ		(0xFF)
#define CMD_READ_ID		(0x90)
#define CMD_CFI			(0x98)
#define CMD_READ_STATUS		(0x70)
#define CMD_CLEAR_STATUS	(0x50)
#define CMD_WORD_PROGRAM	(0x40)
#define CMD_BUFFER_PROGRAM	(0xe8)
#define CMD_ERASE		(0x20)
#define CMD_CONFIRM		(0xD0)
#define CMD_LOCK_SETUP		(0x60)
#define CMD_LOCKED		(0x01)
#define CMD_UNLOCKED		(0xD0)
#define CMD_LOCKDOWN		(0x2F)

/* Status Description */
#define SR_BWS  (1<<0) /* BEFP Status */
#define SR_BLS  (1<<1) /* Block Locked Status */
#define SR_PSS  (1<<2) /* Program Suspend Status */
#define SR_VPPS (1<<3) /* VPP Status */
#define SR_PS   (1<<4) /* Program Status */
#define SR_ES   (1<<5) /* Erase Status */
#define SR_ESS  (1<<6) /* Erase Suspend Status */
#define SR_DWS  (1<<7) /* Device Write Status */

#define INTEL_BUFFER_PROGRAM_ALIGNED_MASK (0x1f)

#define flash_read8(addr)        read8((addr))
#define flash_read(addr)         cpu_to_le16(read16((addr)))
#define flash_write(addr, b)     write16((addr), cpu_to_le16((b)))

static int (*flash_intel_program_func)(u32, u32, u32);

static flash_cfi cfi_info;

/*
 * Loop until both write state machines complete.
 */
static unsigned short
flash_status_wait(addr_t addr)
{
	unsigned short status;
	long timeout = FLASH_TIMEOUT;

	trace();

	do {
		status = flash_read(addr);
	} while ((!(status & SR_DWS)) &&  timeout-- > 0);

	if (status & ~SR_DWS) {
		dev_err("error status: %x\n", status);
		flash_write((addr & FLASH_BASE_MASK), CMD_CLEAR_STATUS);
		return -H_EIO;
	}

	return 0;
}

/*
 * Loop until the Write State machine is ready, then do a full error
 * check.  Clear status and leave the flash in Read Array mode; return
 * 0 for no error, -1 for error.
 */
static int
flash_status_full_check(addr_t addr)
{
	unsigned short status;

	trace();

	flash_write((addr & FLASH_BASE_MASK), CMD_READ_STATUS);

	status = flash_read(addr);

	flash_write((addr & FLASH_BASE_MASK), CMD_READ);

	if (!(status & ~SR_DWS))
		return 0;

	flash_write((addr & FLASH_BASE_MASK), CMD_CLEAR_STATUS);
	if (status & SR_VPPS)
		dev_err("Flash Error: Vpp Range.\n");
	if (status & SR_PS)
		dev_err("Flash Error: Data Program.\n");
	if (status & SR_ES)
		dev_err("Flash Error: Block Erase.\n");
	if (status & SR_BLS)
		dev_err("Flash Error: Block Protect.\n");

	return -H_EADDR;
}

static int
flash_intel_block_unlock(const u32 block_addr)
{
	unsigned long addr;

	trace();

	addr = (block_addr & ~UNALIGNED_MASK);

	flash_write(addr, CMD_LOCK_SETUP);
	flash_write(addr, CMD_UNLOCKED);

	flash_write((addr & FLASH_BASE_MASK), CMD_READ);

	return 0;
}

static int
flash_intel_word_program(const u32 from, const u32 to, const u32 size)
{
	int ret;
	unsigned int loop;
	unsigned short *ptr;
	unsigned long addr;

	trace();

	if (from & UNALIGNED_MASK)
		return -H_EALIGN;
	if (to & UNALIGNED_MASK)
		return -H_EALIGN;

	loop = (size >> BYTE_TO_WORD_SHIFT);
	ptr = (unsigned short*)from;
	addr = to;

	flash_intel_block_unlock(addr);

	for (; loop--; addr += (sizeof(short) * 2)) {
		flash_write(addr, CMD_WORD_PROGRAM);
		flash_write(addr, *ptr);
		flash_status_wait(addr);

		flash_write(addr + 2, CMD_WORD_PROGRAM);
		flash_write(addr + 2, *(ptr + 1));
		flash_status_wait(addr);

		ret = flash_status_full_check(addr);
		if (ret)
		  return ret;

		ptr += 2;
	}

	return 0;
}

static int
flash_intel_buffer_program(const u32 from, const u32 to, const u32 size)
{
	u16 *addr;
	u16 *data;
	u32 remain;
	u32 max_program_size;
	u32 program_size;
	int i;
	int ret;

	if (from & UNALIGNED_MASK)
		return -H_EALIGN;
	if (to & INTEL_BUFFER_PROGRAM_ALIGNED_MASK)
		return -H_EALIGN;

	addr = (u16 *)to;
	data = (u16 *)from;
	remain = (size + 1) / 2;
	if (cfi_info.max_buf_write_size < 2)
		return -H_ENOCMD;
	max_program_size = (1 << (cfi_info.max_buf_write_size - 1));

	while (remain) {
		u32 start = (u32)addr;

		flash_intel_block_unlock(start);

		flash_write(start, CMD_BUFFER_PROGRAM);
		flash_status_wait(start);


		if (remain >= max_program_size)
			program_size = max_program_size;
		else
			program_size = remain;

		flash_write(start, program_size - 1);

		for (i=0; i<program_size; i++)
			flash_write(addr++, *(data++));

		flash_write(start, CMD_CONFIRM);
		flash_status_wait(start);

		ret = flash_status_full_check(start);
		if (ret)
			return ret;

		remain -= program_size;
	}

	return 0;
}

/*
 * Program the contents of the download buffer to flash at the given
 * address.  Size is also specified; we shouldn't have to track usage
 * of the download buffer, since the host side can easily do that.
 *
 * We write words without checking status between each; we only go
 * through the full status check procedure once, when the full buffer
 * has been written.
 *
 * Alignment problems are errors here; we don't automatically correct
 * them because in the context of this command they signify bugs, and
 * we want to be extra careful when writing flash.
 */
int
flash_intel_program(const u32 from, const u32 to, const u32 size)
{
	if (to & INTEL_BUFFER_PROGRAM_ALIGNED_MASK)
		return flash_intel_word_program(from, to, size);
	else
		return flash_intel_program_func(from, to, size);
}

/*
 * Erase a flash block.  Single argument is the first address in the
 * block (actually, it can be anywhere in the block, but the host-side
 * hermit downloader gives the first address).
 */
int
flash_intel_erase(const u32 erase_addr)
{
	unsigned long addr;

	trace();

	addr = (erase_addr & ~UNALIGNED_MASK);	/* word align */

	flash_intel_block_unlock(addr);

	flash_write(addr, CMD_ERASE);
	flash_write(addr, CMD_CONFIRM);
	flash_status_wait(addr);

	return flash_status_full_check(addr);
}

inline u8
flash_intel_read_byte(const u32 from)
{
	return flash_read8(from);
}

u32
flash_intel_copy_to_dram(const u32 from, const u32 to, const u32 size)
{
	flash_memcpy((u16 *)to, (u16 *)from,(u16 *)(to + size));
	return size;
}

u32
flash_intel_get_id(const u32 base_addr)
{
	u32 base;
	u32 id;

	base = (base_addr & FLASH_BASE_MASK);

	flash_write(base, CMD_READ_ID);

	id = (flash_read(base) << 16) | flash_read(base + 2);

	flash_write(base, CMD_READ);

	return id;
}

int
flash_intel_get_size(const u32 base_addr)
{
	return cfi_info.dev_size;
}

static int
flash_intel_read_cfi(const u32 base_addr)
{
	int i;
	u32 base;
	u8 *buf = (u8 *)&cfi_info;

	base = (base_addr & FLASH_BASE_MASK);

	flash_write(base, CMD_READ);
	flash_write(base, CMD_CFI);

	for (i=0; i<0x1c; i++)
		*(buf++) = flash_read(base + ((0x10 + i) << 1)) & 0xff;

	flash_write(base, CMD_READ);

	if (strncmp(cfi_info.qry, "QRY", 3))
		return -1;

	return 0;
}

int
flash_intel_initialize(const u32 base_addr)
{
	int ret;

	ret = flash_intel_read_cfi(base_addr);

	if (cfi_info.buf_write_timeout_typ)
		flash_intel_program_func = flash_intel_buffer_program;
	else
		flash_intel_program_func = flash_intel_word_program;

	return ret;
}


int
flash_intel_create_eraseblock_table(const u32 base_addr, flash_eb *eb)
{
	int i, addr = 0x2c;
	u32 base;

	base = (base_addr & FLASH_BASE_MASK);

	flash_write(base, CMD_READ);
	flash_write(base, CMD_CFI);

	eb->nr_eraseblock = flash_read(base + ((addr++) << 1)) & 0xff;
	if (eb->nr_eraseblock > MAX_NR_ERASEBLOCK) {
		flash_write(base, CMD_READ);
		return -1;
	}

	for (i=0; i<eb->nr_eraseblock; i++) {
		eb->eraseblock[i].num =
			(flash_read(base + ((addr++) << 1)) & 0xff);
		eb->eraseblock[i].num |=
			(flash_read(base + ((addr++) << 1)) & 0xff) << 8;
		eb->eraseblock[i].size =
			(flash_read(base + ((addr++) << 1)) & 0xff);
		eb->eraseblock[i].size |=
			(flash_read(base + ((addr++) << 1)) & 0xff) << 8;
	}

	flash_write(base, CMD_READ);

	eb->base_addr = base_addr;

#if defined(VERBOSE_DEBUG)
	for (i=0; i<eb->nr_eraseblock; i++) {
		dev_vdbg("eraseblock[%d]: %d x %d\n", i,
			eb->eraseblock[i].num + 1,
			eb->eraseblock[i].size * 256);
	}
#endif
	return 0;
}
