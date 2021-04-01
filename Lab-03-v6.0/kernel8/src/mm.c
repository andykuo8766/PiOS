#include "mm.h"


void mm_init(){
    page_init();// must do before buddy or it will break the list
    buddy_init();
    slab_init();
	print_buddy_info();
	
	//struct page * page = buddy_alloc(3);
	//struct page * page2 = buddy_alloc(3);
	//print_buddy_info();
	//buddy_free(page);
	//print_buddy_info();
	print_alloc_info();
	unsigned long p = kmalloc(128);
	print_alloc_info();
	kfree(p);
    // __init_obj_allocator; // init bss should set them to size 0(inused)
}

void page_init(){
    for(int i=0; i<TOTAL_PAGE_NUMBER; i++){
        page_t_pool[i].pfn = i;
        page_t_pool[i].used = 0;
        page_t_pool[i].phy_addr = LOW_MEMORY + i*PAGE_SIZE;
        page_t_pool[i].order = -1;
        INIT_LIST_HEAD(&(page_t_pool[i].list));
    }
}

void buddy_init(){
    for(int i=0; i<MAX_ORDER+1; i++){
        INIT_LIST_HEAD(&page_buddy[i]);
    }
    for(int i=0; i<TOTAL_PAGE_NUMBER; i+=MAX_ORDER_SIZE){
        list_add_tail(&page_t_pool[i].list, &page_buddy[MAX_ORDER]);
    }
}

void slab_init(){
    for (unsigned long i = MIN_KMALLOC_ORDER; i<=MAX_KMALLOC_ORDER; i++){
		unsigned long objsize = 1<<i;
		__init_obj_alloc(&allocator_pool[i], objsize);
    }
}



void __init_obj_alloc(obj_alloc_t*alloc, unsigned int size){
    alloc->curr_page = NULL;
    INIT_LIST_HEAD(&alloc->partial);
    INIT_LIST_HEAD(&alloc->full);
    INIT_LIST_HEAD(&alloc->empty);
    alloc->objsize = size;
}


void __init_obj_page(page_t* page, unsigned size){
    page->nr_unused = page->nr_obj = PAGE_SIZE / size;
    unsigned long chunk_header = page->phy_addr + size*(page->nr_obj-1);
    page->obj_freelist = (void **)chunk_header;
    while(chunk_header > page->phy_addr){
        *(void **)chunk_header = (void *)(chunk_header - size);
        chunk_header -= size;
    }
    *(void **)chunk_header = NULL;
}


void *obj_allocate(int token){

    if(token < 0 || token >= MAX_ALLOCATOR_NUM){
        uart_puts("[obj allocator] invalid token\n");
        return NULL;
    }
	
    struct obj_alloc* alloc = &allocator_pool[token];
	uart_puts("[obj_allocate] token: ");
	uart_put_int(token);
	uart_puts("\tsize: ");
	uart_put_int(alloc->objsize);
	uart_puts("\n");
    //uart_puts("\tbefore allocation\n");
    
    dump_alloc(alloc);
    if(alloc->curr_page == NULL){
        struct page *page;
        if(!list_empty(&alloc->partial)){// find chunk from previous page
            page = (struct page*)alloc->partial.next;
            list_crop(&page->list, &page->list);
        }else if(!list_empty(&alloc->empty)){// using empty object page as second choice
            page = (struct page*)alloc->empty.next;
            list_crop(&page->list, &page->list);
        }else{// demamd new page
            page = buddy_alloc(0);
            __init_obj_page(page, alloc->objsize);
            page->obj_alloc = alloc;
        }
        alloc->curr_page = page;
    }
    //allocate object
    struct page *curr_page = alloc->curr_page;
    void *obj = (void *)curr_page->obj_freelist;
    curr_page->obj_freelist = *curr_page->obj_freelist;
    //check full
    if(-- curr_page->nr_unused == 0){
        list_add_tail((list_ptr_t  *)curr_page, &alloc->full);
        alloc->curr_page = NULL;
    }
    //uart_puts("\tafter allocation\n");
    dump_alloc(alloc);
    //uart_puts("[obj_allocate] **done**\n\n");
    return obj;
}

