#ifndef	_MM_H
#define	_MM_H
#include "list.h"
#include "utils.h"

#define PAGE_SHIFT				12

#define TOTAL_PAGE_NUMBER       4096
#define PAGE_SIZE               (1 << PAGE_SHIFT)	// 2^12 = 4K

#define MAX_ORDER               9
#define MAX_ORDER_SIZE          (1 << MAX_ORDER)


#define HIGH_MEMORY   			LOW_MEMORY + TOTAL_PAGE_NUMBER * PAGE_SIZE
#define LOW_MEMORY    			0x10000000

#define MAX_ALLOCATOR_NUM           10

#define MIN_ALLOCATOR_SIZE          8//we need to put the pointer of next free space in chunk


#define MIN_KMALLOC_ORDER           3
#define MAX_KMALLOC_ORDER           11
#define MIN_KMALLOC_SIZE            (1 << MIN_KMALLOC_ORDER+1)//16
#define MAX_KMALLOC_SIZE            (1 << MAX_KMALLOC_ORDER+1)//4096

#define FIND_BUDDY_PFN(pfn, order)       ((pfn) ^ (1<<(order)))
#define FIND_LBUDDY_PFN(pfn, order)      ((pfn) & (~(1<<(order))))
#define FIND_RBUDDY_PFN(pfn, order)      ((pfn) | (1<<(order)))
#define BUDDY_END(now,order)	((now)+(1<<(order))-1)

#define PFN_MASK                    0x0000FFFFFFFFF000
#define PHY_ADDR_TO_PFN(addr)       (((((unsigned long)(addr)) - LOW_MEMORY) & PFN_MASK) >> PAGE_SHIFT)


struct obj_alloc{
    struct page* curr_page;//current allocating page
    list_ptr_t partial;
    list_ptr_t full;
    list_ptr_t empty;
    unsigned int objsize;// size 0 for non-registered pool
};
typedef struct obj_alloc obj_alloc_t;

struct page{
    list_ptr_t list;
    // buddy
    int order;
    // object allocator
    obj_alloc_t* obj_alloc;
    unsigned int nr_obj;
    int nr_unused;
    void **obj_freelist;
    // general info
    int pfn;
    int used;
    unsigned long phy_addr;
};
typedef struct page page_t;

obj_alloc_t allocator_pool[MAX_ALLOCATOR_NUM];
page_t page_t_pool[TOTAL_PAGE_NUMBER];
list_ptr_t page_buddy[MAX_ORDER + 1];// +1 for order 0


void 	mm_init();//initialize memory system
void 	page_init();//initialize page system
void 	buddy_init();//initialize buddy system
void 	slab_init();//initialize slab system


void 	__init_obj_alloc(obj_alloc_t*alloc, unsigned int size);
void 	__init_obj_page(page_t* page, unsigned size);

page_t*	buddy_alloc(int order);
void 	buddy_free(page_t* block);

void*	kmalloc(unsigned long size);
void 	kfree(unsigned long block);

void 	print_buddy_info();
void 	print_alloc_info();





#endif  /*_BUDDY_H */
