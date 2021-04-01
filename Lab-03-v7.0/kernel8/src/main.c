#include "uart.h"
#include "shell.h"
#include "mm.h"

void kernel_main(){
    // set up serial console
    uart_init();

	init_buddy();
	print_buddy_info();
	kmalloc(5000);
	print_buddy_info();

    // start shell
    shell();
}
