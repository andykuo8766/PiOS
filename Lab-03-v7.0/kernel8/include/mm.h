#ifndef	_MM_H
#define	_MM_H
#include "list.h"
#include "utils.h"




#define FRAME_BASE 					0x10000000
#define FRAME_END  					0x20000000

#define FRAME_ARRAY_SIZE 			((FRAME_END - FRAME_BASE) / PAGE_SIZE)
#define PAGE_SIZE 					4096
#define FRAME_BINS 					9
#define MAX_ORDER 					(FRAME_BINS - 1)
#define FRAME_SIZE_MAX 				(PAGE_SIZE * (1 << (MAX_ORDER)))


/* we dont need free tag */
#define FRAME_FREE 0x8
#define FRAME_INUSE 0x4
#define FRAME_MEM_CACHE 0x2

#define IS_INUSE(flag) ((flag).flags & FRAME_INUSE)
#define IS_MEM_CACHE(flag) ((flag).flags & FRAME_MEM_CACHE)

/* cache start from 32 bytes to 2048 bytes */
#define CACHE_BINS 7
#define CACHE_MIN_SIZE 32
#define CACHE_MAX_ORDER (CACHE_BINS - 1)

struct frame_flag {
    unsigned char flags;
    unsigned char order;
    unsigned short refcnt;
    unsigned char cache_order;
};

struct cache {
    struct cache *next;
};

struct frame_flag frame[FRAME_ARRAY_SIZE];
struct list_head frame_bins[FRAME_BINS];
struct cache *cache_bins[CACHE_BINS];


void init_buddy();
void *kmalloc(unsigned int size);
void kfree(void *ptr);
void print_buddy_info();


#endif  /*_BUDDY_H */
