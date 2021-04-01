#include "uart.h"
#include "shell.h"
#include "mm.h"

extern unsigned char __start, __end;

void kernel_main(){
    // set up serial console
    uart_init();
	uart_puts("start init reserve\n");
	init_reserve();
	uart_puts("reserve init finish\n");
	uart_puts("reserve spin table\n");
	reserve_mem((void *)0x0, 0x1000);  // spin table
	uart_puts("spin table reserve finish\n");
	uart_puts("reserve kernel\n");
	reserve_mem((void *)(&__start), (&__end - &__start)); // kernel
	uart_puts("kernel reserve finish\n");
	uart_puts("reserve buddy system\n");
	reserve_mem((void *)(&__end), mem_size / PAGE_SIZE);     // buddy system
	uart_puts("buddy system reserve finish\n");
	uart_puts("start init buddy\n");
	init_buddy((char *)(&__end));
	uart_puts("buddy init finish\n");
	uart_puts("start init slab\n");
	init_slab();
	uart_puts("slab init finish\n");
	//check_slab();

    // start shell
    shell();
}
