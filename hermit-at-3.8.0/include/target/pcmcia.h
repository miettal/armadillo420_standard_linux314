#ifndef _HERMIT_TARGET_PCMCIA_CORE_H_
#define _HERMIT_TARGET_PCMCIA_CORE_H_

#include <ide.h>

typedef struct {
	u32 mem_base;
	u32 attr_base;
	u32 io_base;
} pcmcia_info_t;

typedef struct {
	int (*hw_init)(pcmcia_info_t *info);
	int (*hw_free)(void);
	int (*set_socket)(int power);
	int (*get_status)(void);
	int (*set_io_map)(int map);
	int (*set_mem_map)(int map);
} pcmcia_ops_t;

extern int pcmcia_probe(struct ide_device *dev, int drive);
extern void pcmcia_remove(struct ide_device *dev);

#endif
