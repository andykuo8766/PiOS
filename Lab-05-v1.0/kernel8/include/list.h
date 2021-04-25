#ifndef __LIST_H
#define __LIST_H

typedef struct list_head {
  struct list_head *next;
  struct list_head *prev;
} list_head_t;

void INIT_LIST_HEAD(list_head_t *list);
void __list_add(list_head_t *new_, list_head_t *prev, list_head_t *next);
void list_add(list_head_t *new_, list_head_t *head);
void list_add_tail(list_head_t *new_, list_head_t *head);
//concatenate list
void list_cat(list_head_t *new_h, list_head_t *new_t, list_head_t *head);
void list_cat_tail(list_head_t *new_h, list_head_t *new_t, list_head_t *head);
void list_crop(list_head_t *list_h, list_head_t *list_t);
int list_empty(const list_head_t *head);


#endif  /*_LIST_H */
