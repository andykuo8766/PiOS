#ifndef __LIST_H
#define __LIST_H

typedef struct list_head {
  struct list_head *fd;
  struct list_head *bk;
} list_head;

void list_init(list_head *list);

void push_list(list_head *l, list_head *chunk);

void pop_list(list_head *chunk);

int list_empty(list_head *l) ;


#endif  /*_LIST_H */
