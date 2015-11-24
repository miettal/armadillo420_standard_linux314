#ifndef _HERMIT_TARGET_IDE_CORE_H_
#define _HERMIT_TARGET_IDE_CORE_H_

#include <htypes.h>
#include <gendisk.h>
#include <file.h>

typedef struct {
	u8 boot_ind;
	u8 head;
	u8 sector;
	u8 cyl;
	u8 sys_ind;
	u8 end_head;
	u8 end_sector;
	u8 end_cylinder;
	u8 start4[4];
	u8 size4[4];
} __attribute__((packed)) partition_t;

typedef struct file file_t;

struct ide_device {
	void *ext_priv; /* external private data */
	int (*ext_probe)(struct ide_device *dev, int drive);
	void (*ext_remove)(struct ide_device *dev);

	int devid;
	int reset_retry;

	u32 data_port;
	u32 ctrl_port;

	u8 (*inb)(addr_t addr);
	u16 (*inw)(addr_t addr);
	void (*outb)(addr_t addr, u8 val);
	void (*outw)(addr_t addr, u16 val);
};

extern struct gendisk *ide_probe(struct ide_device *dev, int disk);
extern void ide_remove(struct ide_device *dev);
extern int ide_load_kernel(char *device);
#endif
