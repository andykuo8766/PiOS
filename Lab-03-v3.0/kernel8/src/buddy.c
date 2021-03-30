#include "buddy.h"



void init_buddy_system(){
	for(int i=0;i<BUDDY_MAX_ORDER ;i++){
		INIT_LIST_HEAD(&page_buddy[i]);
	}
	int remain_counter = 0;
    for(int p = 0; p < TOTAL_PAGE_NUMBER; p++){
        page_t_pool[p].used = NOT_USED;
        page_t_pool[p].order = 0;
        page_t_pool[p].page_index = p;
        page_t_pool[p].phy_addr = __buddy_start + (p * BUDDY_PAGE_SIZE) ;	

        INIT_LIST_HEAD(&(page_t_pool[p].list));

        if(remain_counter == 0){
            if((p + TOTAL_PAGE_NUMBER) <= TOTAL_PAGE_NUMBER){
                remain_counter = TOTAL_PAGE_NUMBER-1;
                uart_puts("head value is:");
				uart_put_int(p);
				uart_puts("\n");
                uart_puts("end value is:");
				uart_put_int(p+remain_counter);
				uart_puts("\n");
				uart_puts("remain_counter is : ");
				uart_put_int(remain_counter);
				uart_puts("\n");
				uart_puts("addr is : 0x");
				uart_hex(page_t_pool[p].phy_addr);
				uart_puts("\n");	
				uart_puts("page struct addr is : 0x");
				uart_hex(&page_t_pool[p]);
				uart_puts("\n");	
                list_add_tail(&(page_t_pool[p].list),&page_buddy[BUDDY_MAX_ORDER-1]);
                page_t_pool[p].order = (BUDDY_MAX_ORDER-1);
            }
            else{
                list_add_tail(&(page_t_pool[p].list),&page_buddy[0]);
                page_t_pool[p].order = 0;
            }
        }
        else{
            page_t_pool[p].order = -1;
            list_add_tail(&(page_t_pool[p].list),&page_buddy[BUDDY_MAX_ORDER-1]);
            remain_counter--;
        }
    }
}
struct page* get_pages_from_list(int order){
    struct page* alloc_page;
    int o = 0;
    for(o = order; o < BUDDY_MAX_ORDER; o++){
        if(!list_empty(&page_buddy[o])){
            alloc_page = list_entry(page_buddy[o].next, struct page, list);

            struct list_head* next_buddy =  (&(BUDDY_END(alloc_page, o)->list))->next;
            //printf("-----------------\n");
            //printf("request buddy order: %d\n",order);
            //printf("want to allocate buddy order: %d\n",alloc_page->order);
            //printf("want to allocate buddy used info: %d\n",alloc_page->used);
            //printf("want to allocate buddy addr: %x\n",alloc_page->phy_addr);
            //printf("want to allocate buddy index: %d\n",alloc_page->page_index);
            //printf("want to allocate buddy total page number: %d\n",(1 << alloc_page->order));
            //printf("page struct addr is : %x\n",&alloc_page);
            //printf("-----------------\n");

            next_buddy->prev = (&page_buddy[o]);
            page_buddy[o].next = next_buddy;

            break;
        }
    }

    if(o == BUDDY_MAX_ORDER){
        uart_puts("cannot have enough memory to allocate!\n");
        return 0;
    }

    alloc_page->order = order;
    if(o != order){
        //alloc_page = get_back_redundant_memory(alloc_page, o, order);
    }
    alloc_page->used = USED;
    //printf("allocate mem index is %d - %d\n", alloc_page->page_index, alloc_page->page_index + (1 << order) - 1);
    //printf("allocate mem used number is %d \n", alloc_page->used);
    //printf("allocate mem order is %d \n", alloc_page->order);
    return alloc_page;
}

void print_buddy_info(){
    for(int l = 0; l < BUDDY_MAX_ORDER; l++){
        if(!list_empty(&page_buddy[l])){
            struct list_head*tmp = page_buddy[l].next;
            struct page* page = list_entry(tmp, struct page ,list);
            while(1){
			    uart_puts("start now buddy order: ");
				uart_put_int(page->order);
				uart_puts("\n");
                // printf("start now buddy used info: %d\n",page->used);
                // printf("start now buddy addr: %x\n",page->phy_addr);
                // printf("start now buddy index: %d\n",page->page_index);
                struct page* next_buddy_end_page = BUDDY_END(page, l);

                struct page* page_next = list_entry((&(BUDDY_END(page, l))->list)->next, struct page ,list);
                if((&(BUDDY_END(page, l))->list)->next == (&page_buddy[l])){
                    break;
                }
                page = page_next;
            }
        }else{
			uart_puts("buddy system level empty value is: ");
			uart_put_int(l);
			uart_puts("\n");
        }
    }
}


void buddy_system(){
    
    //struct mem_zone *zone;
	uart_puts("start init\n");
	init_buddy_system();
    uart_puts("init done\n");
    struct page*  p = get_pages_from_list(5);
    uart_puts("------\n");

	/*
	buddy_system_init(&global_mem_block.zone,global_mem_block.pages,__buddy_start,MAX_PAGE);
	zone = &global_mem_block.zone;
    
	
	struct page *p;
    buddy_get_pages(zone, 8);
	
    p = buddy_get_pages(zone, 8);
	
	dump_print(zone);
	uart_puts("num free: ");
	uart_put_int(buddy_num_free_page(zone));
    uart_puts("\n");

    if (p) buddy_free_pages(zone, p);


	dump_print(zone);
	uart_puts("num free: ");
	uart_put_int(buddy_num_free_page(zone));
    uart_puts("\n");	


*/


}


