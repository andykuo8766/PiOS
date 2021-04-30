#include "exce.h"


void svc_handler(unsigned long *trap_frame){
    switch(trap_frame[22]){
    case (1):
        sys_getpid(trap_frame);
        break;
    case (2):
        sys_uart_read(trap_frame);
        break;
    case (3):
        sys_uart_write(trap_frame);
        break;
    case (4):
        sys_exec(trap_frame);
        break;
    case (5):
        sys_exit(trap_frame);
        break;
    case (6):
        sys_fork(trap_frame);
        break;
    default:
    	uart_puts("Warning: Unknown sys_call\n");
        //printf("Warning: Unknown sys_call #%lu!!!" NEW_LINE, trap_frame[22]);
        //Infinite_Loop(1);
        break;
    }
}

void thread_add_time(){
    current_thread()->thread_time++;
}

void if_schedule(){
    if(current_thread()->thread_time >= 8){  //time slice = 1 second
        schedule();
    }
}

void dump_interrupt_reg(unsigned long type, unsigned long elr_el1, unsigned long esr_el1){
	uart_puts("type = ");
	uart_put_int(type);
	uart_puts(", elr_el1 = ");
	uart_put_hex(elr_el1);
	uart_puts(", esr_el1 = ");
	uart_put_hex(esr_el1);
	uart_puts("\n");

	/*
    volatile uint32_t *p = (uint32_t *)0x3F00B200U;
    printf("IRQ basic pending = %x" NEW_LINE, *p);
    p = (uint32_t *)0x3F00B204U;
    printf("IRQ pending 1 = %x" NEW_LINE, *p);
    p = (uint32_t *)0x3F00B208U;
    printf("IRQ pending 2 = %x" NEW_LINE, *p);
    p = (uint32_t *)0x3F00B20CU;
    printf("FIQ control = %x" NEW_LINE, *p);
    p = (uint32_t *)0x3F00B210U;
    printf("Enable IRQs 1 = %x" NEW_LINE, *p);
    p = (uint32_t *)0x3F00B214U;
    printf("Enable IRQs 2 = %x" NEW_LINE, *p);
    p = (uint32_t *)0x3F00B218U;
    printf("Enable Basic IRQs = %x" NEW_LINE, *p);
    p = (uint32_t *)0x3F00B21CU;
    printf("Disable IRQs 1 = %x" NEW_LINE, *p);
    p = (uint32_t *)0x3F00B220U;
    printf("Disable IRQs 2 = %x" NEW_LINE, *p);
    p = (uint32_t *)0x3F00B224U;
    printf("Disable Basic IRQs = %x" NEW_LINE, *p);
    miniuart_send_nC('-', 22);
    miniuart_send_S(NEW_LINE);
	*/
}

