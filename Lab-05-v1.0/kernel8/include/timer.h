#ifndef __TIMER_H__
#define __TIMER_H__

#include "uart.h"
#include "exce.h"
#include "gpio.h"

#define TIMER_CS        (MMIO_BASE+0x00003000)
#define TIMER_CLO       (MMIO_BASE+0x00003004)
#define TIMER_CHI       (MMIO_BASE+0x00003008)
#define TIMER_C0        (MMIO_BASE+0x0000300C)
#define TIMER_C1        (MMIO_BASE+0x00003010)
#define TIMER_C2        (MMIO_BASE+0x00003014)
#define TIMER_C3        (MMIO_BASE+0x00003018)

#define TIMER_CS_M0	(1 << 0)
#define TIMER_CS_M1	(1 << 1)
#define TIMER_CS_M2	(1 << 2)
#define TIMER_CS_M3	(1 << 3)

#define CORE0_TIMER_IRQ_CTRL 0x40000040
#define SYSTEM_TIMER_INTERVAL 1000000

void core_timer_enable();
void core_timer_disable();
void enable_timer_controller();
void disable_timer_controller();
void sys_timer_init ();
void enable_core_timer();
unsigned long read_cntfrq();
void write_cntp_tval(unsigned int val);
unsigned long read_cntp_tval();

#endif
