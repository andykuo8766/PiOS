#include "sys.h"

void sys_getpid(unsigned long *trap_frame){
    trap_frame[0] = current_thread()->pid;
}

void sys_uart_read(unsigned long *trap_frame){
    char *buffer = (char *)trap_frame[0];
    for(unsigned int i=0; i<trap_frame[1]; i++){
        buffer[i] = uart_get();
    }
    trap_frame[0] = trap_frame[1];
}

void sys_uart_write(unsigned long *trap_frame){
    char *buffer = (char *)trap_frame[0];
    for(unsigned int i=0; i<trap_frame[1]; i++){
        uart_send(buffer[i]);
    }
    trap_frame[0] = trap_frame[1];
}

void sys_exec(unsigned long *trap_frame){
    trap_frame[0] = exec((char *)trap_frame[0], (char **)trap_frame[1]);
}

void sys_exit(unsigned long *trap_frame){
    exit();
}

void sys_fork(unsigned long *parent_trap_frame){
    unsigned long pid = fork();
    if(pid != 0){
        parent_trap_frame[0] = pid;

        Thread *parent = current_thread();
        Thread *child = get_thread(pid);

        unsigned long *child_trap_frame = (parent_trap_frame - (unsigned long *)parent->kernel_stack) + (unsigned long *)child->kernel_stack;
        child_trap_frame[0] = 0;
        child_trap_frame[29] = (parent_trap_frame[29] - (unsigned long)parent->user_stack) + (unsigned long)child->user_stack;
        if(child->user_stack){
            child_trap_frame[30] = (parent_trap_frame[30] - (unsigned long)parent->user_stack) + (unsigned long)child->user_stack;
        }
        child_trap_frame[33] = (parent_trap_frame[33] - (unsigned long)parent->user_stack) + (unsigned long)child->user_stack;
    }
}

