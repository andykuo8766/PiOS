#ifndef _EXEC_H
#define _EXEC_H


#include "sched.h"


void svc_handler(unsigned long *trap_frame);
void dump_interrupt_reg(unsigned long type, unsigned long elr_el1, unsigned long esr_el1);



#endif
