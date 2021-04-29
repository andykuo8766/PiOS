#include "sched.h"


void threadSchedule(){
	if(!rq.beg){
		uart_puts("rq is empty!!\n");
		while(1){}
	}
	
	if(rq.beg==rq.end){
		//no other task, all done
	}else{
		do{
			rq.end->next=rq.beg;
			rq.end=rq.beg;
			rq.beg=rq.beg->next;
			rq.end->next=0;
		}while(rq.beg->status);//ignore abnormal task

		//uart_printf("%d %x %x %x\n",rq.beg->id,rq.beg->context[10],rq.beg->context[11],rq.beg->context[12]);
		//uart_printf("%d -> %d\n",rq.end->id,rq.beg->id);
		asm volatile("\
			mov x1, %0\n\
			mrs x0, tpidr_el1\n\
			bl cpu_switch_to\n\
		"::"r"(rq.beg));//only use bl to avoid stack usage
	}
}

Task* threadCreate(void* func){
	Task* new_task=(Task*)kmalloc(TASKSIZE);
	if((unsigned long)new_task%TASKSIZE){//aligned
		uart_puts("new_task isn't aligned!!\n");
		while(1){}
	}
	
	new_task->context[10]=(unsigned long)new_task+TASKSIZE;//fp
	new_task->context[11]=(unsigned long)func;//lr
	new_task->context[12]=(unsigned long)new_task+TASKSIZE;//sp
	new_task->id=task_cnter++;
	new_task->status=0;
	new_task->a_addr=new_task->a_size=new_task->child=0;
	new_task->next=0;

	if(rq.beg){
		rq.end->next=new_task;
		rq.end=rq.end->next;
	}else{
		rq.beg=rq.end=new_task;
	}

	return new_task;
}

void zombiesKill(){//called by idle()
	uart_puts("kill zombies\n");
	Task* tar=rq.beg;
	while(1){
		while(tar->next&&(tar->next->status&TASKEXIT)){
			Task* tmp=tar->next->next;
			kfree((unsigned long)(tar->next));
			tar->next=tmp;
		}

		if(!tar->next){
			rq.end=tar;
			break;
		}else{
			tar=tar->next;
		}
	}
}

void taskUpdate(Task* p,Task* c){
	p->status^=TASKFORK;
	p->child=c->id;

	Task* tmp=c->next;
	char* src=(char*)p;
	char* dst=(char*)c;
	for(int i=0;i<TASKSIZE;++i){//task copy
		*dst=*src;
		dst++;
		src++;
	}

	c->id=p->child;
	uart_puts("Please enter app load address (Hex): ");
	c->a_addr=uart_get_hex(1);
	c->child=0;
	c->next=tmp;

	long k_delta=(long)c-(long)p;
	long a_delta=(long)c->a_addr-(long)p->a_addr;
	c->context[10]+=k_delta;//kernel fp
	c->context[12]+=k_delta;//kernel sp
	c->context[14]+=a_delta;//elr_el1
	c->context[15]+=a_delta;//sp_el0
	c->context[45]+=a_delta;//user fp
	c->context[46]+=a_delta;//user lr

	src=(char*)(p->context[15]);
	dst=(char*)(c->context[15]);
	for(int i=0,ii=p->a_addr+p->a_size-(p->context[15]);i<ii;++i){//program copy
		*dst=*src;
		dst++;
		src++;
	}
}

void doFork(){//called by idle()
	Task* tar=rq.beg->next;
	while(tar){
		if((tar->status)&TASKFORK){
			Task* child=threadCreate(0);
			taskUpdate(tar,child);
		}
		tar=tar->next;
	}
}



void idle(){
	//while(1){
		//uart_printf("idle()\n");
		//uart_getc();
		zombiesKill();
		//doFork();
		threadSchedule();
	//}
}

int tidGet(){
	Task* cur;
	asm volatile("mrs %0, tpidr_el1\n":"=r"(cur):);
	return cur->id;
}

