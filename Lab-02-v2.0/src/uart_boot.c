#include "uart.h"
#include "utils.h"
#include "string.h"
#include "shell.h"

// See https://sourceware.org/binutils/docs/ld/Source-Code-Reference.html
extern char __bss_end[];
extern char __start[];

void copy_and_jump_to_kernel() {
	char *kernel = (char *)0x80000;
	uart_puts("Wait for get kernel size\r\n");
    int kernel_size = uart_read_int();
	uart_puts("Read kernel size\r\n");
    // Confirm kernel size
    uart_puts("Wait for send kernel size\r\n");
    uart_send_int(kernel_size);
    uart_puts("Send kernel size\r\n");
	
    int checksum = 0;

    for (int i = 0; i < kernel_size; i++) {
        char c = uart_recv();
        checksum += c;
        kernel[i] = c;
    }

    //uart_send_int(checksum);

    uart_puts("Done copying kernel\r\n");
    branch_to_address((void *)0x800000);
	//void (*jump_new_kernel)(void) = new_address;
    //jump_new_kernel();
	
}

/**
 * This is a weird function.  It copies everything from 0x00
 * up to bss_end to the new_address. Then, it gets the address of
 * copy_and_jump_to_kernel and adds the offset of the new address. We do this
 * because we want to call the function in the new address (the newly copied
 * kernel).
 */
void copy_current_kernel_and_jump(char *new_address) {
    char *kernel = (char *)0x800000;
    char *end = __bss_end;
    char *copy = new_address;
	
	uart_puts("start copy self kernel\n");
    while (kernel <= end) {
        *copy = *kernel;
        kernel++;
	    copy++;
    }
    uart_puts("copy self kernel finish\n");
    // Cast the function pointer to char* to deal with bytes.
    char *original_function_address = (char *)&copy_and_jump_to_kernel;

    // Add the new address (we're assuming that the original kernel resides in
    // address 0). copied_function_address should now contain the address of the
    // original function but in the new location.
    char *copied_function_address =original_function_address + (long)new_address;

    // Cast the address back to a function and call it.
    void (*call_function)() = (void (*)())copied_function_address;


    //void (*func_ptr)() = copy_and_jump_to_kernel;
    //unsigned long int original_function_address = (unsigned long int)func_ptr;
    //void (*call_function)(char *) = (void (*)(char *))(original_function_address - (unsigned long int)__start + CHAIN_LOADING_ADDRESS);

	
    call_function();
}


void loadimg(){
    char buff[MAX_BUFFER_LEN];
	unsigned int n, size;
	char *load_addr;
	
	uart_puts("Input kernel size(bytes): ");
	if ((n = read_command(buff, MAX_BUFFER_LEN)) >= 0) {
		buff[n]='\0';
		size = atoi(buff);
	}
	uart_puts("Input kernel load address: ");
	if ((n = read_command(buff, MAX_BUFFER_LEN)) >= 0) {
		buff[n]='\0';
		load_addr = (char *)(long int)(hexatoi(buff));
	}

    uart_puts("Please send image...\n");

    char *base = load_addr;


    while(size--){
        *load_addr = uart_recv();
        load_addr++;
    }

    ((void(*)(void))base)();
}

