#include "buddy.h"


// for buddy system
int size2Index(int size) {
  int i = 0;
  while (size >>= 1) i++;
  return i;
}

buddy_list *__buddy_merge__(buddy_list *now, buddy_list *next,buddy_list **before) {
  int size = now->size;
  if (now > next) swap((void *)&now, (void *)&next);
  if (now + size != next ||
      ((unsigned long)now->addr - BUDDY_START) / PAGE_SIZE % (size << 1) != 0)
    return 0;
  *before = next->next;
  next->size = -1;
  next->next = 0;
  now->size <<= 1;
  now->next = 0;
  return now;
}

void buddy_free(buddy_list *list) {
  int size = list->size;
  buddy_list *now = used_list[size2Index(size)],
             **before = &used_list[size2Index(size)];
  for (; now; before = &now->next, now = now->next)
    if (now == list) break;
  *before = list->next;
  now = free_list[size2Index(size)];
  before = &free_list[size2Index(size)];
  buddy_list *merge;
  for (; now; before = &now->next, now = now->next) {
    if ((merge = __buddy_merge__(list, now, before))) {
      buddy_free(merge);
      return;
    }
  }

  list->next = free_list[size2Index(size)];
  free_list[size2Index(size)] = list;
}

void printDmaPool(dma *list) {
  for (dma *now = list; now != 0; now = now->next) {
    uart_puts("  addr: ");
    uart_put_hex((unsigned long int)((void *)now) + sizeof(dma));
    uart_puts("         size: ");
    uart_put_hex((unsigned long int)now->size);
	uart_puts("\r\n");
    // uart_puts("        allocate memory addr end: ");
    // print_h((unsigned long int)now->addr + now->size * 4 * KB);
  }
  uart_puts("\r\npool end\r\n");
}
void *__dma_merge__(dma *now, dma *next, dma **before) {
  if (now > next) swap((void *)&now, (void *)&next);
  int size = now->size, dmaSize = sizeof(dma) + align(sizeof(dma), 8);
  if ((void *)now + size + dmaSize != (void *)next || now->page != next->page)
    return 0;
  *before = (*before)->next;
  now->size += next->size + dmaSize;
  now->next = 0;
  return (void *)now + dmaSize;
}

void free(void *addr) {
  int dmaSize = sizeof(dma) + align(sizeof(dma), 8);
  dma *list = (dma *)(addr - dmaSize), *now = used_pool, **before = &used_pool;
  for (; now; before = &now->next, now = now->next)
    if (now == list) break;
  *before = list->next;
  now = free_pool;
  before = &free_pool;
  void *merge;
  for (; now; before = &now->next, now = now->next)
    if ((merge = __dma_merge__(list, now, before))) {
      free(merge);
      return;
    }
  if (list->size + dmaSize == list->page->size * PAGE_SIZE) {
    buddy_free(list->page);
    return;
  }
  list->next = free_pool;
  free_pool = list;
}

void print_buddyList(buddy_list **lists) {
  for (unsigned long i = 0; i < BUDDY_INDEX; i++) {
    uart_puts("index: ");
    uart_put_hex(i);
  	uart_puts("\n");
    for (buddy_list *now = lists[i]; now != 0; now = now->next) {
      uart_puts("    memory addr: ");
	  uart_put_hex((unsigned long)now->addr);
	  uart_puts("\n");
      // uart_puts("        allocate memory addr end: ");
      // print_h((unsigned long int)now->addr + now->size * 4 * KB);
    }
  }
}
buddy_list *buddy_alloc(int size) {
  size /= PAGE_SIZE;
  if (free_list[size2Index(size)] == 0) {
    buddy_list *now = buddy_alloc(2 * size * PAGE_SIZE);
    used_list[size2Index(2 * size)] = now->next;
    free_list[size2Index(size)] = now;
    now->size = size;
    now->next = now + now->size;
    now = now->next;
    now->size = size;
    now->next = 0;
  }
  buddy_list *now = free_list[size2Index(size)];
  free_list[size2Index(size)] = now->next;
  now->next = used_list[size2Index(size)];
  used_list[size2Index(size)] = now;
  return now;
}

