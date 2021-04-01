#include "uart.h"
#include "shell.h"
#include "buddy.h"

void kernel_main(){
    // set up serial console
    uart_init();

	init_page_sys();
    // start shell
    shell();
}
