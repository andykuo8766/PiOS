#ifndef	_BUDDY_H
#define	_BUDDY_H

#include "list.h"
#include "string.h"

enum pageflags{
    PG_head,    
    PG_tail,    
    PG_buddy,   
};

extern unsigned long __buddy_start;
extern unsigned long __buddy_end;



#define MAX_PAGE                1 << 12

#define BUDDY_PAGE_SHIFT    	12
#define BUDDY_PAGE_SIZE     	1 << 12
#define BUDDY_MAX_ORDER     	10



struct page{
    // spin_lock        lock;
    struct list_head    lru;
    unsigned long       flags;
    union {
        unsigned long   order;
        struct page     *first_page;
    };
};

struct free_area{
    struct list_head    free_list;
    unsigned long       nr_free;
};

struct mem_zone{
    // spin_lock        lock;
    unsigned long       page_num;
    unsigned long       page_size;
    struct page        *first_page;
    unsigned long       start_addr;
    unsigned long       end_addr;
    struct free_area    free_area[BUDDY_MAX_ORDER];
};
void buddy_system_init(struct mem_zone *zone,
                          struct page *start_page,
                          unsigned long start_addr,
                          unsigned long page_num);
struct page *buddy_get_pages(struct mem_zone *zone,unsigned long order);
void buddy_free_pages(struct mem_zone *zone,struct page *page);
unsigned long buddy_num_free_page(struct mem_zone *zone);
void buddy_system();

void __SetPageHead(struct page *page);
void __SetPageTail(struct page *page);
void __SetPageBuddy(struct page *page);

/**/
void __ClearPageHead(struct page *page);
void __ClearPageTail(struct page *page);
void __ClearPageBuddy(struct page *page);

/**/
int PageHead(struct page *page);
int PageTail(struct page *page);
int PageBuddy(struct page *page);

/*
 * 
 * */
void set_page_order_buddy(struct page *page, unsigned long order);
void rmv_page_order_buddy(struct page *page);
/*
 * 
 * */
unsigned long __find_buddy_index(unsigned long page_idx, unsigned int order);
unsigned long __find_combined_index(unsigned long page_idx, unsigned int order);

unsigned long compound_order(struct page *page);
void set_compound_order(struct page *page, unsigned long order);

void BUDDY_BUG(char *f, int line);
void dump_print(struct mem_zone *zone);


#endif  /*_BUDDY_H */
