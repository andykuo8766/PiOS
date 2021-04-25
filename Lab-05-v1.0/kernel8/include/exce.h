#ifndef _IRQ_H
#define _IRQ_H
#include "uart.h"
#include "utils.h"
#include "timer.h"
#include "svc.h"
#include "sched.h"
#include "gpio.h"
#include "mm.h"

#define IRQ_BASIC_PENDING	((volatile unsigned int*)(MMIO_BASE+0x0000B200))
#define IRQ_PENDING_1		((volatile unsigned int*)(MMIO_BASE+0x0000B204))
#define IRQ_PENDING_2		((volatile unsigned int*)(MMIO_BASE+0x0000B208))
#define FIQ_CONTROL		    ((volatile unsigned int*)(MMIO_BASE+0x0000B20C))
#define ENABLE_IRQS_1		((volatile unsigned int*)(MMIO_BASE+0x0000B210))
#define ENABLE_IRQS_2		((volatile unsigned int*)(MMIO_BASE+0x0000B214))
#define ENABLE_BASIC_IRQS	((volatile unsigned int*)(MMIO_BASE+0x0000B218))
#define DISABLE_IRQS_1		((volatile unsigned int*)(MMIO_BASE+0x0000B21C))
#define DISABLE_IRQS_2		((volatile unsigned int*)(MMIO_BASE+0x0000B220))
#define DISABLE_BASIC_IRQS	((volatile unsigned int*)(MMIO_BASE+0x0000B224))


#define SYSTEM_TIMER_IRQ_0	(1 << 0)
#define SYSTEM_TIMER_IRQ_1	(1 << 1)
#define SYSTEM_TIMER_IRQ_2	(1 << 2)
#define SYSTEM_TIMER_IRQ_3	(1 << 3)

#define CORE0_INTERRUPT_SOURCE 0x40000060
#define AUX_IRQ (1 << 29)


char *entry_error_messages[] = {
	"SYNC_INVALID_EL1t\n",
	"IRQ_INVALID_EL1t\n",		
	"FIQ_INVALID_EL1t\n",		
	"ERROR_INVALID_EL1T\n",		

	"SYNC_INVALID_EL1h\n",		
	"IRQ_INVALID_EL1h\n",		
	"FIQ_INVALID_EL1h\n",		
	"ERROR_INVALID_EL1h\n",		

	"SYNC_INVALID_EL0_64\n",		
	"IRQ_INVALID_EL0_64\n",		
	"FIQ_INVALID_EL0_64\n",		
	"ERROR_INVALID_EL0_64\n",	

	"SYNC_INVALID_EL0_32\n",		
	"IRQ_INVALID_EL0_32\n",		
	"FIQ_INVALID_EL0_32\n",		
	"ERROR_INVALID_EL0_32\n"	
};



struct trapframe {
    unsigned long Xn[29]; // general register
    unsigned long fp;
    unsigned long lr;
    unsigned long sp_el0;
    unsigned long elr_el1;
    unsigned long spsr_el1;
};
unsigned int core_jf   = 1;

// void enable_interrupt_controller();
// void handle_irq();
void enable_irq();
void disable_irq();
void ret_fork_child();
void ret_from_fork(void);
void handle_core_timer_irq();


#endif
