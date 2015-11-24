#ifndef _HERMIT_TARGET_LINUX_H_
#define _HERMIT_TARGET_LINUX_H_

typedef struct boot_device {
	char	*name;
	u8	major;
	u8	minor_start;
	u8	minor_inc;
} boot_device_t;

#endif