void obj_free(unsigned long obj){   
    int pfn = PHY_ADDR_TO_PFN(obj);
    struct page* page = &page_t_pool[pfn];
    struct obj_alloc* alloc = page->obj_alloc;
    //printf("[obj_free] 0x%X (pfn=%d)\n", obj, pfn);
    //printf("\tbefore free\n");
    //printf("\t\tfreelist --> (0x%X)\n", page->obj_freelist);
    //dump_alloc(alloc);
    //printf("\t\tnr_unused: %d\n", page->nr_unused);
    // add to freelist
    void **header = (void **)obj;
    *header = (void *)page->obj_freelist;
    page->obj_freelist = header;
    page->nr_unused++;
    // from full to partial
    if(page->nr_unused == 1){
        list_crop(&page->list, &page->list);
        list_add_tail(&page->list, &alloc->partial);
    }
    // from partial to empty
    if(page->nr_unused == page->nr_obj){
        list_crop(&page->list, &page->list);
        //keep one empty page to prevent frequentlly page demand
        if(list_empty(&alloc->empty)){
            list_add_tail(&page->list, &alloc->empty);
        }else{
            page->obj_alloc = NULL;
            buddy_free(page);
        }
    }
    //printf("\tafter free\n");
    //printf("\t\tfreelist --> (0x%X) --> (0x%X)\n", page->obj_freelist, *page->obj_freelist);
    //dump_alloc(alloc);
    //printf("\t\tnr_unused: %d\n", page->nr_unused);
    //uart_puts("[obj_free] **done**\n\n");
}

page_t* buddy_alloc(int order){
    if( (order>MAX_ORDER) | (order<0) ){
        uart_puts("[__buddy_block_alloc] invalid order!\n");
        return 0;
    }
    //printf("[buddy allocate] order: %d\tsize: %d\n", order, 1<<order);
    
    //uart_puts("\tbefore\n");
    //dump_buddy();
    for(int ord_ext=order; ord_ext<=MAX_ORDER; ord_ext++){
        if(list_empty(&page_buddy[ord_ext])){
            continue;
        }
        // block found, remove block from free list
        page_t* to_alloc = (page_t*)page_buddy[ord_ext].next;
        list_crop(&to_alloc->list, &to_alloc->list);
        to_alloc->order = order;
        to_alloc->used = 1;

        // Release redundant memory block
        while(--ord_ext >= order){
            long buddy_pfn = FIND_BUDDY_PFN(to_alloc->pfn, ord_ext);
            page_t* buddy = &page_t_pool[buddy_pfn];
            buddy->order = ord_ext;
            list_add_tail(&buddy->list, &page_buddy[ord_ext]);
            //printf("\tfree redundant(PFN:%d, Order:%d)\n", buddy->pfn, buddy->order);
        }
        //printf("\tallocated block(PFN:%d, Order:%d)\n", to_alloc->pfn, to_alloc->order);
        //uart_puts("\tafter\n");
        //dump_buddy();
        //uart_puts("[buddy allocate] **done**\n\n");

        return to_alloc;
    }
    //uart_puts("[__buddy_block_alloc] No free space!\n");
    return 0;
}

void buddy_free(page_t* block){
    long buddy_pfn, lbuddy_pfn, rbuddy_pfn;
    struct page *buddy, *lbuddy, *rbuddy;
    
    //printf("[buddy free] **start**\n\tblock(PFN:%d, Order:%d)\n", block->pfn, block->order);
    //uart_puts("before\n");
    //dump_buddy();

    block->used = 0;
    // coalesce free buddy
    buddy_pfn = FIND_BUDDY_PFN(block->pfn, block->order);
    buddy = &page_t_pool[buddy_pfn];
    while(buddy->order == block->order && !buddy->used){
        list_crop(&buddy->list, &buddy->list);
        //merge
        //printf("\tcoalesce free buddy(PFN:%d, Order:%d)\n", buddy->pfn, buddy->order);
        rbuddy_pfn = FIND_RBUDDY_PFN(block->pfn, block->order);
        rbuddy = &page_t_pool[rbuddy_pfn];
        lbuddy_pfn = FIND_LBUDDY_PFN(block->pfn, block->order);
        lbuddy = &page_t_pool[lbuddy_pfn];
        rbuddy->order = -1;
        lbuddy->order += 1;
        //next iteration
        block = lbuddy;
        buddy_pfn = FIND_BUDDY_PFN(block->pfn, block->order);
        buddy = &page_t_pool[buddy_pfn];
    }
    list_add_tail(&block->list, &page_buddy[block->order]);
    //uart_puts("after\n");
    //dump_buddy();
    //uart_puts("[buddy free] **done**\n\n");

}

