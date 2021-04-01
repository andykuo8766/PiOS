#ifndef	_MM_H
#define	_MM_H
#include "list.h"
#include "utils.h"

	

#define BUDDY_MAX 16    // 4kB ~ 128MB
#define STARTUP_MAX 16  // max reserve slot
#define PAGE_SIZE 4096
#define PAGE_SIZE_CTZ 12
#define pad(x, y) ((((x) + (y)-1) / (y)) * (y))

extern unsigned long long mem_size;
extern unsigned long long reserve_count;

void *kmalloc(unsigned long size);
void kfree(void *ptr);

void init_reserve();
void init_buddy(char *ps);
void init_slab();
void *register_slab(unsigned int size);

void *alloc_page(unsigned int size);
int reserve_mem(void *addr, unsigned long size);
void *alloc_slab(void *slab_tok);

void free_page(void *ptr);
void free_reserve(void *ptr);
void free_slab(void *ptr, void *slab);
void free_unknow_slab(void *ptr);

void log_buddy();
void check_buddy_stat();
void check_slab();

#define get_order(ptr) (__builtin_ctzl((unsigned long)ptr) - PAGE_SIZE_CTZ)
#define set_buddy_ord(bd, ord) (bd = ord | (bd & 0xe0))
#define set_buddy_flag(bd, flag) (bd = (flag << 5) | (bd & 0x1f))
#define get_buddy_ord(bd) (bd & 0x1f)
#define get_buddy_flag(bd) ((bd & 0xe0) >> 5)
#define ptr_to_pagenum(ptr) (((unsigned long)(ptr)) >> PAGE_SIZE_CTZ)
#define pagenum_to_ptr(pn) ((void *)((pn) << PAGE_SIZE_CTZ))
#define buddy_pagenum(pg, ord) ((pg) ^ (1 << ord))
#define BUDDY_FREE 0
#define BUDDY_USE 1
#define SLAB_USE 2
#define RESRVE_USE 3
#define INIT_PAGE 0x20

typedef struct buddy_system {
  list_head free_list[BUDDY_MAX];
} buddy_system;

typedef struct startup_allocator {
  void *addr[STARTUP_MAX];
  unsigned long long size[STARTUP_MAX];
} startup_allocator;

typedef struct cache_list {
  struct cache_list *next;
} cache_list;

typedef struct page_descriptor {
  void *page;
  struct page_descriptor *next_pd;
  cache_list *free_list;
  unsigned int free_count;
} page_descriptor;

static inline void *pop_cache(cache_list **cl) {
  void *addr = (void *)*cl;
  *cl = (*cl)->next;
  return addr;
}

static inline void push_cache(cache_list **cl, cache_list *new_chunk) {
  new_chunk->next = (*cl);
  (*cl) = new_chunk;
}

typedef struct slab_cache {
  struct slab_cache *next_slab;
  page_descriptor *head_pd;
  page_descriptor *cache_pd;
  void *page_slice_pos;
  unsigned int size;
  unsigned int free_count;
  unsigned int page_remain;
} slab_cache;

#endif  /*_BUDDY_H */
