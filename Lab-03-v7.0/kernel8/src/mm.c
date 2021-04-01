#include "mm.h"
//#define debug

unsigned long init = 0;


unsigned long align_up(unsigned long  size, int alignment) {
  return (size + alignment - 1) & -alignment;
}

int addr2idx(void *addr) {
    return (((unsigned long)addr & -PAGE_SIZE) - FRAME_BASE) / PAGE_SIZE;
}

unsigned align_up_exp(unsigned n) {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}

void *split_frames(int order, int target_order) {
    /* take the ready to split frame out */
    struct list_head *ptr = remove_head(&frame_bins[order]);
	
	#ifdef debug
	uart_puts("split frame: ");
	uart_put_hex((unsigned long)ptr);
	uart_puts("\r\n");
	#endif
	
    /* puts splitted frame into bin list */
    for (int i = order; i > target_order; i--) {
        struct list_head *s = (struct list_head *)((char *)ptr + (PAGE_SIZE * (1 << (i-1))));
        insert_head(&frame_bins[i-1], s);
        frame[((unsigned long)s - FRAME_BASE) / PAGE_SIZE].order = i - 1;
		#ifdef debug
		uart_puts("insert frame: ");
		uart_put_hex((unsigned long)ptr);
		uart_puts("\r\n");
		#endif
    }
    int idx = addr2idx(ptr);
    frame[idx].order = target_order;
    frame[idx].flags = FRAME_INUSE;
    return ptr;
}


/* Assuming total frame size can be divide by biggest bin size */
void init_buddy() {
    struct list_head *victim;
    for (int i = 0; i < FRAME_BINS; i++) {
        list_init(&frame_bins[i]);
    }
	
    for (unsigned long i = 0; i < FRAME_ARRAY_SIZE; i += 1 << MAX_ORDER) {
        frame[i].order = MAX_ORDER;

        victim = (struct list_head *)(FRAME_BASE + i * PAGE_SIZE);
        insert_tail(&frame_bins[MAX_ORDER], victim);
    }
}

int pages_to_frame_order(unsigned count) {
    count = align_up_exp(count);
    return __builtin_ctz(count);
}
int size_to_cache_order(unsigned size) {
    size = align_up_exp(size);
    size /= CACHE_MIN_SIZE;
    return __builtin_ctz(size);
}

void *alloc_pages(unsigned count) {
    if (!init) {
        init = 1;
        init_buddy();
    }

    int target_order = pages_to_frame_order(count);

    for (int i = target_order; i < FRAME_BINS; i++) {
        if (frame_bins[i].next != &frame_bins[i]) {
            return split_frames(i, target_order);
        }
    }

    /* TODO: OOM handling */
    return NULL;
}

void free_pages(void *victim) {
    int page_idx = ((unsigned long)victim - FRAME_BASE) / PAGE_SIZE;
    if (!IS_INUSE(frame[page_idx])) {
        uart_puts("[Warn] Kernel: double free");
        return;
    }
    
    unsigned order = frame[page_idx].order;
    int buddy_page_idx = page_idx ^ (1 << order);
    frame[page_idx].flags &= ~FRAME_INUSE;

    /* merge frames */
    while (order <= MAX_ORDER &&
           !IS_INUSE(frame[buddy_page_idx]) &&
           order == frame[buddy_page_idx].order)
    {
        void *buddy_victim = (void *)(FRAME_BASE + buddy_page_idx * PAGE_SIZE);
        del_link((struct list_head *)buddy_victim);
		#ifdef debug
		uart_puts("merge buddy frame: ");
		uart_put_hex((unsigned long)buddy_victim);
		uart_puts("\r\n");
		#endif
        order += 1;
        victim = page_idx < buddy_page_idx ? victim : buddy_victim;
        page_idx = page_idx < buddy_page_idx ? page_idx : buddy_page_idx;
        buddy_page_idx = page_idx ^ (1 << order);
    }

    insert_head(&frame_bins[order], victim);
    frame[page_idx].order = order;
	#ifdef debug
	uart_puts("attach frame: ");
	uart_put_hex((unsigned long)victim);
	uart_puts("\r\n");
	#endif
}

void *get_cache(unsigned int size) {
    int order = size_to_cache_order(size);

    void *ptr = cache_bins[order];
    if (ptr) {
        cache_bins[order] = cache_bins[order]->next;
        int idx = addr2idx(ptr);
        frame[idx].refcnt += 1;
		#ifdef debug
		uart_puts("detach cache: ");
		uart_put_hex((unsigned long)ptr);
		uart_puts("\r\n");
		#endif
    }

    return ptr;
}

/* size should aligned to next exp size */
void alloc_cache(void *mem, int size) {
    int count = PAGE_SIZE / size;
    int idx = addr2idx(mem);
    int order = size_to_cache_order(size);
    frame[idx].flags |= FRAME_MEM_CACHE;
    frame[idx].refcnt = 0;
    frame[idx].cache_order = order;

    for (int i = 0; i < count; i++) {
        struct cache *ptr = (struct cache *)((unsigned long)mem + i * size);
        ptr->next = cache_bins[order];
        cache_bins[order] = ptr;
		#ifdef debug
		uart_puts("insert cache: ");
		uart_put_hex((unsigned long)ptr);
		uart_puts("\r\n");
		#endif
    }
}

/* request <  PAGE_SIZE: page memory pool
 * request >= PAGE_SIZE: only use alloc_page
 */
void *kmalloc(unsigned int size) {
    if (align_up(size, PAGE_SIZE) > FRAME_SIZE_MAX) {
        return NULL;
    }

    if (size < CACHE_MIN_SIZE) {
        size = CACHE_MIN_SIZE;
    }

    void *cache;
    if (align_up_exp(size) < PAGE_SIZE) {   
        size = align_up_exp(size);
        cache = get_cache(size);

        if (!cache) {
            void *mem = alloc_pages(1);
            alloc_cache(mem, size);
            cache = get_cache(size);
        }
    } else {
        unsigned pages = align_up(size, PAGE_SIZE) / PAGE_SIZE;
        cache = alloc_pages(pages);
    }

    return cache;
}

void kfree(void *ptr) {
    int idx = addr2idx(ptr);
    if (idx >= FRAME_ARRAY_SIZE) {
        uart_puts("[Warn] Kernel: kfree wrong address");
        return;
    }
    if (IS_MEM_CACHE(frame[idx])) {
        int order = frame[idx].cache_order;
        ((struct cache *)ptr)->next = cache_bins[order];
        cache_bins[order] = ptr;
        frame[idx].refcnt -= 1;
		#ifdef debug
		uart_puts("attach cache: ");
		uart_put_hex((unsigned long)ptr);
		uart_puts("\r\n");
		#endif
        /* find when to release unreferenced cache */
    } else {
        free_pages(ptr);
    }
}
void print_buddy_info(){
	int count = 0;
	uart_puts("*********************************************\n");
	uart_puts("order\t(npage)\t\tnr_free\n");
    for(int i=0; i<=MAX_ORDER; i++){
		count = 0;
        struct list_head *next;
        for(next = frame_bins[i].next; next!=&frame_bins[i]; next = next->next){
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

