#ifndef _SVC_H
#define _SVC_H

#define __NR_syscalls	            8

#define SYS_TIME_IRQ    0x1
#define SYS_CLONE       0x2
#define SYS_EXIT_NUMBER 0x3
#define SYS_UART_READ   0x4
#define SYS_UART_WRITE  0x5
#define SYS_EXEC        0x6
#define SYS_FORK        0x7
#define SYS_ID          0x8
#define SYS_KILL        0x9
#define SYS_UART_EN     0x10

#ifndef __ASSEMBLER__
long sys_get_taskid();
int sys_exec(void(*func)());
int sys_fork();
void sys_exit();
void sys_sched_yield();
unsigned int sys_uart_read(char buf[], unsigned long size);
unsigned int sys_uart_write(const char buf[], unsigned long size);
#endif//__ASSEMBLER__


#endif//_SVC_H