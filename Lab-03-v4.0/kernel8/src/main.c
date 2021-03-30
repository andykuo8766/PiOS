#include "uart.h"
#include "shell.h"
#include "buddy.h"

void kernel_main(){
    // set up serial console
    uart_init();
	
	buddy_init((char *)BUDDY_START);
    // start shell
    shell();
}
