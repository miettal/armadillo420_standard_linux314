#define CORE_NAME "list"

#include <hermit.h>
#include <herrno.h>
#include <htypes.h>
#include <list.h>

void dump_list(struct list *head)
{
	struct list *list = head;
	while (list) {
		dev_dbg_r("LIST: 0x%p, P.0x%p, N.0x%p\n",
			  (unsigned long)list,
			  (unsigned long)list->prev,
			  (unsigned long)list->next);
		if (list == list->next) {
			dev_dbg_r("  dump error\n");
			return;
		}
		list = list->next;
	}
}

void list_init(struct list *head)
{
	head->prev = NULL;
	head->next = NULL;
}

struct list *list_head(struct list *head)
{
	struct list *list = head;

	while (list->prev)
		list = list->prev;

	return list;
}

struct list *list_tail(struct list *head)
{
	struct list *list = head;

	while (list->next)
		list = list->next;

	return list;
}

int list_add(struct list *head, struct list *add)
{
	if (!head)
		return -H_EINVAL;

	add->prev = head;
	if (head->next) {
		head->next->prev = add;
		add->next = head->next;
	} else {
		add->next = NULL;
	}
	head->next = add;

	return 0;
}

int list_add_head(struct list *list, struct list *add)
{
	struct list *head;

	if (!list || !add)
		return -H_EINVAL;

	head = list_head(list);
	add->prev = NULL;
	add->next = head;
	head->prev = add;

	return 0;
}

int list_add_tail(struct list *list, struct list *add)
{
	struct list *tail;

	if (!list || !add)
		return -H_EINVAL;

	tail = list_tail(list);
	return list_add(tail, add);
}

int list_del(struct list *head, struct list *del)
{
	struct list *list;

	if (!head || !del)
		return -H_EINVAL;

	list = list_head(head);
	while (list) {
		if (list == del) {
			if (list->prev)
				list->prev->next = del->next;
			if (del->next)
				del->next->prev = del->prev;
			return 0;
		}
		list = list->next;
	}

	return -H_EINVAL;
}

int list_swap(struct list *l1, struct list *l2)
{
	struct list tmp;

	if (!l1 || !l2 || l1->next != l2 || l2->prev != l1)
		return -H_EINVAL;

	tmp.next = l2->next;

	if (l1->prev)
		l1->prev->next = l2;
	if (l2->next)
		l2->next->prev = l1;

	l2->prev = l1->prev;
	l2->next = l1;
	l1->prev = l2;
	l1->next = tmp.next;

	return 0;
}
