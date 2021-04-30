#ifndef __TIMER_H__
#define __TIMER_H__
#include "arm.h"

void arm_core_timer_enable();
void arm_core_timer_disable();
void arm_local_timer_enable();
void arm_local_timer_disable();
void clean_core_timer();

#endif
