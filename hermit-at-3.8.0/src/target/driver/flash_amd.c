/*
 * Copyright (c) 2001 Atmark Techno, Inc.  All Rights Reserved.
 *
 * Commands to program the Fujitsu MBM29DL324BD flash chips on the
 * Armadillo board.  These are 32 Mbit "bottom boot"
 * (boot and param blocks at low end of address space) flash chips.
 */

#define CORE_NAME "flash_amd"

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

static flash_cfi cfi_info;

#define FLASH_TIMEOUT 400000000

#define flash_read(addr)         read16(addr)
#define flash_write(addr, b)     write16((addr), (b))

#define flash_read_cmd(base, offset) \
	cpu_to_le16(read16((base) + ((offset) << 1)))
#define flash_write_cmd(addr, b) \
	write16((addr), cpu_to_le16((b)))

/*
 * Come up with an error string representing the flash error; we can
 * have distinct errors for the two chips.  Get the status from
 * errdata, since this is used for error output.
 */
static void flash_print_error_string(void)
{
	dev_err("status: %p error\n", errdata.word);
}

/*
 * Loop until both write state machines complete.
 */
static unsigned short flash_status_wait(addr_t addr, unsigned short value)
{
	unsigned short status;
	long timeout = FLASH_TIMEOUT;

	while (((status = (flash_read(addr))) != value) && timeout > 0) {
		timeout--;
	}
	return status;
}

/*
 * Loop until the Write State machine is ready, then do a full error
 * check.  Clear status and leave the flash in Read Array mode; return
 * 0 for no error, -1 for error.
 */
static int flash_status_full_check(addr_t addr, unsigned short value1, unsigned short value2)
{
	unsigned short status1, status2;

	status1 = flash_status_wait(addr, value1);
	status2 = flash_status_wait(addr + 2, value2);
	if (status1 != value1 || status2 != value2) {
		errdata.word = status1 | status2 << 16;
		errfunc = &flash_print_error_string;
	}
	return (status1 != value1 || status2 != value2) ? -1 : 0;
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
int flash_amd_program(const u32 from, const u32 to, const u32 size)
{
	int status;
	unsigned int loop;
	unsigned short *ptr;
	unsigned long base;
	unsigned long addr;

	if (from & UNALIGNED_MASK) return -H_EALIGN;
	if (to & UNALIGNED_MASK) return -H_EALIGN;

	base = (to & FLASH_BASE_MASK);

	loop = (size >> BYTE_TO_WORD_SHIFT);
	ptr = (unsigned short*)from;
	addr = to;

	for(status = 0; loop--; addr += (sizeof(short) * 2)) {
		flash_write_cmd(base + (0x555 << 1), 0xAA);
		flash_write_cmd(base + (0x2AA << 1), 0x55);
		flash_write_cmd(base + (0x555 << 1), 0xA0);
		flash_write(addr, *ptr);
		flash_status_wait(addr, *ptr);

		flash_write_cmd(base + (0x555 << 1), 0xAA);
		flash_write_cmd(base + (0x2AA << 1), 0x55);
		flash_write_cmd(base + (0x555 << 1), 0xA0);
		flash_write(addr + 2, *(ptr + 1));
		flash_status_wait(addr + 2, *(ptr + 1));
		status |= flash_status_full_check(addr, *ptr, *(ptr + 1));
		ptr += 2;
	}
	return status;
}

/*
 * Erase a flash block.  Single argument is the first address in the
 * block (actually, it can be anywhere in the block, but the host-side
 * hermit downloader gives the first address).
 */
int flash_amd_erase(const u32 erase_addr)
{
	unsigned long base;
	unsigned long addr;

	addr = (erase_addr & ~UNALIGNED_MASK);	/* word align */

	base = (erase_addr & FLASH_BASE_MASK);

	flash_write_cmd(base + (0x555 << 1), 0xAA);
	flash_write_cmd(base + (0x2AA << 1), 0x55);
	flash_write_cmd(base + (0x555 << 1), 0x80);
	flash_write_cmd(base + (0x555 << 1), 0xAA);
	flash_write_cmd(base + (0x2AA << 1), 0x55);
	flash_write_cmd(addr, 0x30);
	return flash_status_full_check(addr, 0xFFFF, 0xFFFF);
}

inline u8 flash_amd_read_byte(const u32 from)
{
	return *(volatile u8 *)from;
}

u32 flash_amd_copy_to_dram(const u32 from, const u32 to, const u32 size)
{
	flash_memcpy((u16 *)to, (u16 *)from,(u16 *)(to + size));
	return size;
}

u32 flash_amd_get_id(const u32 base_addr)
{
	u32 base;
	u32 id;

	base = (base_addr & FLASH_BASE_MASK);

	flash_write_cmd(base, 0xF0);
	flash_write_cmd(base + (0x555 << 1), 0xAA);
	flash_write_cmd(base + (0x2AA << 1), 0x55);
	flash_write_cmd(base + (0x555 << 1), 0x90);

	id = *(u32 *)(base);

	flash_write_cmd(base, 0xF0);

	return id;
}

int flash_amd_get_size(const u32 base_addr)
{
	return cfi_info.dev_size;
}

static int
flash_amd_read_cfi(const u32 base_addr)
{
	int i;
	u32 base;
	u8 *buf = (u8 *)&cfi_info;

	base = (base_addr & FLASH_BASE_MASK);

	flash_write_cmd(base, 0xf0);
	flash_write_cmd(base + (0x555 << 1), 0x98);

	for (i=0; i<0x1c; i++)
		*(buf++) = flash_read_cmd(base, 0x10 + i) & 0xff;

	flash_write_cmd(base, 0xf0);

	if (strncmp(cfi_info.qry, "QRY", 3))
		return -1;

	return 0;
}

int flash_amd_initialize(const u32 base_addr)
{
	int ret;

	ret = flash_amd_read_cfi(base_addr);

	return ret;
}

int
flash_amd_create_eraseblock_table(const u32 base_addr, flash_eb *eb)
{
	int i, addr = 0x2c;
	u32 base;

	base = (base_addr & FLASH_BASE_MASK);

	flash_write_cmd(base, 0xf0);
	flash_write_cmd(base + (0x555 << 1), 0x98);

	eb->nr_eraseblock = flash_read_cmd(base, addr++) & 0xff;
	if (eb->nr_eraseblock > MAX_NR_ERASEBLOCK) {
		flash_write_cmd(base, 0xf0);
		return -1;
	}

	for (i=0; i<eb->nr_eraseblock; i++) {
		eb->eraseblock[i].num =
			(flash_read_cmd(base, addr++) & 0xff);
		eb->eraseblock[i].num |=
			(flash_read_cmd(base, addr++) & 0xff) << 8;
		eb->eraseblock[i].size =
			(flash_read_cmd(base, addr++) & 0xff);
		eb->eraseblock[i].size |=
			(flash_read_cmd(base, addr++) & 0xff) << 8;
	}

	flash_write_cmd(base, 0xf0);

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
