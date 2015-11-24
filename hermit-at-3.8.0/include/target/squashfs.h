#ifndef _HERMIT_TARGET_FS_SQUASHFS_H_
#define _HERMIT_TARGET_FS_SQUASHFS_H_

extern int is_squashfs(addr_t addr);
extern int squashfs_copy_to(char *label, addr_t to, addr_t from,
			    size_t max_length);

#endif
