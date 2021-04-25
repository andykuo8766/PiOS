#include "mm.h"
//#define debug_mm

static unsigned short mem_map [ PAGING_PAGES ] = {0,};
static unsigned short kernel_stack_map[64] = {0,};


unsigned long get_user_page(int pid){
	return LOW_USER_STACK + (pid - 1) * PAGE_SIZE;
}

unsigned long get_kernel_id_page(){
	unsigned long i;
	for (i = 0 ; i < 64; i++) {
		if (kernel_stack_map[i] == 0) {
			kernel_stack_map[i] = 1;
			return i;
		}
	}
	return 0;
}
void free_page(unsigned long p){
	mem_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
}



void init_page_struct(){
    for(int i=0; i<NR_PAGE; i++){
        bookkeep[i].pfn = i;
        bookkeep[i].used = 0;
        bookkeep[i].phy_addr = LOW_MEMORY + i*PAGE_SIZE;
        bookkeep[i].order = -1;
        INIT_LIST_HEAD(&(bookkeep[i].list));// feature envy?
    }
}

void init_buddy(){
    for(int i=0; i<MAX_ORDER+1; i++){
        INIT_LIST_HEAD(&buddy_freelist[i]);
    }
    for(int i=0; i<NR_PAGE; i+=MAX_ORDER_SIZE){
        list_add_tail(&bookkeep[i].list, &buddy_freelist[MAX_ORDER]);
    }
}


#ifdef debug_mm
void dump_buddy(){
    uart_puts("\tbuddy_freelist={\n");
    for(int i=0; i<=MAX_ORDER; i++){
		uart_puts("\t\tORDER_");
		uart_put_int(i);
        list_head_t *next;
        for(next = buddy_freelist[i].next; next!=&buddy_freelist[i]; next = next->next){
			uart_puts(" --> {pfn(");
			uart_put_int(((struct page *)next)->pfn);
			uart_puts(")}");
        }
        uart_puts("\n");
    }
    uart_puts("\t}\n");
}
#endif//debug_mm

struct page *__buddy_block_alloc(int order){
    if( (order>MAX_ORDER) | (order<0) ){
        uart_puts("[__buddy_block_alloc] invalid order!\n");
        return 0;
    }
	
    #ifdef debug_mm
	uart_puts("[buddy allocate] order: ");
	uart_put_int(order);
	uart_puts("\tsize: ");
	uart_put_int(1<<order);
	uart_puts("\n");
    uart_puts("\tbefore\n");
    dump_buddy();
    #endif//debug_mm
	
    for(int ord_ext=order; ord_ext<=MAX_ORDER; ord_ext++){
        if(list_empty(&buddy_freelist[ord_ext])){
            continue;
        }
        
        // block found, remove block from free list
        struct page *to_alloc = (struct page *)buddy_freelist[ord_ext].next;
        list_crop(&to_alloc->list, &to_alloc->list);
        to_alloc->order = order;
        to_alloc->used = 1;

        // Release redundant memory block
        while(--ord_ext >= order){
            long buddy_pfn = FIND_BUDDY_PFN(to_alloc->pfn, ord_ext);
            struct page* buddy = &bookkeep[buddy_pfn];
            buddy->order = ord_ext;
            list_add_tail(&buddy->list, &buddy_freelist[ord_ext]);
        #ifndef debug_mm
        }
        #else
			uart_puts("\tfree redundant(PFN:");
			uart_put_int(buddy->pfn);
			uart_puts(", Order:");
			uart_put_int(buddy->order);
			uart_puts(")\n");
        }
		uart_puts("\tallocated block(PFN:");
		uart_put_int(to_alloc->pfn);
		uart_puts(", Order:");
		uart_put_int(to_alloc->order);
		uart_puts(")\n");
		
        uart_puts("\tafter\n");
        dump_buddy();
        uart_puts("[buddy allocate] **done**\n\n");
        #endif//debug_mm
        return to_alloc;
    }
    uart_puts("[__buddy_block_alloc] No free space!\n");
    return 0;
}

void buddy_block_free(struct page* block){
    long buddy_pfn, lbuddy_pfn, rbuddy_pfn;
    struct page *buddy, *lbuddy, *rbuddy;
    
    #ifdef debug_mm
	uart_puts("[buddy free] **start**\n\tblock(PFN:");
	uart_put_int(block->pfn);
	uart_puts(", Order:");
	uart_put_int(block->order);
	uart_puts(")\n");
    uart_puts("before\n");
    dump_buddy();
    #endif//debug_mm
    block->used = 0;
    // coalesce free buddy
    buddy_pfn = FIND_BUDDY_PFN(block->pfn, block->order);
    buddy = &bookkeep[buddy_pfn];
    while(buddy->order == block->order && !buddy->used){
        list_crop(&buddy->list, &buddy->list);
        //merge
		uart_puts("\tcoalesce free buddy(PFN:");
		uart_put_int(buddy->pfn);
		uart_puts(", Order:");
		uart_put_int(buddy->order);
		uart_puts(")\n");
        rbuddy_pfn = FIND_RBUDDY_PFN(block->pfn, block->order);
        rbuddy = &bookkeep[rbuddy_pfn];
        lbuddy_pfn = FIND_LBUDDY_PFN(block->pfn, block->order);
        lbuddy = &bookkeep[lbuddy_pfn];
        rbuddy->order = -1;
        lbuddy->order += 1;
        //next iteration
        block = lbuddy;
        buddy_pfn = FIND_BUDDY_PFN(block->pfn, block->order);
        buddy = &bookkeep[buddy_pfn];
    }
    list_add_tail(&block->list, &buddy_freelist[block->order]);
    #ifdef debug_mm
    uart_puts("after\n");
    dump_buddy();
    uart_puts("[buddy free] **done**\n\n");
    #endif//debug_mm
}

