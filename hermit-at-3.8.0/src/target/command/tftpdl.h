#ifndef _HERMIT_TARGET_PLATFORM_H_
#define _HERMIT_TARGET_PLATFORM_H_

typedef struct tftpdl_download_info {
	/* from regions[] */
	char label[70];
	char *region;
	u32 flash_addr;
	u32 flash_size;

	/* */
	char *file;
	u32 file_size;
	u32 ram_addr;
} dl_file_t;

typedef struct tftpdl_option_info {
	u8 client_ipaddr[4];
	u8 server_ipaddr[4];
	u8 fake;

	int nr_dl_files;
	dl_file_t *dl_file;
} app_opt_info_t;

#endif
