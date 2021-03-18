#include "uart.h"
#include "uart_boot.h"
#include "string.h"

#define MAX_BUFFER_LEN 128
#define  CHAIN_LOADING_ADDRESS  0x100000

void kernel_main(){
    // set up serial console
    uart_init();
    // uart_puts
    uart_puts("bootloader is already\n");
	
    char buffer[MAX_BUFFER_LEN];
    readline(buffer, MAX_BUFFER_LEN);

    if (strcmp(buffer, "kernel") == 0) {
        copy_current_kernel_and_jump(CHAIN_LOADING_ADDRESS);
    }


}