void *kmalloc(unsigned long size){
    uart_puts("[kmalloc] size: ");
	uart_put_int(size);
	uart_puts("\n");
    void *block;
    for(unsigned long i=MIN_KMALLOC_ORDER; i <= MAX_KMALLOC_ORDER; i++){
        if(size <= (1<<i)){
			uart_puts("Use slab system\n");
            block = obj_allocate(i-MIN_KMALLOC_ORDER);
			uart_puts("[kmalloc] **done**\n\n");
			//uart_put_hex((unsigned long)block);
			//uart_puts("\n\n");	
            return block;
        }
    }
    for(unsigned long i=0; i<MAX_ORDER; i++){
        if(size <= 1<<(i+PAGE_SHIFT)){
			uart_puts("Use buddy system\n");
            block = (void *)(buddy_alloc(i)->phy_addr);
			uart_puts("[kmalloc] **done**\n\n");
			//uart_put_hex((unsigned long)block);
			//uart_puts("\n\n");	
            return block;
        }
    }
    uart_puts("[kmalloc] too large!\n");
    return NULL;
}



void kfree(unsigned long block){
    uart_puts("[kfree] block: ");
	uart_put_hex(block);
	uart_puts("\n");
    int pfn = PHY_ADDR_TO_PFN(block);
    page_t* page = &page_t_pool[pfn];
    if(page->obj_alloc){
		uart_puts("Use obj_free\n");
		obj_free(block);
	}
    else{
		uart_puts("Use buddy_free\n");
		buddy_free(page);
	}
	uart_puts("[kfree] **done**\n\n");
}




void print_buddy_info(){
	int count = 0;
	uart_puts("*********************************************\n");
	uart_puts("order\t(npage)\t\tnr_free\n");
    for(int i=0; i<=MAX_ORDER; i++){
		count = 0;
        list_ptr_t *next;
        for(next = page_buddy[i].next; next!=&page_buddy[i]; next = next->next){
			count++;
        }
		uart_put_int(i);
		uart_puts("\t(");
		uart_put_int(1<<i);
		uart_puts(")\t\t");
		uart_put_int(count);
		uart_puts("\n");
    }
}
void print_alloc_info(){
	for (unsigned long i = MIN_KMALLOC_ORDER; i<=MAX_KMALLOC_ORDER; i++){
		struct obj_alloc* alloc = &allocator_pool[i];
		dump_alloc(alloc);
	}
}

void dump_alloc(obj_alloc_t* alloc){
    list_ptr_t *list;
    uart_puts("obj_alloc\n");
    uart_puts("alloc.objsize=");
    uart_put_int(alloc->objsize);
	uart_puts("\n");
    //printf("\t\talloc.curr_page{pfn=%d}\n", (alloc->curr_page == 0) ? -1 : alloc->curr_page->pfn);
    //printf("\t\t - nr_unused: %d\n", (alloc->curr_page == 0) ? -1 : alloc->curr_page->nr_unused);
    list =  alloc->partial.next;
    uart_puts("\t\talloc.partial");
    while(list != &alloc->partial){
        uart_puts("--> {pfn=");
    	uart_put_int(((page_t*)list)->pfn);
		uart_puts("}\n");
        list = list->next;
    }
    list =  alloc->full.next;
    uart_puts("\n\t\talloc.full");
    while(list != &alloc->full){
        uart_puts("--> {pfn=");
    	uart_put_int(((page_t*)list)->pfn);
		uart_puts("}\n");
        list = list->next;
    }
    list =  alloc->empty.next;
    uart_puts("\n\t\talloc.empty");
    while(list != &alloc->empty){
        uart_puts("--> {pfn=");
    	uart_put_int(((page_t*)list)->pfn);
		uart_puts("}\n");
        list = list->next;
    }
    uart_puts("\n\n");
}

