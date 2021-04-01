#ifndef	_BUDDY_H
#define	_BUDDY_H
#include "list.h"
#include "utils.h"
#include "cpio.h" //include align

#define NULL                    ((void *)0) 

#define PAGE_SHIFT	 		    12
#define TABLE_SHIFT 			9
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   			(1 << PAGE_SHIFT)	
#define SECTION_SIZE			(1 << SECTION_SHIFT)	

#define EL1_LOW_MEMORY          (2 * SECTION_SIZE)
#define EL0_LOW_MEMORY          (4 * SECTION_SIZE)

#define PAGE_SIZE               (1 << PAGE_SHIFT) // 2^12 = 4K

#define HIGH_PAGE_POOL_MEMORY   LOW_PAGE_POOL_MEMORY + TOTAL_PAGE_NUMBER*PAGE_SIZE
#define LOW_PAGE_POOL_MEMORY    0xf00000

#define MAX_ORDER 9

// #define TOTAL_PAGE_NUMBER       ((HIGH_PAGE_POOL_MEMORY-LOW_PAGE_POOL_MEMORY)/PAGE_SIZE)
#define TOTAL_PAGE_NUMBER       2048
#define MAX_BUDDY_PAGE_NUMBER   (1 << (MAX_ORDER-1))//256

#define NEXT_BUDDY_START(now,order)	((now)+(1<<(order)))
#define PREV_BUDDY_START(now,order)	((now)-(1<<(order)))
#define BUDDY_END(now,order)	((now)+(1<<(order))-1)
#define SLUB_INDEX_TO_SIZE(slub_index)	(1 << (slub_index+4))
#define SLUB_NUMBER             9 // 16,32,48,96

enum {
	NOT_USED,
	USED,
};

struct page {
	int used;
	int order;
	int page_index;
	unsigned long phy_addr;
	struct slub* slub_next;
    int slub_num;
    int slub_index;
	list_ptr_t list;
};
struct slub {
	struct slub* next;
};

struct kmem_cache_cpu{
	struct slub* free_list;
	struct page* page;
	list_ptr_t partial;
};

struct kmem_cache{
	struct kmem_cache_cpu cache_cpu;
};

typedef struct page page_t;
typedef struct slub slub_t;
typedef struct kmem_cache kmem_cache_t;
typedef struct kmem_cache_cpu kmem_cache_cpu_t;


page_t 		page_t_pool[TOTAL_PAGE_NUMBER];
list_ptr_t 	page_buddy[MAX_ORDER];
kmem_cache_t kmem_cache_arr[SLUB_NUMBER]; 

void memzero(unsigned long src, unsigned long n);

void init_buddy_sys();

page_t* get_back_redundant_memory(page_t* alloc_page, int get_page_level, int req_page_level);
page_t* get_pages_from_list(int order);
void give_back_pages(page_t* page , int order);
void free_page(unsigned long physical_addr);
page_t *alloc_pages(int order);
unsigned long allocate_memory(int size);
unsigned long give_slab(int size);
int slub_size_to_index(int size);
void split_page_to_slub(page_t* page, int slub_index);
void init_kmalloc_caches();
unsigned long give_one_slub(int slub_index);
page_t* find_partial_free_slub(int slub_index);
void receive_memory(unsigned long physical_addr);
void receive_slub(unsigned long physical_addr);
void init_page(int page_index);

void print_buddy_info();
void print_slub_info();
void init_page_sys();



#endif  /*_BUDDY_H */
