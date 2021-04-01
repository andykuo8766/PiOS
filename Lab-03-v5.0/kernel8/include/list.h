#ifndef __LIST_H
#define __LIST_H

struct list_head{
    struct list_head *next, *prev;
};
typedef struct list_head list_ptr_t;


#define LIST_POISON1  ((void *)0x00100100)
#define LIST_POISON2  ((void *)0x00200200)

void INIT_LIST_HEAD(struct list_head *list);

///////////////////////////////////////////////////////////////////////////////////////////////////
//add
void __list_add(struct list_head *new, struct list_head *prev, struct list_head *next);
void list_add(struct list_head *new, struct list_head *head);
void list_add_tail(struct list_head  *new, struct list_head *head);

///////////////////////////////////////////////////////////////////////////////////////////////////
//delete
void __list_del(struct list_head *prev, struct list_head *next);
void list_del(struct list_head *entry);

int list_empty(const struct list_head *head);

void list_add_chain(struct list_head *ch,struct list_head *ct,struct list_head *head);
void list_add_chain_tail(struct list_head *ch,struct list_head *ct,struct list_head *head);
void list_remove_chain(struct list_head *ch,struct list_head *ct);

#define container_of(ptr, type, member) ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define list_for_each(pos, head) for (pos = (head)->next; pos != (head); pos = pos->next)


#endif  /*_LIST_H */
