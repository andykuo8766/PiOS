#ifndef	_MM_H
#define	_MM_H




#define KERNEL_VIRT_BASE        0xFFFF000000000000
#define PAGE_SIZE               4096
#define PAGE_FRAMES_NUM         (0x40000000 / PAGE_SIZE)
#define PAGE_MASK               ~0xFFF
#define MAX_BUDDY_ORDER         9 // 2^0 ~ 2^8 => 4k to 1MB
#define MAX_OBJ_ALLOCTOR_NUM    32
#define MAX_POOL_PAGES          16

#ifndef __ASSEMBLY__

#include "list.h"
#include "utils.h"
#include "sched.h"
#include "mbox.h"

enum booking_status {
    AVAL,
    USED,
};

struct buddy_t {
    unsigned  nr_free;
    struct list_head head;
};

struct page_t {
    enum booking_status used;
    int order;
    int idx; // debug used
    struct list_head list;
};

struct free_list {
    struct free_list *next;
};

struct pool_t {
    enum booking_status used;
    int obj_size;
    int obj_per_page;
    int obj_used;
    int page_used;
    unsigned long page_addr[MAX_POOL_PAGES];
    struct free_list* free;
};

/* Variables init in mm.c */
extern struct pool_t obj_allocator[MAX_OBJ_ALLOCTOR_NUM];
extern struct buddy_t free_area[MAX_BUDDY_ORDER];
extern struct page_t *page;
extern unsigned long remain_page;

/* Function in mm.c */
void mm_init();
void kfree(void* addr);
void* kmalloc(unsigned long size);
void fork_pgd(struct task_t* target, struct task_t* dest);
//void* map_page(struct task_t* task, unsigned long user_addr);
void page_reclaim(unsigned long pgd_phy);

// object allocator
int obj_alloc_register(unsigned long size);
void* obj_alloc_kernel(int token);
void obj_free(int token, void* virt_addr);

// buddy system
void buddy_info();
void buddy_init();
void buddy_push(struct buddy_t* bd, struct list_head* elmt);
void* buddy_alloc(int order);
void buddy_free(void* virt_addr);

// utility function
unsigned long user_addr_to_page_addr(unsigned long user_addr, unsigned long pgd);
unsigned long virtual_to_physical(unsigned long virt_addr);
unsigned long phy_to_pfn(unsigned long phy_addr);
struct page_t* find_buddy(int pfn, int order);

#endif /* __ASSEMBLY__ */



#endif  /*_BUDDY_H */
