#ifndef _HERMIT_TARGET_PLATFORM_H_
#define _HERMIT_TARGET_PLATFORM_H_

#include <command.h>
#include <boost.h>
#include <net/net.h>
#include <ide.h>
#include <mmcsd.h>
#include <linux.h>

struct platform_info {
	void (*prepare_normal_boot)(struct platform_info *pinfo);
	int (*is_autoboot)(struct platform_info *pinfo);
	void (*led_on)(struct platform_info *pinfo, u32 leds);
	void (*led_off)(struct platform_info *pinfo, u32 leds);
	void (*udelay)(struct platform_info *pinfo, u32 usecs);
	void (*mdelay)(struct platform_info *pinfo, u32 msecs);
	int (*get_mmu_page_tables)(struct platform_info *pinfo, u32 mode,
				   struct page_table **pt, int *nr_pt);
	int (*change_clock)(struct platform_info *pinfo, char *clock);
	void (*display_info)(struct platform_info *pinfo);
	void (*get_mac_address)(struct platform_info *pinfo, u8 *addr);
	void (*adjust_boot_args)(struct platform_info *pinfo,
				 int *argc, char *argv[]);
	void (*adjust_boot_device)(struct platform_info *pinfo, char *device);

	void *private_data;
	char default_mtdparts[CMDLINE_MAXLEN];
        char default_ramparts[CMDLINE_MAXLEN];
	struct memory_map *map;
	int machine_nr;
	char **support_clocks;
	u32 system_rev;
	u32 serial_num;
	struct boot_device *boot_device_table;

	struct net_device *net_dev;
	struct ide_device *ide_dev;
	struct mmcsd_device *mmcsd_dev;
};

extern struct platform_info platform_info;

#endif
