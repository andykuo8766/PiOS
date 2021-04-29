#ifndef	_MM_H
#define	_MM_H



#include "list.h"
#include "utils.h"

#define BUDDY_MAX 				16    	// 4kB ~ 128MB
#define STARTUP_MAX 			16  	// max reserve slot
#define PAGE_SIZE_CTZ 			12
#define INIT_PAGE 				32

#define PAGE_SHIFT	 		    12
#define TABLE_SHIFT 			9
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)
#define PAGE_SIZE   		    (1 << PAGE_SHIFT)	
#define SECTION_SIZE			(1 << SECTION_SHIFT)	


#define LOW_MEMORY              SECTION_SIZE  // warning 0x200000
#define HIGH_MEMORY             MMIO_BASE

#define LOW_KERNEL_STACK        LOW_MEMORY
#define HIGH_KERNEL_STACK       LOW_MEMORY + 64 * PAGE_SIZE
#define LOW_USER_STACK          HIGH_KERNEL_STACK



extern unsigned long mem_size;


#define pad(x, y) ((((x) + (y)-1) / (y)) * (y))
#define get_order(ptr) (__builtin_ctzl((unsigned long)ptr) - PAGE_SIZE_CTZ)
#define set_buddy_ord(bd, ord) (bd = ord | (bd & 0xe0))
#define set_buddy_flag(bd, flag) (bd = (flag << 5) | (bd & 0x1f))
#define get_buddy_ord(bd) (bd & 0x1f)
#define get_buddy_flag(bd) ((bd & 0xe0) >> 5)
#define ptr_to_pagenum(ptr) (((unsigned long)(ptr)) >> PAGE_SIZE_CTZ)
#define pagenum_to_ptr(pn) ((void *)(((unsigned long)pn) << PAGE_SIZE_CTZ))
#define buddy_pagenum(pg, ord) ((pg) ^ (1 << ord))

unsigned long get_user_page(int pid);
unsigned long get_kernel_id_page();

void *kmalloc(unsigned long size);
void *alloc_page(unsigned int size);
void *alloc_slab(void *slab_tok);

void kfree(void *ptr);
void free_page(void *ptr);
void free_reserve(void *ptr);
void free_unknow_slab(void *ptr);

void init_memory_system();

void print_buddy_info();
void print_buddy_stat();
void print_slab();

void test_buddy();
void test_slab();




#endif  /*_BUDDY_H */