void *malloc(int size) {
  size += align(size, 8);
  int dmaSize = sizeof(dma) + align(sizeof(dma), 8);
  /* get appropriate size*/
  int min = (1<<30);//GB
  dma *result = 0, **rBefore = &free_pool;
  for (dma *now = free_pool, **before = &free_pool; now;
       before = &now->next, now = now->next) {
    int delta = now->size - dmaSize - size;
    if (0 < delta && delta < min) {
      min = delta;
      result = now;
      rBefore = before;
    }
  }
  if (result == 0) {
    int buddy_size = 2 * dmaSize + size + align(2 * dmaSize + size, PAGE_SIZE);
    buddy_list *page = buddy_alloc(buddy_size);
    result = (dma *)page->addr;
    result->size = buddy_size - dmaSize;
    result->page = page;
    result->next = 0;
  };
  dma *list = (dma *)((void *)result + dmaSize + size);
  list->next = result->next;
  list->page = result->page;
  list->size = result->size - dmaSize - size;
  *rBefore = list;

  /* alloc */
  result->next = used_pool;
  result->size = size;
  used_pool = result;
  return (void *)result + dmaSize;
}


void buddy_init(char *mem_start) {
  for (int i = 0; i < BUDDY_INDEX; i++) {
  	free_list[i] = 0;
	used_list[i] = 0;
  }
  for (int i = 0; i < BUDDY_ARRAY_SIZE; i++){
    buddy[i] =(buddy_list){.next = 0, .addr = mem_start + i * PAGE_SIZE, .size = -1};
  }
  
  int begin_size = 2 << (BUDDY_INDEX - 1);
  for (int size = begin_size, i = 0; size > 0; size >>= 1)
    for (; i + size <= BUDDY_ARRAY_SIZE; i += size) {
      if (i + 2 * size <= BUDDY_ARRAY_SIZE) buddy[i].next = &buddy[i + size];
      buddy[i].size = size;
    }
  for (int size = begin_size, reminder_size = BUDDY_ARRAY_SIZE, i = 0; size > 0;
       size >>= 1)
    if (buddy[i].size == size) {
      free_list[size2Index(size)] = &buddy[i];
      i += (reminder_size / size) * size;
      reminder_size %= size;
    }
}
void dma_init() { 
	free_pool = used_pool = 0; 
}

void buddy_test() {
  uart_puts("\r\n+++++++++ buddy_test +++++++++\r\n");
  uart_puts("\r\n********* init *********\r\n");
  uart_puts("\r\n********* free_list *********\r\n");
  print_buddyList(free_list);
  uart_puts("\r\n");
  uart_puts("\r\n********* used_list *********\r\n");
  print_buddyList(used_list);
  uart_puts("\r\n********* alloced *********\r\n");
  struct buddy_list *list0 = buddy_alloc(PAGE_SIZE);
  uart_puts("\r\n********* free_list *********\r\n");
  print_buddyList(free_list);
  uart_puts("\r\n");
  uart_puts("\r\n********* used_list *********\r\n");
  print_buddyList(used_list);
  uart_puts("\r\n********* freed *********\r\n");
  buddy_free(list0);
  uart_puts("\r\n");
  uart_puts("\r\n********* free_list *********\r\n");
  print_buddyList(free_list);
  uart_puts("\r\n");
  uart_puts("\r\n********* used_list *********\r\n");
  print_buddyList(used_list);
  uart_puts("\r\nending\r\n");
  return;
}

void dma_test() {
  uart_puts("\r\n+++++++++ dma_test1 +++++++++\r\n");
  uart_puts("\r\n********* init *********\r\n");
  uart_puts("free:\r\n");
  printDmaPool(free_pool);
  uart_puts("used:\r\n");
  printDmaPool(used_pool);
  uart_puts("\r\n********* alloced *********\r\n");
  void *p0 = malloc(3);
  void *p1 = malloc(4);
  void *p2 = malloc(4);
  uart_puts("free:\r\n");
  printDmaPool(free_pool);
  uart_puts("used:\r\n");
  printDmaPool(used_pool);
  uart_puts("\r\n********* freed *********\r\n");
  free(p2);
  free(p1);
  uart_puts("free:\r\n");
  printDmaPool(free_pool);
  uart_puts("used:\r\n");
  printDmaPool(used_pool);
  uart_puts("\r\n********* freed *********\r\n");
  free(p0);
  uart_puts("free:\r\n");
  printDmaPool(free_pool);
  uart_puts("used:\r\n");
  printDmaPool(used_pool);
  uart_puts("\r\nending\r\n");
}

