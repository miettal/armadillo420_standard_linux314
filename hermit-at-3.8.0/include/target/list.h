#ifndef _LIST_H_
#define _LIST_H_

struct list {
	struct list *prev;
	struct list *next;
};

extern void list_init(struct list *head);
extern int list_add(struct list *head, struct list *add);
extern int list_add_head(struct list *list, struct list *add);
extern int list_add_tail(struct list *head, struct list *add);
extern int list_del(struct list *head, struct list *del);
extern int list_swap(struct list *l1, struct list *l2);

#endif
