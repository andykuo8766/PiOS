#ifndef _SCHED_H
#define _SCHED_H

#include "mm.h"
#include "timer.h"
#include "cpio.h"




typedef struct {
    unsigned long x19;
    unsigned long x20;
    unsigned long x21;
    unsigned long x22;
    unsigned long x23;
    unsigned long x24;
    unsigned long x25;
    unsigned long x26;
    unsigned long x27;
    unsigned long x28;
    unsigned long fp;
    unsigned long lr;
    unsigned long sp;
} Thread_reg;

typedef struct __Thread {
    struct __Thread *next;
    unsigned long pid;
    unsigned long ppid;
    void (*entry)();
    void *kernel_stack;  //fix 1 page
    unsigned int user_stack_npage;
    void *user_stack;
    unsigned int state;  //0 == exit, 1 == init, 2 == running/idle
    Thread_reg reg;
    unsigned long thread_time;
} Thread;

void idle();
unsigned long thread_create(void (*func)());
void exit();
Thread *current_thread();
Thread *get_thread(unsigned long pid);
void kill_zombies();
void schedule();
unsigned long fork();


void thread_copy_reg(Thread_reg *reg, void *fp, void (*lr)(), void *sp);
Thread_reg *get_current();
void switch_to(Thread_reg *prev, Thread_reg *next, void (*entry)());
void fork_stack(void *child_fp, void *parent_fp, unsigned long stack_size);
unsigned long get_fp();
unsigned long get_sp();



void test();





















#endif