void exec(char* path,char** argv){//will not reset sp...
	unsigned long a_addr;
	uart_puts("Please enter app load address (Hex): ");
	a_addr=uart_get_hex(1);
	//loadApp_with_argv(path,a_addr,argv,&(rq.beg->a_addr),&(rq.beg->a_size));
	exit();
}

void exit(){
	Task* cur;
	asm volatile("mrs %0, tpidr_el1\n":"=r"(cur):);
	cur->status|=TASKEXIT;
	threadSchedule();

	while(1){
		uart_puts("exit() failed!!\n");
	}
}

void foo1(){
	for(int i=0;i<10;++i){
		//uart_getc();
		uart_puts("Thread id: ");
		uart_put_int(tidGet());
	  	uart_puts(" ");
	  	uart_put_int(i);
	  	uart_puts("\n");
		threadSchedule();
	}

	exit();
}

void threadTest1(){
	Task* cur=threadCreate(0);//use startup stack (not kernel stack)
	asm volatile("msr tpidr_el1, %0\n"::"r"((unsigned long)cur));

	for(int i=0;i<3;++i){
		threadCreate(foo1);
	}

	idle();
}

void foo2(){
	char* argv[]={"argv_test","-o","arg2",0};
	exec("app1",argv);
}

void threadTest2(){
	Task* cur=threadCreate(0);//use startup stack (not kernel stack)
	asm volatile("msr tpidr_el1, %0\n"::"r"((unsigned long)cur));//TODO

	threadCreate(foo2);

	idle();
}


/*
struct task_struct init_task = { {0,0,0,0,0,0,0,0,0,0,0,0,0},0,0,0,1, 0, 0, PF_KTHREAD, 0};
struct task_struct *current = &(init_task);
struct task_struct * task[NR_TASKS] = {&(init_task), };
int nr_tasks = 1;

void switch_to(struct task_struct * next) {
	if (current == next) 
		return;
	struct task_struct * prev = current;
	current = next;
	cpu_switch_to(prev, next);
}

void _schedule(void){
	int next,c;
	struct task_struct * p;
	while (1) {
		c = -1;
		next = 0;
		for (int i = 0; i < NR_TASKS; i++){
			p = task[i];
			if (p && p->state == TASK_RUNNING && p->counter > c) {
				c = p->counter;
				next = i;
			}
		}
		if (c) {
			break;
		}
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			if (p) {
				p->counter = (p->counter >> 1) + p->priority;
			}
		}
	}
	if (task[next]->kill_flag == 1) {
		task[next]->state = TASK_ZOMBIE;
		nr_tasks -= 1;
		_schedule();
	}
	else {
		switch_to(task[next]);
	}
	return;
}

void schedule_uart(void){
	current->counter = 0;
	preempt_disable();
	int next,c;
	struct task_struct * p;
	while(1) {
		c = -1;
		next = 0;
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			if (p && p->state == TASK_WAITING && p->counter > c) {
				c = p->counter;
				next = i;
			}
		}
		if (c == -1) {
			return; //not waiting task
		}
		if (c) {
			break;
		}
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			if (p && p->state == TASK_WAITING) {
				p->counter = (p->counter >> 1) + p->priority;
			}
		}
	}
	task[next]->state = TASK_RUNNING;
	switch_to(task[next]);
	return;
}

void schedule(void){
	current->counter = 0;
	_schedule();
}

void schedule_tail(void) {
	//enable
	preempt_enable();
}

void preempt_disable(void){
	current->preempt_count++;
}

void preempt_enable(void){
	current->preempt_count--;
}

void timer_tick(){
    enable_interrupt();
    if(current->counter > 0){
        current->counter--;
		uart_puts(">>>>Task_id:");
		uart_put_int(current->task_id);
		uart_puts("  Counter value: ");
		uart_put_int(current->counter);
		uart_puts("\n");
    }else
        current->kill_flag=1;
    disable_interrupt();
}
*/
