#include "timer.h"

const unsigned int interval = SYSTEM_TIMER_INTERVAL;
unsigned int curVal    = 0;

void enable_timer_controller(){
	put32(ENABLE_IRQS_1, SYSTEM_TIMER_IRQ_1);
	return;
}

void disable_timer_controller(){
	put32(DISABLE_IRQS_1, 1 << 1);
	return;
}

void sys_timer_init (){
	curVal = get32(TIMER_CLO);
	curVal += interval;
	put32(TIMER_C1, curVal);
	return;
}

void enable_core_timer(){
    unsigned long cntfrq;
    unsigned long val;
    cntfrq = read_cntfrq();
    write_cntp_tval(cntfrq);    // clear cnthp interrupt and set next 1 sec timer.
    val = read_cntp_tval();
    core_timer_enable();
    enable_irq();
}

unsigned long read_cntfrq(){
    unsigned long val;
	asm volatile ("mrs %0, cntfrq_el0" : "=r" (val));
    return val;
}
void write_cntp_tval(unsigned int val){
	asm volatile ("msr cntp_tval_el0, %0" :: "r" (val));
    return;
}
unsigned long read_cntp_tval(){
    unsigned int val;
	asm volatile ("mrs %0, cntp_tval_el0" : "=r" (val));
    return val;
}



