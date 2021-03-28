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




static void __SetPageHead(struct page *page){
    page->flags |= (1UL<<PG_head);
}
static void __SetPageTail(struct page *page){
    page->flags |= (1UL<<PG_tail);
}
static void __SetPageBuddy(struct page *page){
    page->flags |= (1UL<<PG_buddy);
}


/**/
static void __ClearPageHead(struct page *page){
    page->flags &= ~(1UL<<PG_head);
}
static void __ClearPageTail(struct page *page){
    page->flags &= ~(1UL<<PG_tail);
}
static void __ClearPageBuddy(struct page *page){
    page->flags &= ~(1UL<<PG_buddy);
}

/**/
static int PageHead(struct page *page){
    return (page->flags & (1UL<<PG_head));
}
static int PageTail(struct page *page){
    return (page->flags & (1UL<<PG_tail));
}
static int PageBuddy(struct page *page){
    return (page->flags & (1UL<<PG_buddy));
}

/*
 * 
 * */
static void set_page_order_buddy(struct page *page, unsigned long order){
    page->order = order;
    __SetPageBuddy(page);
}
static void rmv_page_order_buddy(struct page *page){
    page->order = 0;
    __ClearPageBuddy(page);
}

/*
 * 
 * */
static unsigned long __find_buddy_index(unsigned long page_idx, unsigned int order){
    return (page_idx ^ (1 << order));
}

static unsigned long __find_combined_index(unsigned long page_idx, unsigned int order){
    return (page_idx & ~(1 << order));
}

/*
 * 
 * */
static unsigned long compound_order(struct page *page){
    if (!PageHead(page))
        return 0; //等珜
    //return (unsigned long)page[1].lru.prev;
    return page->order;
}
static void set_compound_order(struct page *page, unsigned long order){
    //page[1].lru.prev = (void *)order;
    page->order = order;
}
static void BUDDY_BUG(char *f, int line){
	uart_puts("BUDDY_BUG in ");
	uart_puts(f);
	uart_puts(", ");
	uart_put_int(line);
	uart_puts(".\n");
}
void dump_print(struct mem_zone *zone);


#endif  /*_BUDDY_H */
