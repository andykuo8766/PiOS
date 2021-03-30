#ifndef	_BUDDY_H
#define	_BUDDY_H

#include "list.h"
#include "string.h"

#define __buddy_start 			0x10000000
#define __buddy_end 			0x20000000



#define TOTAL_PAGE_NUMBER       (1 << 12)

#define BUDDY_PAGE_SHIFT    	12
#define BUDDY_PAGE_SIZE     	(1 << 12)
#define BUDDY_MAX_ORDER     	9

#define BUDDY_END(now,order)	((now)+(1<<(order))-1)


enum {
	NOT_USED,
	USED,
};

struct page {
	int used;
	int order;
	int page_index;
	unsigned long phy_addr;
	struct list_head list;
};

struct page page_t_pool[TOTAL_PAGE_NUMBER];
struct list_head page_buddy[BUDDY_MAX_ORDER];






void init_buddy_system();
struct page* get_pages_from_list(int order);
void print_buddy_info();
void buddy_system();




#endif  /*_BUDDY_H */
