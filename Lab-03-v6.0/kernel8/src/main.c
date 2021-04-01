#include "uart.h"
#include "shell.h"
#include "mm.h"

void kernel_main(){
    // set up serial console
    uart_init();

	mm_init();
	
    // start shell
    shell();
}
