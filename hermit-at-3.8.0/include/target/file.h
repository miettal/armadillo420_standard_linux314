#ifndef _HERMIT_TARGET_FILE_H_
#define _HERMIT_TARGET_FILE_H_

struct file {
	/* fs */
	char name[16];
	u32 size;
	int compressed;
	u32 blocks[15];
	u32 sectors_per_block;

	/* driver */
	u32 load_addr;

	/* ide */
	u32 partition_start;
	u32 partition_size;
};

#endif
