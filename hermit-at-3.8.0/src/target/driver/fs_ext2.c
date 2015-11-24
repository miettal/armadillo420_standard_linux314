#define CORE_NAME "ext2"

#include <hermit.h>
#include <io.h>
#include <herrno.h>
#include <htypes.h>
#include <string.h>
#include <ext2.h>
#include <gendisk.h>
#include <file.h>
#include <bitops.h>

#if defined(DEBUG)
char *feature_list[3][32] = {
	/* compat */
	{"dir_prealloc", "imagic_inodes", "has_journal", "ext_attr",
	 "resize_inode", "dir_index"},
	/* incompat */
	{"compression", "filetype", "needs_recovery", "journal_dev", "meta_bg"},
	/* ro_compat */
	{"sparse_super", "large_file", ""},
};

static char *feature_string(int compat, int f)
{
	return feature_list[compat][f] ? feature_list[compat][f] : "(unknown)";
}

static void print_features(ext2_super_block_t *sb)
{
	int i, j, printed = 0;
	u32 *mask = &sb->s_feature_compat, m;

	dev_dbg("features:");
	for (i=0; i<3; i++, mask++) {
		for (j=0, m=1; j<32; j++, m<<=1) {
			if (*mask & m) {
				dev_dbg_r(" %s", feature_string(i, j));
				printed++;
			}
		}
	}
	dev_dbg_r("\n");
}
#else
static void print_features(ext2_super_block_t *sb) {}
#endif

static int is_power_of_2(unsigned long n)
{
	return (n != 0 && ((n & (n - 1)) == 0));
}

static inline int test_root(int a, int b)
{
	int num = b;

	while (a > num)
		num *= b;
	return num == a;
}

static int ext2_group_sparse(int group)
{
	if (group <= 1)
		return 1;
	return (test_root(group, 3) || test_root(group, 5) ||
		test_root(group, 7));
}

static int
ext2_bg_has_super(ext2_super_block_t *sb, u32 bg)
{
	if ((sb->s_feature_ro_compat & 0x1) && !ext2_group_sparse(bg))
		return 0;
	return 1;
}

static ext2_group_desc_t *
ext2_get_group_desc(struct gendisk *disk, struct file *file,
		    ext2_super_block_t *sb, u32 group, u8 *buf)
{
	ext2_group_desc_t *desc;
	u32 block, offset, sector;
	u32 has_super;
	int ret;

	trace();

	block = group >> EXT2_DESC_PER_BLOCK_BITS(sb);
	offset = group & (EXT2_DESC_PER_BLOCK(sb) - 1);

	if (ext2_bg_has_super(sb, block))
		has_super = 1;
	else
		has_super = 0;

	sector = (file->partition_start + file->sectors_per_block *
		  (block + has_super + sb->s_first_data_block) +
		  offset / (SECTOR_SIZE / EXT2_DESC_SIZE));

	ret = gendisk_sector_read(disk, sector, 1, buf);
	if (ret < 0) {
		dev_err("Group desc read error.\n");
		return NULL;
	}
	desc = (ext2_group_desc_t *)buf;
	return desc + (offset % (SECTOR_SIZE / EXT2_DESC_SIZE));
}

