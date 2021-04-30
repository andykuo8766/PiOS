#ifndef __SYS_H__
#define __SYS_H__

#include "sched.h"
#include "uart.h"
#include "cpio.h"

void sys_getpid(unsigned long *trap_frame);
void sys_uart_read(unsigned long *trap_frame);
void sys_uart_write(unsigned long *trap_frame);
void sys_exec(unsigned long *trap_frame);
void sys_exit(unsigned long *trap_frame);
void sys_fork(unsigned long *parent_trap_frame);

#endif