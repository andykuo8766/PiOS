#include "exce.h"


void default_exception_handler(int type,unsigned long esr,unsigned long address){
	unsigned long elr,spsr;
	asm volatile("mrs %0, esr_el1	\n":"=r"(esr):);
	asm volatile("mrs %0, elr_el1	\n":"=r"(elr):);
	asm volatile("mrs %0, spsr_el1	\n":"=r"(spsr):);
	uart_puts("---------------------\n");
	uart_puts("ERROR TYPE ");
    uart_puts(entry_error_messages[type]);
	uart_puts("Exception class (EC) ");
	uart_put_hex((unsigned long)esr>>26);
	uart_puts("Instruction specific syndrome (ISS) ");
	uart_put_hex((unsigned long)esr & 0xfff);	
	uart_puts("---------------------\n");

}
void handle_el0_irq(){
	unsigned int sec_level_irq = get32(IRQ_PENDING_1);
	unsigned int fir_level_irq = get32(CORE0_INTERRUPT_SOURCE);
	if(sec_level_irq & AUX_IRQ) {
		//schedule_uart();
	}
	else if (fir_level_irq == 2) {
		handle_core_timer_irq();
	}
}
void handle_el1_irq(){
	unsigned int fir_level_irq = get32(CORE0_INTERRUPT_SOURCE);
	unsigned int sec_level_irq = get32(IRQ_PENDING_1);
	if(sec_level_irq & AUX_IRQ) {
		//schedule_uart();
	}
	else if (fir_level_irq == 2) {
		handle_core_timer_irq();
	}
}

void handle_el0_sync(unsigned long par1, unsigned long par2){
    unsigned int val;
    asm volatile ("uxtw %0, w8" : "=r" (val));
    if (val == SYS_CLONE) {
        //uart_send("return from child\r\n");
        return privilege_task_create(0, 0, 0, 0);
    }
    if (val == SYS_EXIT_NUMBER) {
        for (int i = 0 ; i < NR_TASKS ; i++) {
            if (task[i] == current) {
                task[i]->state = par1;
                nr_tasks -= 1;
                break;
            }
        }
        //TODO free user page
        schedule();
    }
    if (val == SYS_UART_READ) {
        // par1 is buffer address
        // par2 is read size
        char c;
        int  i = 0;
        char *buf_ptr = par1;
        while(1) {
		    if(get32(AUX_MU_LSR)&0x01) {
                c = get32(AUX_MU_IO)&0xFF;
                *(buf_ptr + i) = c;
                i++;
                if (i == par2) {
                    break;
                }
            } 
            else if(i > 0) {
                break;
            }
	    }
        return i;
    }
    if (val == SYS_UART_WRITE) {
        unsigned long i;
        char *buf_ptr = par1; 
        for (i = 0 ; i < par2 ;i++) {
            uart_send(*(buf_ptr + i));
        }
        return i;
    }
    if (val == SYS_EXEC) {
        return do_exec(par1);
    }
    if (val == SYS_FORK) {
        return privilege_task_create(0, 0, 0, 0);
    }
    if (val == SYS_ID) {
        unsigned long i = current->task_id;
        return i;
    }
    if (val == SYS_KILL) {
        unsigned long pid = par1;
        struct task_struct *killed_child; 
        killed_child = (struct task_struct *)(LOW_KERNEL_STACK + (pid - 1) *PAGE_SIZE);
        killed_child->kill_flag = 1;
    }
    if (val == SYS_UART_EN) {
        put32(ENABLE_IRQS_1, AUX_IRQ);
    }
}

void handle_el1_sync(unsigned long esr, unsigned long address){
    unsigned long val;
	asm volatile ("uxtw %0, w8" : "=r" (val));
    if (val == SYS_TIME_IRQ) {
        sys_timer_init();
        enable_timer_controller();  // function in irq.c
        unsigned long cntfrq;
        cntfrq = read_cntfrq();
        write_cntp_tval(cntfrq);    // clear cnthp interrupt and set next 1 sec timer.
        val = read_cntp_tval();
        core_timer_enable();
        enable_irq();
    }
}

void handle_core_timer_irq(){
	unsigned int val;
	asm volatile ("mrs %0, cntfrq_el0" : "=r" (val)); // read val
	asm volatile ("msr cntp_tval_el0, %0" :: "r" (val)); // write tval
	uart_puts("Arm core timer interrupt, jiffies ");
	uart_put_int(core_jf);
	uart_puts("\r\n");
	core_jf += 1;
	current->counter -= 1;
	if (current->counter <= 0 ) {
		current->counter = 0;
		schedule();
	}
}

