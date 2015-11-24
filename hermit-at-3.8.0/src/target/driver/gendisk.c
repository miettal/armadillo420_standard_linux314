#define CORE_NAME "gendisk"

#include <autoconf.h>
#include <hermit.h>
#include <platform.h>
#include <herrno.h>
#include <command.h>
#include <io.h>
#include <string.h>

#include <file.h>
#include <gendisk.h>

#include <ext2.h>

#define read4_le(cp)	((unsigned long)(cp[0]) +        \
			((unsigned long)(cp[1]) << 8) +  \
			((unsigned long)(cp[2]) << 16) + \
			((unsigned long)(cp[3]) << 24))

static int gendisk_read_file_data(struct gendisk *disk, u32 count, u32 *table,
				  u32 start, u32 sectors_per_block,
				  u32 remain, u32 dst_addr)
{
	u32 file_data_sector, copy_size;
	u32 copied = 0;
	int ret;
	int i;

	trace();
	dev_dbg("dst_addr: %p, remain: %d\n", dst_addr, remain);

	dev_info_r(".");

	for (i = 0; i < count && (remain - copied) > 0; i++) {
		file_data_sector = start + sectors_per_block * table[i];
		ret = gendisk_sector_read(disk, file_data_sector,
					  sectors_per_block, (u8 *)dst_addr);
		if (ret < 0)
			return -H_EIO;

		if ((remain - copied) > (SECTOR_SIZE * sectors_per_block))
			copy_size = (SECTOR_SIZE * sectors_per_block);
		else
			copy_size = remain - copied;

		copied += copy_size;
		dst_addr += copy_size;
	}

	dev_dbg("copied: %p (%d)\n", copied, copied);
	return (int)copied;
}

int gendisk_file_copy(struct gendisk *disk, struct file *file)
{
	u32 sectors_per_block;
	u32 file_data_sector, addr_per_block;
	u8 buf[SECTOR_SIZE * 8], buf2[SECTOR_SIZE * 8];
	u32 copied_addr;
	int copied_size;
	int err;
	int ret;
	int j;

	trace();

	dev_info_r("Copying        kernel");

	sectors_per_block = file->sectors_per_block;

	copied_addr = file->load_addr;
	ret = gendisk_read_file_data(disk, 12, file->blocks,
				     file->partition_start,
				     sectors_per_block, file->size,
				     copied_addr);
	if (ret < 0) {
		dev_err("%s data read error.\n", file->name);
		return -H_EIO;
	}
	copied_size = ret;
	copied_addr += ret;

	file_data_sector =
		file->partition_start + sectors_per_block * file->blocks[12];

	ret = gendisk_sector_read(disk, file_data_sector,
				  sectors_per_block, buf);
	if (ret < 0) {
		dev_err("%s data read error.\n", file->name);
		return -H_EIO;
	}

	addr_per_block = (SECTOR_SIZE * sectors_per_block) / sizeof(u32);
	ret = gendisk_read_file_data(disk, addr_per_block, (u32 *)buf,
				 file->partition_start,
				 sectors_per_block,
				 file->size - copied_size,
				 copied_addr);
	if (ret < 0) {
		dev_err("%s data read error.\n", file->name);
		return -H_EIO;
	}
	copied_size += ret;
	copied_addr += ret;

	file_data_sector = (file->partition_start +
			    sectors_per_block * file->blocks[13]);

	ret = gendisk_sector_read(disk, file_data_sector,
				  sectors_per_block, buf2);
	if (ret < 0) {
		dev_err("%s data read error.\n", file->name);
		return -H_EIO;
	}

	err = 0;
	for (j = 0;
	     j < addr_per_block && (file->size - copied_size) > 0;
	     j++) {
		file_data_sector = (file->partition_start +
				    sectors_per_block * *(((u32 *)buf2) + j));

		ret = gendisk_sector_read(disk, file_data_sector,
					  sectors_per_block, buf);
		if (ret < 0) {
			err = 1;
			break;
		}
		ret = gendisk_read_file_data(disk, addr_per_block,
					 (u32 *)buf, file->partition_start,
					 sectors_per_block,
					 (file->size - copied_size),
					 copied_addr);
		if (ret < 0) {
			err = 1;
			break;
		}
		if (ret == 0)
			break;
		copied_size += ret;
		copied_addr += ret;
	}

	if (err) {
		dev_err("%s data read error.\n", file->name);
		return -H_EIO;
	} else
		dev_info_r("done.\n");

	return 0;
}

static int gendisk_find_image(struct gendisk *disk, int drive, int pno,
			      partition_t *partition, struct file *file)
{
	u32 super_block_sector;
	u8 buf[SECTOR_SIZE * 8];
	int ret;
	int fs_detect;

	trace();

	if (partition->sys_ind != 0x83) {
		dev_err("/dev/%s: Unknown partition type '%b'\n",
			disk->diskname(disk, drive, pno + 1),
			partition->sys_ind);
		return -H_ENOSYS;
	}

	file->partition_start = read4_le(partition->start4);
	file->partition_size = read4_le(partition->size4);
	dev_info("/dev/%s: start=%p, size=%p\n",
		 disk->diskname(disk, drive, pno + 1),
		 file->partition_start, file->partition_size);
	super_block_sector = file->partition_start + 2;

	ret = gendisk_sector_read(disk, super_block_sector, 1, buf);
	if (ret < 0) {
		dev_err("/dev/%s: Super block read error.\n",
			disk->diskname(disk, drive, pno + 1));
		return -H_EIO;
	}

	fs_detect = 0;
	if (!fs_detect) { /* EXT2 filesystem */
		ext2_super_block_t *ext2sb;
		ext2sb = (ext2_super_block_t *)buf;
		if (ext2sb->s_magic == 0xef53) {
			fs_detect = 1;
			ret = ext2_find_image(disk, file, ext2sb);
		}
	}

	if (!fs_detect) { /* unknown filesystem */
		dev_err("/dev/%s: Unknown FileSystem\n",
			disk->diskname(disk, drive, pno + 1));
		return -H_ENOSYS;
	}

	return ret;
}

int gendisk_find_loadable_image(struct gendisk *disk, struct file *file,
				int drive, int start, int end)
{
	u8 buf[512];
	partition_t partitions[4];
	int ret;
	int i;

	ret = gendisk_sector_read(disk, 0, 1, buf);
	if (ret)
		return ret;

	memcpy(partitions, buf + 0x1be, sizeof(partition_t) * 4);

	for (i=start; i<end; i++) {
		memzero(file, sizeof(struct file));
		ret = gendisk_find_image(disk, drive, i, &partitions[i], file);
		if (ret == -H_ENOSYS)
			continue;
		if (ret < 0)
			return ret;
		dev_info("%s is found. (%d Bytes)\n", file->name, file->size);
		return 0;
	}

	return -H_EINVAL;
}
