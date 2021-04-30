#include "sched.h"

static Thread *run_queue;
static unsigned long next_id = 1;
static Thread_reg idle_reg;


void idle(){
    while(1){
        kill_zombies();
        schedule();
    }
}


unsigned long thread_create(void (*func)()){
    Thread *new = kmalloc(sizeof(Thread));
    if(!new){
        return -1;
    }
    new->next = NULL;
    new->pid = next_id++;
    new->ppid = 0;
    new->entry = func;
    new->kernel_stack = buddy_alloc(1);
    if(!new->kernel_stack){
        kfree(new);
        return -1;
    }
    new->user_stack_npage = 0;
    new->user_stack = NULL;
    new->state = 1;
    new->thread_time = 0;
    thread_copy_reg(&new->reg, (char *)new->kernel_stack + PAGE_SIZE, exit, (char *)new->kernel_stack + PAGE_SIZE);

    Thread *t = current_thread();
    if(t){
        new->ppid = t->pid;
    }

    if(!run_queue){
        run_queue = new;
        return new->pid;
    }

    Thread *lastq = run_queue;
    while(lastq->next){
        lastq = lastq->next;
    }
    lastq->next = new;

    return new->pid;
}

void exit(){
    Thread *t = current_thread();
    if(t){
		uart_puts("exit from pid: ");
		uart_put_int(t->pid);
		uart_puts("\n");
        t->state = 0;
    }
    schedule();
}

Thread *current_thread(){
    Thread_reg *current_reg = get_current();
    Thread *t = run_queue;
    while(current_reg && t){
        if(&t->reg == current_reg){
            return t;
        }
        t = t->next;
    }
    return NULL;
}
Thread *get_thread(unsigned long pid){
    Thread *t = run_queue;
    while(t){
        if(t->pid == pid){
            return t;
        }
        t = t->next;
    }
    return NULL;
}

void kill_zombies(){
    Thread *prev = NULL;
    Thread *curr = run_queue;
    Thread *next = NULL;
    while(curr){
        next = curr->next;
        if(curr->state == 0){
            if(!prev){
                run_queue = next;
            }else{
                prev->next = next;
            }
            kfree(curr->kernel_stack);
            kfree(curr->user_stack);
            kfree(curr);
			uart_puts("killed zombie pid: ");
			uart_put_int(curr->pid);
			uart_puts("\n");
        }else{
            prev = curr;
        }
        curr = next;
    }
}


void schedule(){
    if(!run_queue){
        return;
    }
    Thread *curr = current_thread();
    if(!curr){
        //printf("idle switch_to %ld" NEW_LINE, run_queue->pid);
        if(run_queue->state == 1){
            run_queue->state = 2;
            switch_to(&idle_reg, &run_queue->reg, run_queue->entry);
        }else{
            switch_to(&idle_reg, &run_queue->reg, NULL);
        }
    }else{
        curr->thread_time = 0;
        Thread *next = curr->next;
        while(next && next->state == 0){
            next = next->next;
        }
        if(!next){
            //printf("%ld switch_to idle" NEW_LINE, curr->pid);
            switch_to(&curr->reg, &idle_reg, NULL);
        }else{
            //printf("%ld switch_to %ld" NEW_LINE, curr->pid, next->pid);
            if(next->state == 1){
                next->state = 2;
                switch_to(&curr->reg, &next->reg, next->entry);
            }else{
                switch_to(&curr->reg, &next->reg, NULL);
            }
        }
    }
}

unsigned long fork(){
    Thread *t = current_thread();
    if(!t){
		uart_puts("Warning: No running thread to fork!!!\n");
        return -1;
    }
    Thread *new = kmalloc(sizeof(Thread));
    if(!new){
        return -1;
    }
    new->next = NULL;
    new->pid = next_id++;
    new->ppid = t->pid;
    new->entry = t->entry;
    new->kernel_stack = buddy_alloc(1);
    if(!new->kernel_stack){
        kfree(new);
        return -1;
    }
    new->user_stack_npage = 0;
    new->user_stack = NULL;
    new->state = t->state;
    new->thread_time = t->thread_time;

    fork_stack(new->kernel_stack, t->kernel_stack, PAGE_SIZE);
    if(t->user_stack){
        new->user_stack_npage = t->user_stack_npage;
        new->user_stack = buddy_alloc(new->user_stack_npage);
        fork_stack(new->user_stack, t->user_stack, new->user_stack_npage * PAGE_SIZE);
    }
    thread_copy_reg(&new->reg, ((void *)get_fp() - t->kernel_stack) + new->kernel_stack, NULL, ((void *)get_sp() - t->kernel_stack) + new->kernel_stack);

    if(current_thread()->pid == new->pid){
        return 0;
    }

    if(!run_queue){
        run_queue = new;
        return new->pid;
    }

    Thread *lastq = run_queue;
    while(lastq->next){
        lastq = lastq->next;
    }
    lastq->next = new;

    return new->pid;
}


void foo(){
    for(int i=0; i<10; i++) {
		uart_puts("Thread id: ");
		uart_put_int(current_thread()->pid);
		uart_puts(" ");
		uart_put_int(i);
		uart_puts("\n");
        delay(1000000);
        schedule();
    }
    //exit();
}
void user_test(){
    char* argv[] = {"argv_test", "-o", "arg2", 0};
    exec("argv_test", argv);
    //exec("preempt_test_delay.elf", NULL);
}

void test(){
    int N = 5;
    for(int i=0; i<N; i++){
        thread_create(foo);
    }
    idle();
}

void user(){

    thread_create(user_test);

    idle();
}




