#define CORE_NAME "squashfs"

#include <autoconf.h>
#if defined(CONFIG_FS_SQUASH_DEBUG)
#define DEBUG
#endif
#include <hermit.h>
#include <herrno.h>
#include <string.h>

#define SQUASHFS_MAGIC			0x73717368
#define SQUASHFS_PADDING		0x1000 /* 4K */

extern unsigned long bytes_out;		/* defined by gunzip.c */

struct squashfs_super_block {
	__le32                  s_magic;
	__le32                  inodes;
	__le32                  mkfs_time;
	__le32                  block_size;
	__le32                  fragments;
	__le16                  compression;
	__le16                  block_log;
	__le16                  flags;
	__le16                  no_ids;
	__le16                  s_major;
	__le16                  s_minor;
	__le64                  root_inode;
	__le64                  bytes_used;
	__le64                  id_table_start;
	__le64                  xattr_id_table_start;
	__le64                  inode_table_start;
	__le64                  directory_table_start;
	__le64                  fragment_table_start;
	__le64                  lookup_table_start;
};

int is_squashfs(addr_t addr)
{
	struct squashfs_super_block *sblk = (void *)addr;

	dev_dbg("MAGIC: %p\n", sblk->s_magic);
	if (sblk->s_magic == SQUASHFS_MAGIC)
		return 1;

	return 0;
}

int squashfs_copy_to(char *label, addr_t to, addr_t from, size_t max_length)
{
	struct squashfs_super_block *sblk = (void *)from;
	size_t imgsize, blksize;
	size_t copy, remain, copied;

	if (!is_squashfs(from))
		return -H_EINVAL;

	imgsize = (((size_t)sblk->bytes_used + SQUASHFS_PADDING - 1) &
		   ~(SQUASHFS_PADDING - 1));
	blksize = sblk->block_size;

	dev_dbg("imgsize: %lld (%d)\n", sblk->bytes_used, imgsize);
	dev_dbg("blksize: %d\n", blksize);

	hprintf("Copying %s", label);

	bytes_out = remain = imgsize;
	copied = 0;
	do {
		if (remain >= blksize)
			copy = blksize;
		else
			copy = remain;
		memcpy((void *)(to + copied),
		       (void *)(from + copied),
		       copy);
		copied += copy;
		remain -= copy;
		hprintf(".");
	} while (remain > 0);

	hprintf("done.\n");

	return 0;
}
