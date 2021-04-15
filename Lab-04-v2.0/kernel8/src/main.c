#include "uart.h"
#include "shell.h"
#include "mm.h"
#include "irq.h"

void kernel_main(){
	timeout_event_init();

    // set up uart
    uart_init();
	// init memory system
    init_memory_system();
	enable_interrupt();
    // start shell
    shell();
}
