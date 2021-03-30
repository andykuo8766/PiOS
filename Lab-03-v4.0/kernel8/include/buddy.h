#ifndef	_BUDDY_H
#define	_BUDDY_H
#include "list.h"
#include "utils.h"
#include "cpio.h" //include align

#define BUDDY_START 		0x10000000
#define BUDDY_END 			0x40000000

#define PAGE_SIZE 			(1 << 12)
#define BUDDY_ARRAY_SIZE 	((BUDDY_END - BUDDY_START) / PAGE_SIZE)
#define BUDDY_INDEX 		20

typedef struct buddy_list {
  struct buddy_list *next;
  char *addr;
  int size;
  
} buddy_list;

buddy_list buddy[BUDDY_ARRAY_SIZE];
buddy_list *free_list[BUDDY_INDEX];
buddy_list *used_list[BUDDY_INDEX];

typedef struct dma {
  struct dma *next;
  buddy_list *page;
  int size;
} dma;
dma *dma_dummy[6];  // align for 8
dma *free_pool, *used_pool;




int size2Index(int size);
buddy_list *__buddy_merge__(buddy_list *now, buddy_list *next,buddy_list **before);
void buddy_free(buddy_list *list);
void *__dma_merge__(dma *now, dma *next, dma **before);
void free(void *addr);
void printDmaPool(dma *list);
void print_buddyList(buddy_list **lists);
buddy_list *buddy_alloc(int size);

void buddy_init(char *mem_start);
void dma_init();


void buddy_test();
void dma_test();



#endif  /*_BUDDY_H */
