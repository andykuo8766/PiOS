#include "uart.h"

void dumpState(){
	unsigned long esr,elr,spsr;
	asm volatile("mrs %0, esr_el1	\n":"=r"(esr):);
	asm volatile("mrs %0, elr_el1	\n":"=r"(elr):);
	asm volatile("mrs %0, spsr_el1	\n":"=r"(spsr):);

	uart_puts("--------------------\n");
	uart_puts("SPSR: ");
	uart_put_hex(spsr);
	uart_puts("\n\r");
	uart_puts("ELR: ");
	uart_put_hex(elr);
	uart_puts("\n\r");	
	uart_puts("ESR: ");
	uart_put_hex(esr);
	uart_puts("\n\r");		
	uart_puts("--------------------\n");
	
	//uart_printf("Exception Return Address: 0x%x\n",elr);
	//uart_printf("Exception Class: 0x%x\n",(esr>>26)&0x3f);//0x15 for svc inst
	//uart_printf("Instruction Specific Syndrome: 0x%x\n",esr&0x1ffffff);//issued imm value for svc inst
}

void exception_handler(){
	dumpState();
}

void interrupt_handler(){
	asm volatile("mrs x0, cntfrq_el0	\n");
	asm volatile("add x0, x0, x0		\n");
	asm volatile("msr cntp_tval_el0, x0	\n");
	unsigned long cntpct,cntfrq,tmp;
	asm volatile("mrs %0, cntpct_el0	\n":"=r"(cntpct):);
	asm volatile("mrs %0, cntfrq_el0	\n":"=r"(cntfrq):);

	tmp=cntpct*10/cntfrq;
	uart_puts("--------------------\n");
	uart_puts("Time Elapsed: ");
	uart_put_int(tmp/10);	
	uart_puts(".");
	uart_put_int(tmp%10);	
	uart_puts("s\n");
	uart_puts("--------------------\n");
}

void error_handler(){
	dumpState();
	uart_puts("unknown exception...\n");
	while(1){}
}

void print_el(){
	int el = get_el();
	uart_puts("Exception Level:");
	uart_put_int(el);
	uart_puts("\n");
}