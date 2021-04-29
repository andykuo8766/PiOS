#ifndef __TIMER_H__
#define __TIMER_H__

#include "uart.h"
#include "system.h"
#include "mm.h"
#include "shell.h"
// #define TIME_FREQ 0x03b9aca0


typedef struct timeout_event {
  unsigned int register_time;
  unsigned int duration;
  void (*callback)(char *);
  char args[20];
  struct timeout_event *prev, *next;
} timeout_event;

timeout_event *timeout_queue_head, *timeout_queue_tail;

void timeout_event_init();
void set_timeout(char *args);
void timer_callback(char *msg); 
void add_timer(void (*callback)(char *), char *args, unsigned int duration);
void el1_timer_irq();
void el0_timer_irq();
unsigned long get_current_time();
void set_expired_time(unsigned int duration);

void core_timer_enable();
void core_timer_disable();
void core_timer_handler();
unsigned long get_current_timer_cnt();
unsigned long get_timer_freq();


#endif
