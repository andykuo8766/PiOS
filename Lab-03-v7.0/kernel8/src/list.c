#include "list.h"

void list_init(struct list_head *list) {
    list->next = list;
    list->prev = list;
}

void insert_head(struct list_head *head, struct list_head *new) {
    new->next = head->next;
    new->prev = head;
    head->next->prev = new;
    head->next = new;
}

void insert_tail(struct list_head *head, struct list_head *new) {
    new->next = head;
    new->prev = head->prev;
    head->prev->next = new;
    head->prev = new;
}

struct list_head *remove_head(struct list_head *head) {
    struct list_head *ptr;
    ptr = head->next;
    head->next = head->next->next;
    head->next->prev = head;

    return ptr;
}

struct list_head *remove_tail(struct list_head *head) {
    struct list_head *ptr;
    ptr = head->prev;
    head->prev = head->prev->prev;
    head->prev->next = head;

    return ptr;
}

void del_link(struct list_head *node) {
    struct list_head *next, *prev;
    next = node->next;
    prev = node->prev;
    next->prev = prev;
    prev->next = next;
}



