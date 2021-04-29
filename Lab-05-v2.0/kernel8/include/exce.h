#ifndef _EXEC_H
#define _EXEC_H

#define S_FRAME_SIZE			256 		// size of all saved registers 

#define SYNC_INVALID_EL1t		0 
#define IRQ_INVALID_EL1t		1 
#define FIQ_INVALID_EL1t		2 
#define ERROR_INVALID_EL1t		3 

#define SYNC_INVALID_EL1h		4 
#define IRQ_INVALID_EL1h		5 
#define FIQ_INVALID_EL1h		6 
#define ERROR_INVALID_EL1h		7 

#define SYNC_INVALID_EL0_64	    8 
#define IRQ_INVALID_EL0_64	    9 
#define FIQ_INVALID_EL0_64		10 
#define ERROR_INVALID_EL0_64	11 

#define SYNC_INVALID_EL0_32		12 
#define IRQ_INVALID_EL0_32		13 
#define FIQ_INVALID_EL0_32		14 
#define ERROR_INVALID_EL0_32	15 

#ifndef __ASSEMBLER__


#include "uart.h"
#include "utils.h"
#include "timer.h"

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


void _load_user_program(void*, void*);
int get_el(void);
void print_el();
void ret_from_fork(void);

void enable_interrupt();
void disable_interrupt();
void default_exception_handler();
void handle_sync_el1(unsigned long esr_el1, unsigned long elr_el1);
void handle_sync_el0(unsigned long esr_el1, unsigned long elr_el1);

#endif

#endif