void init_obj_alloc(struct obj_alloc *alloc, unsigned int size){
    alloc->curr_page = NULL;
    INIT_LIST_HEAD(&alloc->partial);
    INIT_LIST_HEAD(&alloc->full);
    INIT_LIST_HEAD(&alloc->empty);
    alloc->objsize = size;
}

int register_obj_allocator(unsigned int objsize){
    if(objsize<MIN_ALLOCATOR_SIZE){
        objsize = MIN_ALLOCATOR_SIZE;
        uart_puts("[register_obj_allocator] reset objsize to MIN_ALLOCATOR_SIZE\n");
    }
    for(int token=0; token<MAX_ALLOCATOR_NUM; token++){
        if(allocator_pool[token].objsize)
            continue;
        init_obj_alloc(&allocator_pool[token], objsize);
        #ifdef debug_mm     
		uart_puts("[register_obj_allocator] successed!\n\tobjsize: ");
		uart_put_int(objsize);
		uart_puts("\ttoken: ");
		uart_put_int(token);
		uart_puts("\n\n");
        #endif//debug_mm
        return token;
    }
    uart_puts("[register_obj_allocator] Allocator pool has fulled\n");
    return -1;
}



void init_obj_page(struct page* page, unsigned size){
    page->nr_unused = page->nr_obj = PAGE_SIZE / size;

    unsigned long chunk_header = page->phy_addr + size*(page->nr_obj-1);
    page->obj_freelist = (void **)chunk_header;
    while(chunk_header > page->phy_addr){
        *(void **)chunk_header = (void *)(chunk_header - size);
        chunk_header -= size;
    }
    *(void **)chunk_header = NULL;
    #ifdef debug_mm
	uart_puts("[__init_obj_page] chunk_size: ");
	uart_put_int(size);
	uart_puts("\tnr_chunk: ");
	uart_put_int(page->nr_obj);
	uart_puts("\n");
    unsigned long base = page->phy_addr;
    for(int i=0; i<3;i++){
		uart_puts("\tchunk_");
		uart_put_int(i);
		uart_puts(" @ ");
		uart_put_hex(base+i*size);
		uart_puts(" --> (void *)");
		uart_put_hex( *(unsigned long *)(base+i*size));
		uart_puts("\n");
    }
    uart_puts("\t\t---skip---\n");
    for(int i=page->nr_obj-3; i<page->nr_obj;i++){
		uart_puts("\tchunk_");
		uart_put_int(i);
		uart_puts(" @ ");
		uart_put_hex(base+i*size);
		uart_puts(" --> (void *)");
		uart_put_hex( *(unsigned long *)(base+i*size));
		uart_puts("\n");
    }
	uart_puts("\tpage->obj_freelist  --> (void *)");
	uart_put_hex((unsigned long)page->obj_freelist);
	uart_puts("\n");
    uart_puts("[__init_obj_page] end\n\n");
    #endif//debug_mm    
}

#ifdef debug_mm
void dump_alloc(struct obj_alloc* alloc){
    list_head_t *list;
    uart_puts("\t\tobj_alloc = {\n");
	uart_puts("\t\talloc.objsize=");
	uart_put_int(alloc->objsize);
	uart_puts("\n");
	uart_puts("\t\talloc.curr_page{pfn=");
	uart_put_int((alloc->curr_page == 0) ? -1 : alloc->curr_page->pfn);
	uart_puts("}\n");
	uart_puts("\t\t - nr_unused: ");
	uart_put_int((alloc->curr_page == 0) ? -1 : alloc->curr_page->nr_unused);
	uart_puts("}\n");
    list =  alloc->partial.next;
    uart_puts("\t\talloc.partial");
    while(list != &alloc->partial){
		uart_puts("--> {pfn=");
		uart_put_int(((struct page*)list)->pfn);
		uart_puts("}");
        list = list->next;
    }
    list =  alloc->full.next;
    uart_puts("\n\t\talloc.full");
    while(list != &alloc->full){
		uart_puts("--> {pfn=");
		uart_put_int(((struct page*)list)->pfn);
		uart_puts("}");
        list = list->next;
    }
    list =  alloc->empty.next;
    uart_puts("\n\t\talloc.empty");
    while(list != &alloc->empty){
		uart_puts("--> {pfn=");
		uart_put_int(((struct page*)list)->pfn);
		uart_puts("}");
        list = list->next;
    }
    uart_puts("\n}\n");
}
#endif//debug_mm

