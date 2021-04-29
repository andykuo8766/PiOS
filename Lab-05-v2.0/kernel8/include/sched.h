#ifndef _SCHED_H
#define _SCHED_H

#include "exce.h"
#include "uart.h"





#define TASKSIZE 4096
#define TASKEXIT 1
#define TASKFORK 2


typedef struct _Task{
	unsigned long context[12+1+2+1+31];//kreg+ksp & spsr+elr & usp+ureg
	int id;
	int status;
	unsigned long a_addr,a_size,child;
	struct _Task* next;
	/*
	task stack:this ~ this+TASKSIZE
	*/
}Task;

typedef struct{
	Task *beg,*end;
}RQ;

static RQ rq;
static int task_cnter;


void threadTest1();
void threadTest2();












/*
#define THREAD_SIZE				4096

#define TASK_RUNNING			0
#define TASK_ZOMBIE				1
#define TASK_WAITING            2


#define NR_TASKS				64 
#define PF_KTHREAD		        0x00000002	

extern struct task_struct *current;
extern struct task_struct *task[NR_TASKS];
extern int nr_tasks;

struct cpu_context {
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
	unsigned long sp;
	unsigned long pc;
};

struct task_struct {
    struct cpu_context cpu_context;
	long task_id;
    long state;
    long counter;
    long priority;
    long preempt_count;
	unsigned long stack;
	unsigned long flags;
	unsigned long kill_flag;
};

void preempt_disable(void);
void preempt_enable(void);
void schedule();

void cpu_switch_to(struct task_struct* prev, struct task_struct* next);
void schedule_uart();
*/



#endif
