#include "uart.h"
#include "string.h"

void loadimg(){
    char buf[10];
    int i=0;





    uart_puts("Please input image size: ");
    buf[i] = uart_getc();
    while(buf[i] != '\n' && buf[i] != '\r'){
        uart_send(buf[i++]);
        buf[i] = uart_getc();
    }
    buf[i] = '\0';
    int size = atoi(buf);
	
    uart_puts("\n");

    i = 0;
    uart_puts("Please input image load address: ");
    buf[i] = uart_getc();
    while(buf[i] != '\n' && buf[i] != '\r'){
        uart_send(buf[i++]);
        buf[i] = uart_getc();
    }
    buf[i] = '\0';
    uart_puts("\n");
    char *load_addr = (char *)(unsigned long)hextoint(buf);

    uart_puts("Please send image...\n");

    char *base = load_addr;


    while(size--){
        *load_addr = uart_recv();
        load_addr++;
    }

    ((void(*)(void))base)();
}

