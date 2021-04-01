#include "list.h"

void init_list(list_head *l) {
  l->bk = l;
  l->fd = l;
}

void push_list(list_head *l,list_head *chunk) {
  chunk->bk = l;
  chunk->fd = l->fd;
  l->fd->bk = chunk;
  l->fd = chunk;
}

void pop_list(list_head *chunk) {
  chunk->fd->bk = chunk->bk;
  chunk->bk->fd = chunk->fd;
}

int list_empty(list_head *l) { 
	return l->fd == l; 
}



