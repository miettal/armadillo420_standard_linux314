#ifndef _HERMIT_TARGET_GENDISK_H_
#define _HERMIT_TARGET_GENDISK_H_

#include <file.h>

#define SECTOR_SIZE     0x200

struct gendisk_ops {
	int (*sector_read)(void *dev, u32 sector, u32 nr_sectors, void *buf);
	int (*sector_write)(void *dev, u32 sector, u32 nr_sectors, void *buf);
};

struct gendisk {
	char *name;
	void *dev;
	struct gendisk_ops *ops;

	char *(*diskname)(struct gendisk *disk, int major, int minor);
};

#define gendisk_sector_read(disk, sec, nr_sec, buf) \
	disk->ops->sector_read(disk->dev, sec, nr_sec, buf)

#define gendisk_sector_write(disk, sec, nr_sec, buf) \
	disk->ops->sector_write(disk->dev, sec, nr_sec, buf)

extern int gendisk_file_copy(struct gendisk *disk, struct file *file);
extern int gendisk_find_loadable_image(struct gendisk *disk, struct file *file,
				       int drive, int start, int end);

#endif
