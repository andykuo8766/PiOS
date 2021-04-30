#include "sys_call.h"

int getpid(){
    asm volatile("mov x22, #1");
    asm volatile("svc #0");
}

unsigned int uart_read(char buf[], unsigned int size){
    asm volatile("mov x22, #2");
    asm volatile("svc #0");
}

unsigned int uart_write(const char buf[], unsigned int size){
    asm volatile("mov x22, #3");
    asm volatile("svc #0");
}

int exec(const char* name, char *const argv[]){
    asm volatile("mov x22, #4");
    asm volatile("svc #0");
}

void exit(){
    asm volatile("mov x22, #5");
    asm volatile("svc #0");
}

int fork(){
    asm volatile("mov x22, #6");
    asm volatile("svc #0");
}
