#define CORE_NAME "mm"

//#define DEBUG
//#define TRACE_DEBUG
//#define VERBOSE_DEBUG
//#define DEBUG_DUMP

#include <hermit.h>
#include <herrno.h>
#include <htypes.h>
#include <list.h>

struct mm_struct {
	unsigned long start;
	unsigned long size;
	struct list list;
};

static unsigned long _mm_align;
static struct mm_struct *_mm_inused;
static struct mm_struct *_mm_free;

#define to_mm_struct(LIST) (struct mm_struct *)container_of( \
					LIST, struct mm_struct, list)
#define mm_align(SIZE, ALIGN) \
	(((SIZE) + (ALIGN) - 1) & ~((ALIGN) - 1))

#define MM_SIZE \
	mm_align(sizeof(struct mm_struct), _mm_align)

void dump_mm(char *str)
{
#if defined(DEBUG_DUMP)
	struct mm_struct *ptr;
	struct list *list;
	dev_dbg_r("== %s ==\n", str);
	if (_mm_inused) {
		dev_dbg_r("[INUSED]\n");
		list = &_mm_inused->list;
		while (list) {
			ptr = to_mm_struct(list);
			dev_dbg_r("  MM: 0x%p, SA.0x%p, SZ.0x%p\n",
				  (unsigned long)ptr, ptr->start, ptr->size);
			list = list->next;
		}
	}
	if (_mm_free) {
		dev_dbg_r("[FREE]\n");
		list = &_mm_free->list;
		while (list) {
			ptr = to_mm_struct(list);
			dev_dbg_r("  MM: 0x%p, SA.0x%p, SZ.0x%p\n",
				  (unsigned long)ptr, ptr->start, ptr->size);
			list = list->next;
		}
	}
	dev_dbg_r("=======================================================\n");
#endif
}

int mm_init(unsigned long base, unsigned long size, unsigned long align)
{
	trace();

	if (size < sizeof(struct mm_struct) || size < align)
		return -H_EINVAL;

	_mm_align = align;
	_mm_inused = NULL;
	_mm_free = (struct mm_struct *)base;
	_mm_free->start = base + MM_SIZE;
	_mm_free->size = size - MM_SIZE;

	list_init(&_mm_free->list);

	return 0;
}

static struct mm_struct *mm_use(struct mm_struct *free, unsigned long size)
{
	struct mm_struct *new;

	new = (struct mm_struct *)free->start;
	new->start = free->start + MM_SIZE;
	new->size = size;

	if (!_mm_inused) {
		_mm_inused = new;
		list_init(&new->list);
	} else {
		list_add_tail(&_mm_inused->list, &new->list);
	}

	free->start += (MM_SIZE + size);
	free->size -= (MM_SIZE + size);

	return new;
}

static void mm_defrag(struct mm_struct *free)
{
	struct list *list;
	struct mm_struct *prev, *next;

	list = &free->list;
	do {
		if (list->next) {
			prev = to_mm_struct(list);
			next = to_mm_struct(list->next);
			dev_vdbg("prev->start + prev->size = %p, "
				 "next->start = %p\n",
				 prev->start + prev->size , next->start);
			if (prev->start + prev->size == next->start) {
				next->start -= MM_SIZE + prev->size;
				next->size += MM_SIZE + prev->size;
				list_del(&_mm_free->list, &prev->list);
				if (&prev->list == &_mm_free->list) {
					if (!prev->list.next)
						_mm_free = NULL;
					else
						_mm_free = to_mm_struct(prev->list.next);
				}
				list = next->list.prev;
				continue;
			}
		}
		break;
	} while (list);
}

static void mm_free(struct mm_struct *free)
{
	struct mm_struct *prev, *next;
	struct list *list;

	dev_vdbg("%s(): free %p, (%p, %p)\n",
		 __func__, free, free->start, free->size);
	list_del(&_mm_inused->list, &free->list);
	if (&free->list == &_mm_inused->list) {
		if (!free->list.next)
			_mm_inused = NULL;
		else
			_mm_inused = to_mm_struct(free->list.next);
	}
	list_add_head(&_mm_free->list, &free->list);
	_mm_free = free;

	/* one-dimensional bubble sort */
	list = &_mm_free->list;
	do {
		if (list->next) {
			prev = to_mm_struct(list);
			next = to_mm_struct(list->next);
			if (prev->start > next->start) {
				if (list == &_mm_free->list)
					_mm_free = next;
				list_swap(list, list->next);
				continue;
			}
		}
		list = list->next;
	} while (list);

	mm_defrag(free);
}

void *hmalloc(unsigned long size)
{
	struct list *list;
	struct mm_struct *ptr;
	struct mm_struct *new;
	unsigned long new_size = mm_align(size, _mm_align);

	if (!_mm_free)
		return NULL;

	dev_vdbg("%s(): size=%d, new_size=%d\n", __func__, size, new_size);

	list = &_mm_free->list;
	while (list) {
		ptr = to_mm_struct(list);
		if (ptr->size >= (MM_SIZE + new_size)) {
			new = mm_use(ptr, new_size);
			return (void *)new->start;
		}
		list = list->next;
	}
	return NULL;
}

void hfree(void *addr)
{
	struct list *list = &_mm_inused->list;
	struct mm_struct *ptr;

	while (list) {
		ptr = to_mm_struct(list);
		dev_vdbg("%s(): ptr->start=%p, addr=%p\n",
			 __func__, ptr->start, addr);
		if (ptr->start == (unsigned long)addr) {
			mm_free(ptr);
			return;
		}
		list = list->next;
	}
}
