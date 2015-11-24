/*
 * Copyright (c) 2006 Atmark Techno, Inc.  All Rights Reserved.
 */

#include <htypes.h>

inline u8 flash_spi_read_byte(const u32 from)
{
	return -1;
}

u32 flash_spi_copy_to_dram(const u32 from, const u32 to, const u32 size)
{
	return -1;
}

int flash_spi_program(const u32 from, const u32 to, const u32 size)
{
	return -1;
}

int flash_spi_erase(const u32 erase_addr)
{
	return -1;
}

u32 flash_spi_get_id(const u32 base_addr)
{
	return -1;
}

int flash_spi_get_size(const u32 base_addr)
{
	return -1;
}

int flash_spi_initialize(const u32 base_addr)
{
	return -1;
}
