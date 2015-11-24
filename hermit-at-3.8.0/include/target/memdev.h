#ifndef _HERMIT_TARGET_MEMDEV_H_
#define _HERMIT_TARGET_MEMDEV_H_

#include <htypes.h>
#include <list.h>

struct memory_device {
	/* device */
	const char *name;
	u32 type;
#define MEMDEV_TYPE_CHAR	0x00
#define MEMDEV_TYPE_BLOCK	0x01
	u32 blksize;

	int (*read)(struct memory_device *memdev,
		    u32 addr, u32 size, void *buf);
	int (*write)(struct memory_device *memdev,
		     u32 addr, u32 size, void *buf);
	int (*erase)(struct memory_device *memdev, u32 addr, u32 size);

	/* common */
	char *label;
	struct {
		u32 start;
		u32 size;
	} res;

	/* private */
	struct list list;
};

#define memdev_add(TYPE, MEMDEV, LABEL, START, SIZE)	\
({							\
	struct memory_device *dev = MEMDEV;		\
	dev->res.start = (START);			\
	dev->res.size = (SIZE);				\
	dev->label = LABEL;				\
	TYPE##_memdev_init(dev);			\
	memdev_register_device(dev);			\
})

extern struct memory_device *find_memdev(char *label);
extern int memdev_read(struct memory_device *memdev,
		       u32 offset, u32 size, void *buf);
extern int memdev_write(struct memory_device *memdev,
			u32 offset, u32 size, void *buf);
extern int memdev_erase(struct memory_device *memdev,
			u32 offset, u32 size);
extern int memdev_register_device(struct memory_device *memdev);

#endif /* _HERMIT_TARGET_MEMDEV_H_ */