void *obj_allocate(int token){
    if(token < 0 || token >= MAX_ALLOCATOR_NUM){
        uart_puts("[obj allocator] invalid token\n");
        return NULL;
    }
    struct obj_alloc* alloc = &allocator_pool[token];
    #ifdef debug_mm
	uart_puts("[obj_allocate] token: ");
	uart_put_int(token);
	uart_puts("\tsize: ");
	uart_put_int(alloc->objsize);
	uart_puts("\n");
    uart_puts("\tbefore allocation\n");
    dump_alloc(alloc);
    #endif//debug_mm
    
    if(alloc->curr_page == NULL){
        struct page *page;
        if(!list_empty(&alloc->partial)){// find chunk from previous page
            page = (struct page*)alloc->partial.next;
            list_crop(&page->list, &page->list);
        }else if(!list_empty(&alloc->empty)){// using empty object page as second choice
            page = (struct page*)alloc->empty.next;
            list_crop(&page->list, &page->list);
        }else{// demamd new page
            page = __buddy_block_alloc(0);
            init_obj_page(page, alloc->objsize);
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
        list_add_tail((list_head_t *)curr_page, &alloc->full);
        alloc->curr_page = NULL;
    }
    #ifdef debug_mm
    uart_puts("\tafter allocation\n");
    dump_alloc(alloc);
    uart_puts("[obj_allocate] **done**\n\n");
    #endif//debug_mm
    return obj;
}

void obj_free(void *obj){   
    int pfn = PHY_ADDR_TO_PFN(obj);
    struct page* page = &bookkeep[pfn];
    struct obj_alloc* alloc = page->obj_alloc;
    #ifdef debug_mm
	uart_puts("[obj_free] ");
	uart_put_hex((unsigned long)obj);
	uart_puts(" (pfn=");
	uart_put_int(pfn);
	uart_puts(")\n");
    uart_puts("\tbefore free\n");
	uart_puts("\t\tfreelist --> (");
	uart_put_hex((unsigned long)page->obj_freelist);
	uart_puts(")\n");
    dump_alloc(alloc);
	uart_puts("\t\tnr_unused: ");
	uart_put_int(page->nr_unused);
	uart_puts("\n");	
    #endif//debug_mm
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
            buddy_block_free(page);
        }
    }
    #ifdef debug_mm
    uart_puts("\tafter free\n");
	uart_puts("\t\tfreelist --> (");
	uart_put_hex((unsigned long)page->obj_freelist);
	uart_puts(") --> (");
	uart_put_hex((unsigned long)*page->obj_freelist);
	uart_puts(")\n");
    dump_alloc(alloc);
	uart_puts("\t\tnr_unused: ");
	uart_put_int(page->nr_unused);
	uart_puts("\n");
    uart_puts("[obj_free] **done**\n\n");
    #endif//debug_mm
}


void init_kmalloc(){
    for (unsigned int i=MIN_KMALLOC_ORDER; i<=MAX_KMALLOC_ORDER; i++){
        register_obj_allocator(1<<i);
    }
}

void *kmalloc(unsigned int size){
    #ifdef debug_mm
	uart_puts("[kmalloc] size: ");
	uart_put_int(size);
	uart_puts("\n");
    #endif//debug_mm
    void *block;
    for(unsigned int i=MIN_KMALLOC_ORDER; i<MAX_KMALLOC_ORDER; i++){
        if(size <= (1<<i)){
            block = obj_allocate(i-MIN_KMALLOC_ORDER);
            #ifdef debug_mm
			uart_puts("[kmalloc] **done** block@ ");
			uart_put_hex((unsigned long)block);
			uart_puts("\n\n");
            #endif//debug_mm
            return block;
        }
    }
    for(unsigned int i=0; i<MAX_ORDER; i++){
        if(size <= 1<<(i+PAGE_SHIFT)){
            block = (void *)(__buddy_block_alloc(i)->phy_addr);
            #ifdef debug_mm
			uart_puts("[kmalloc] **done** block@ ");
			uart_put_hex((unsigned long)block);
			uart_puts("\n\n");
            #endif//debug_mm
            return block;
        }
    }
    uart_puts("[kmalloc] too large!\n");
    return NULL;
}

void kfree(void *block){
    #ifdef debug_mm
	uart_puts("[kfree] block@ ");
	uart_put_hex((unsigned long)block);
	uart_puts("\n");
    #endif//debug_mm
    int pfn = PHY_ADDR_TO_PFN(block);
    struct page* page = &bookkeep[pfn];
    if(page->obj_alloc)
        obj_free(block);
    else
        buddy_block_free(page);
    #ifdef debug_mm
    uart_puts("[kfree] **done**\n\n");
    #endif//debug_mm
}

void mm_init(){
    init_page_struct();// must do before buddy or it will break the list
    init_buddy();
    init_kmalloc();
    // __init_obj_allocator; // init bss should set them to size 0(inused)
}


