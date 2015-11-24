#define CORE_NAME "memdev"

//#define DEBUG
//#define TRACE_DEBUG
//#define VERBOSE_DEBUG

#include <hermit.h>
#include <herrno.h>
#include <init.h>
#include <string.h>
#include <list.h>
#include <memdev.h>

static struct list listhead = {NULL, NULL};

#define to_memdev(LIST) (struct memory_device *)container_of( \
		LIST, struct memory_device, list)

struct memory_device *find_memdev(char *label)
{
	struct memory_device *memdev = NULL;
	struct list *list = listhead.next;

	trace();

	while (list) {
		memdev = to_memdev(list);
		list = list->next;

		if (!memdev)
			continue;

		if (strcmp(memdev->label, label) == 0)
			return memdev;
	}
	return NULL;
}

int memdev_read(struct memory_device *memdev, u32 offset, u32 size, void *buf)
{
	if (!memdev)
		return -H_EINVAL;
	if (!memdev->read)
		return -H_ENOSYS;

	return memdev->read(memdev, memdev->res.start + offset, size, buf);
}

int memdev_write(struct memory_device *memdev, u32 offset, u32 size, void *buf)
{
	if (!memdev)
		return -H_EINVAL;
	if (!memdev->write)
		return -H_ENOSYS;

	return memdev->write(memdev, memdev->res.start + offset, size, buf);
}

int memdev_erase(struct memory_device *memdev, u32 offset, u32 size)
{
	if (!memdev)
		return -H_EINVAL;
	if (!memdev->erase)
		return -H_ENOSYS;

	return memdev->erase(memdev, memdev->res.start + offset, size);
}

int memdev_register_device(struct memory_device *memdev)
{
	trace();

	if (!memdev || !memdev->label)
		return -H_EINVAL;

	list_init(&memdev->list);
	list_add_tail(&listhead, &memdev->list);

	return 0;
}