int
ext2_find_image(struct gendisk *disk, struct file *file,
		ext2_super_block_t *sb)
{
	ext2_group_desc_t *ext2gd;
	ext2_inode_t *ext2inode;
	ext2_dir_entry_t *ext2de;

	u32 sectors_per_block;
	u32 inodes_per_group;
	u32 inode_table_sector;
	u32 dir_entry_table_sector;
	u32 group, inode;
	u16 inode_size;
	u8 buf[EXT2_MAX_BLOCK_SIZE];
	u8 buf2[EXT2_MAX_BLOCK_SIZE];
	int ext = 0;
	int ret;
	int i;

	trace();

	if (sb->s_log_block_size > 2) {
		dev_err("Bad block size.\n");
		return -H_EIO;
	}
	sectors_per_block = 2;
	for (i=0; i<sb->s_log_block_size; i++)
		sectors_per_block *= 2;

	if (sb->s_rev_level == 0) {
		inode_size = EXT2_GOOD_OLD_INODE_SIZE;
	} else {
		if ((sb->s_inode_size < EXT2_GOOD_OLD_INODE_SIZE) ||
		    (! is_power_of_2(sb->s_inode_size)) ||
		    (sb->s_inode_size > sectors_per_block * SECTOR_SIZE)) {
			dev_err("Bad inode size.\n");
			return -H_EIO;
		}
		inode_size = sb->s_inode_size;
	}
	dev_dbg("inode size: %d\n", inode_size);

	print_features(sb);

	file->sectors_per_block = sectors_per_block;
	inodes_per_group = sb->s_inodes_per_group;
	ext2gd = ext2_get_group_desc(disk, file, sb, 0, buf);
	inode_table_sector = (file->partition_start +
			      sectors_per_block * ext2gd->bg_inode_table);

	ret = gendisk_sector_read(disk, inode_table_sector,
				  file->sectors_per_block, buf);
	if (ret < 0) {
		dev_err("Inode table read error.\n");
		return -H_EIO;
	}

	ext2inode = (ext2_inode_t *)(buf + inode_size);
	if ((ext2inode->i_mode & 0xf000) != 0x4000) {
		dev_err("Can't find root.\n");
		return -H_EIO;
	}

	for (i=0; i<12; i++) {
		if (!ext2inode->i_block[i])
			break;
		dir_entry_table_sector = (
			file->partition_start + sectors_per_block *
			ext2inode->i_block[i]);
		ret = gendisk_sector_read(disk, dir_entry_table_sector,
					  sectors_per_block, buf2);
		if (ret < 0) {
			dev_err("Root directory entry read error.\n");
			return -H_EIO;
		}

		for (ext2de = (ext2_dir_entry_t *)buf2;
		     ((addr_t)ext2de - (addr_t)buf2) <=
			     (EXT2_BLOCK_SIZE(sb) - 12);
		     (ext2de = (ext2_dir_entry_t *)(((addr_t)ext2de) +
						    ext2de->rec_len))) {
			if (!ext2de->rec_len)
				break;

			if (ext2de->name_len != 4)
				continue;

			if (!memcmp (ext2de->name, "boot", 4))
				goto boot_found;
		}
	}

	if ((ext2inode->i_block[12] || ext2inode->i_block[13] ||
	     ext2inode->i_block[14])) {
		dev_err("Not support Indirect blocks.\n");
		return -H_ENOSYS;
	}

	dev_err("Can't find \"/boot\"\n");
	return -H_EIO;

boot_found:
	dev_dbg("\"/boot\" is found.\n");

	group = (ext2de->inode - 1) / inodes_per_group;
	inode = (ext2de->inode - 1) % inodes_per_group;
	ext2gd = ext2_get_group_desc(disk, file, sb, group, buf);
	inode_table_sector = (file->partition_start +
			      sectors_per_block * ext2gd->bg_inode_table +
			      inode / (SECTOR_SIZE >> ffs(inode_size)));
	ret = gendisk_sector_read(disk, inode_table_sector, 1, buf);
	if (ret < 0) {
		dev_err("Inode table read error.\n");
		return -H_EIO;
	}
	ext2inode = (ext2_inode_t *)
		(buf + inode_size * (inode % (SECTOR_SIZE >> ffs(inode_size))));
	if ((ext2inode->i_mode & 0xf000) != 0x4000) {
		dev_err("\"/boot\" is not directory. (%p)\n",
			ext2inode->i_mode);
		return -H_EIO;
	}
	dev_dbg("\"/boot\" is directory.\n");

	for (i=0; i<12; i++) {
		if (!ext2inode->i_block[i])
			break;
		dir_entry_table_sector = (
			file->partition_start +
			sectors_per_block * ext2inode->i_block[i]);
		ret = gendisk_sector_read(disk, dir_entry_table_sector,
					  sectors_per_block, buf2);
		if (ret < 0) {
			dev_err("/boot directory entry read error.\n");
			return -H_EIO;
		}

		for (ext2de = (ext2_dir_entry_t *)buf2;
		     ((addr_t)ext2de - (addr_t)buf2) <=
			     (EXT2_BLOCK_SIZE(sb) - 16);
		     ext2de = (ext2_dir_entry_t *)(((addr_t)ext2de) +
						   ext2de->rec_len)) {
			if (!ext2de->rec_len)
				break;

			if (ext2de->name_len < 5)
				continue;

			ext = 0;
			if (!memcmp(ext2de->name, "Image", 5) ||
			    !memcmp(ext2de->name, "linux", 5)) {
				if (ext2de->name_len == 5)
					goto kernel_found;

				if (ext2de->name_len >= 9) {
					if (!memcmp(ext2de->name + 5,
						    ".bin", 4)) {
						ext = 4;
						if (ext2de->name_len == 9)
							goto kernel_found;
					}
				}
				if (ext2de->name_len == 5 + ext + 3) {
					if (!memcmp(ext2de->name + 5 + ext,
						    ".gz", 3)) {
						file->compressed = 1;
						goto kernel_found;
					}
				}
			}
		}
	}

	if ((ext2inode->i_block[12] || ext2inode->i_block[13] ||
	     ext2inode->i_block[14])) {
		    dev_err("Not support Indirect blocks.\n");
		    return -H_ENOSYS;
	}

	dev_err("Can't find /boot/Image(or linux)(.bin)(.gz)\n");
	return -H_EIO;

kernel_found:
	memcpy (file->name, ext2de->name, ext2de->name_len);
	file->name[ext2de->name_len] = '\0';
	dev_dbg("\"/boot/%s\" is found.\n", file->name);

	group = (ext2de->inode - 1) / inodes_per_group;
	inode = (ext2de->inode - 1) % inodes_per_group;
	ext2gd = ext2_get_group_desc(disk, file, sb, group, buf);
	inode_table_sector = (file->partition_start +
			      sectors_per_block * ext2gd->bg_inode_table +
			      inode / (SECTOR_SIZE >> ffs(inode_size)));
	ret = gendisk_sector_read(disk, inode_table_sector, 1, buf);
	if (ret < 0) {
		dev_err("Inode table read error.\n");
		return -H_EIO;
	}

	ext2inode = (ext2_inode_t *)(
		buf + inode_size * (inode % (SECTOR_SIZE >> ffs(inode_size))));
	if ((ext2inode->i_mode & 0xf000) != 0x8000) {
		dev_err("%s is not regular file.\n", file->name);
		return -H_EIO;
	}
	dev_dbg("\"/boot/%s\" is regular file.\n", file->name);

	file->size = ext2inode->i_size;
	if (file->size <= 0) {
		dev_err("\"/boot/%s\" is 0 byte.\n", file->name);
		return -H_EIO;
	}
	dev_dbg("\"/boot/%s\" is %d bytes.\n", file->name, file->size);

	for (i = 0; i < 15; i++)
		file->blocks[i] = ext2inode->i_block[i];

	return 0;
}
