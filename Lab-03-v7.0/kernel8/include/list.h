#ifndef __LIST_H
#define __LIST_H

struct list_head{
    struct list_head *next, *prev;
};


void list_init(struct list_head *list);

void insert_head(struct list_head *head, struct list_head *new);
void insert_tail(struct list_head *head, struct list_head *new);

struct list_head *remove_head(struct list_head *head);
struct list_head *remove_tail(struct list_head *head);

void del_link(struct list_head *node);


#endif  /*_LIST_H */
