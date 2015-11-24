/*
 * Copyright (c) 2000 Blue Mug, Inc.  All Rights Reserved.
 */

#ifndef _HERMIT_TARGET_FLASH_H_
#define _HERMIT_TARGET_FLASH_H_

#include <autoconf.h>
#include <htypes.h>
#include <arch/byteorder.h>
#include <memdev.h>

/* Device-ID,Manufacturer-ID */
#define FLASH_DID_UNKNOWN              (0x00000000)
#define FLASH_DID_AMD_AM29LV160DB      (0x22490001)
#define FLASH_DID_FUJITSU_MBM29LV160BE (0x22490004)
#define FLASH_DID_ST_M29W160EB         (0x22490020)
#define FLASH_DID_ST_M29DW323DB        (0x225f0020)

enum _flash_manufacturer_id{
	FLASH_MID_UNKNOWN = 0x0000,
	FLASH_MID_AMD     = 0x0001,
	FLASH_MID_FUJITSU = 0x0004,
	FLASH_MID_ST      = 0x0020,
};

enum _flash_amd_device_id{
	FLASH_DID_AMD_UNKNOWN = 0x0000,
};

enum _flash_fujitsu_device_id{
	FLASH_DID_FUJITSU_UNKNOWN = 0x0000,
};

enum _flash_st_device_id{
	FLASH_DID_ST_UNKNOWN = 0x0000,
	FLASH_DID_ST_M25P64  = 0x2017,
};

#if defined(ARMADILLOJ_CTYPE)
#define PROTECT_START (0x02000000)
#define PROTECT_END   (0x02010000 - 1)
#else
#define PROTECT_START 1
#define PROTECT_END   0
#endif


#if defined(ARMADILLOJ_STYPE) || defined(ARMADILLOJ_CTYPE) || defined(CONFIG_PLATFORM_SUZAKU)
#define FLASH_BASE_MASK (0xFF000000)
#else
#define FLASH_BASE_MASK (0xF0000000)
#endif

#define FLASH_8MiB (0x17)

enum _flash_type{
	FLASH_TYPE_AMD = 0,
	FLASH_TYPE_INTEL,
	FLASH_TYPE_SPI,
};

#define MAX_NR_ERASEBLOCK 4
typedef struct flash_eraseblock {
	u16 num; /* number of identical-size erase blocks */
	u16 size; /* region erase blocks size: (size)*256 byte */
} eraseblock_t;

typedef struct flash_eraseblock_info {
	u32 base_addr;
	int nr_eraseblock;
	eraseblock_t eraseblock[MAX_NR_ERASEBLOCK];
} flash_eb;

extern flash_eb eb;

typedef struct flash_operations {
	int	(*initialize)(const u32 base_addr);
	int	(*program)(const u32 from, const u32 to, const u32 size);
	int	(*erase)(const u32 erase_addr);
	u8	(*read_byte)(const u32 from);
	u32	(*copy_to_dram)(const u32 from, const u32 to, const u32 size);
	u32	(*get_id)(const u32 base_addr);
	int	(*get_size)(const u32 base_addr);

	int	(*create_eraseblock_table)(const u32 base_addr, flash_eb *);
}flash_ops;

typedef struct flash_protect_info {
	u32	start;
	u32	size;
}flash_protect;

extern flash_ops fops;

typedef struct flash_cfi_info {
	char	qry[3];
	u16	p_id;
	u16	p_addr;
	u16	a_id;
	u16	a_addr;
	u8	vcc_min;
	u8	vcc_max;
	u8	vpp_min;
	u8	vpp_max;
	u8	word_write_timeout_typ;
	u8	buf_write_timeout_typ;
	u8	block_erase_timeout_typ;
	u8	chip_erase_timeout_typ;
	u8	word_write_timeout_max;
	u8	buf_write_timeout_max;
	u8	block_erase_timeout_max;
	u8	chip_erase_timeout_max;
	u8	dev_size;
	u16	interface_desc;
	u16	max_buf_write_size;
	u8	num_erase_regions;
} __attribute__((packed)) flash_cfi;

#include <command.h>

extern const command_t flash_program_command;
extern const command_t flash_erase_command;

extern int flash_amd_initialize(const u32 base_addr) __weak;
extern int flash_amd_program(const u32, const u32, const u32) __weak;
extern int flash_amd_erase(const u32) __weak;
extern u8  flash_amd_read_byte(const u32) __weak;
extern u32 flash_amd_copy_to_dram(const u32, const u32, const u32) __weak;
extern u32 flash_amd_get_id(const u32) __weak;
extern int flash_amd_get_size(const u32) __weak;

extern int flash_amd_create_eraseblock_table(const u32, flash_eb *) __weak;

extern int flash_intel_initialize(const u32) __weak;
extern int flash_intel_program(const u32, const u32, const u32) __weak;
extern int flash_intel_erase(const u32) __weak;
extern u8  flash_intel_read_byte(const u32) __weak;
extern u32 flash_intel_copy_to_dram(const u32, const u32, const u32) __weak;
extern u32 flash_intel_get_id(const u32) __weak;
extern int flash_intel_get_size(const u32) __weak;

extern int flash_intel_create_eraseblock_table(const u32, flash_eb *) __weak;

extern int flash_spi_initialize(const u32 base_addr) __weak;
extern int flash_spi_program(const u32, const u32, const u32) __weak;
extern int flash_spi_erase(const u32) __weak;
extern u8  flash_spi_read_byte(const u32) __weak;
extern u32 flash_spi_copy_to_dram(const u32, const u32, const u32) __weak;
extern u32 flash_spi_get_id(const u32) __weak;
extern int flash_spi_get_size(const u32) __weak;

extern int flash_spi_create_eraseblock_table(const u32, flash_eb *) __weak;

int flash_initialize(const int mode, const u32 base_addr);
int flash_register_protect_table(flash_protect *table);
int flash_program(const u32 from, const u32 to, const u32 size);
int flash_erase(const u32 erase_addr);
extern u8 flash_read8(const addr_t addr);
extern u16 flash_read16(const addr_t addr);
extern u32 flash_read32(const addr_t addr);
u32 flash_copy_to_dram(const u32 from, const u32 to, const u32 size);
u32 flash_get_id(const u32 base_addr);
int flash_get_size(const u32 base_addr);

int flash_get_eraseblock_size(const int eraseblock);
u32 flash_get_eraseblock_addr(const int eraseblock);
int flash_addr_to_eraseblock(const u32 addr);
int flash_get_block_string(char *buf, size_t size,
			   addr_t start, addr_t end, int flag);

extern int flash_memdev_init(struct memory_device *memdev);

#endif /* _HERMIT_TARGET_FLASH_H_ */
