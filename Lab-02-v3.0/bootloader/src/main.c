#include "uart.h"
#include "shell.h"




void kernel_main(){
    // set up serial console
    uart_init();
    
    // uart_puts
    uart_puts("Bootloader is already\n");

    // start shell
    shell_start();

}
