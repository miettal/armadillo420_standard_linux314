#ifndef _HERMIT_TARGET_FS_EXT2_H_
#define _HERMIT_TARGET_FS_EXT2_H_

#include <htypes.h>
#include <gendisk.h>
#include <file.h>

#define EXT2_GOOD_OLD_INODE_SIZE	(128)

#define EXT2_MIN_BLOCK_LOG_SIZE		(10)
#define EXT2_MIN_BLOCK_SIZE		(1 << EXT2_MIN_BLOCK_LOG_SIZE)
#define	EXT2_MAX_BLOCK_SIZE		(4096)
#define EXT2_BLOCK_SIZE(sb)		\
	(EXT2_MIN_BLOCK_SIZE << (sb)->s_log_block_size)
#define EXT2_BLOCK_SIZE_BITS(sb)	\
	(EXT2_MIN_BLOCK_LOG_SIZE + (sb)->s_log_block_size)
#define EXT2_DESC_SIZE			(sizeof(ext2_group_desc_t))
#define EXT2_DESC_PER_BLOCK(sb) 	\
	(EXT2_BLOCK_SIZE(sb) / EXT2_DESC_SIZE)
#define EXT2_DESC_PER_BLOCK_BITS(sb)	(fls(EXT2_DESC_PER_BLOCK(sb)))

typedef struct {
	u32	s_inodes_count;
	u32	s_blocks_count;
	u32	s_r_blocks_count;
	u32	s_free_blocks_count;
	u32	s_free_inodes_count;
	u32	s_first_data_block;
	u32	s_log_block_size;
	u32	s_log_frag_size;
	u32	s_blocks_per_group;
	u32	s_frags_per_group;
	u32	s_inodes_per_group;
	u32	s_mtime;
	u32	s_wtime;
	u16	s_mnt_count;
	u16	s_max_mnt_count;
	u16	s_magic;
	u16	s_state;
	u16	s_errors;
	u16	s_minor_rev_level;
	u32	s_lastcheck;
	u32	s_checkinterval;
	u32	s_creator_os;
	u32	s_rev_level;
	u16	s_def_resuid;
	u16	s_def_resgid;
	u32	s_first_ino;
	u16	s_inode_size;
	u16	s_block_group_nr;
	u32	s_feature_compat;
	u32	s_feature_incompat;
	u32	s_feature_ro_compat;
	u8	s_uuid[16];
	char	s_volume_name[16];
	char	s_last_mounted[64];
	u32	s_algorithm_usage_bitmap;
	u8	s_prealloc_blocks;
	u8	s_prealloc_dir_blocks;
	u16	s_padding1;
	/* Journaling support valid if EXT3_FEATURE_COMPAT_HAS_JOURNAL set. */
	u8	s_journal_uuid[16];
	u32	s_journal_inum;
	u32	s_journal_dev;
	u32	s_last_orphan;
	u32	s_hash_seed[4];
	u8	s_def_hash_version;
	u8	s_reserved_char_pad;
	u16	s_reserved_word_pad;
	u32	s_default_mount_opts;
	u32	s_first_meta_bg;
	u32	s_reserved[190];
} __attribute__((packed)) ext2_super_block_t;

typedef struct {
	u32	bg_block_bitmap;
	u32	bg_inode_bitmap;
	u32	bg_inode_table;
	u16	bg_free_blocks_count;
	u16	bg_free_inodes_count;
	u16	bg_used_dirs_count;
	u16	bg_pad;
	u32	bg_reserved[3];
} __attribute__((packed)) ext2_group_desc_t;

typedef struct {
	u16	i_mode;
	u16	i_uid;
	u32	i_size;
	u32	i_atime;
	u32	i_ctime;
	u32	i_mtime;
	u32	i_dtime;
	u16	i_gid;
	u16	i_links_count;
	u32	i_blocks;
	u32	i_flags;
	union {
		struct {
			u32	l_i_reserved1;
		} linux1;
		struct {
			u32	h_i_translator;
		} hurd1;
		struct {
			u32	m_i_reserved1;
		} masix1;
	} osd1;
	u32	i_block[15];
	u32	i_generation;
	u32	i_file_acl;
	u32	i_dir_acl;
	u32	i_faddr;
	union {
		struct {
			u8	l_i_frag;
			u8	l_i_fsize;
			u16	i_pad1;
			u16	l_i_uid_high;
			u16	l_i_gid_high;
			u32	l_i_reserved2;
		} linux2;
		struct {
			u8	h_i_frag;
			u8	h_i_fsize;
			u16	h_i_mode_high;
			u16	h_i_uid_high;
			u16	h_i_gid_high;
			u32	h_i_author;
		} hurd2;
		struct {
			u8	m_i_frag;
			u8	m_i_fsize;
			u16	m_pad1;
			u32	m_i_reserved2[2];
		} masix2;
	} osd2;
} __attribute__((packed)) ext2_inode_t;

typedef struct {
	u32	inode;
	u16	rec_len;
	u8	name_len;
	u8	file_type;
	char	name[255];
} __attribute__((packed)) ext2_dir_entry_t;



extern int ext2_find_image(struct gendisk *disk, struct file *file,
			   ext2_super_block_t *sb);

#endif
