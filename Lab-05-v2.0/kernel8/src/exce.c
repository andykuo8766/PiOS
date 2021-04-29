#include "exce.h"

void enable_interrupt() { asm volatile("msr DAIFClr, 0xf"); }

void disable_interrupt() { asm volatile("msr DAIFSet, 0xf"); }

void default_exception_handler(){
/*
	unsigned long esr,elr,spsr;
	asm volatile("mrs %0, esr_el1	\n":"=r"(esr):);
	asm volatile("mrs %0, elr_el1	\n":"=r"(elr):);
	asm volatile("mrs %0, spsr_el1	\n":"=r"(spsr):);
	uart_puts("---------------------\n");
	uart_puts("= SPSR : ");
	uart_put_hex(spsr);
	uart_puts(" =\n\r");
	uart_puts("= ELR  : ");
	uart_put_hex(elr);
	uart_puts(" =\n\r");	
	uart_puts("= ESR  : ");
	uart_put_hex(esr);
	uart_puts(" =\n\r");		
	uart_puts("---------------------\n");
*/
}
char *entry_error_messages[] = {
	"SYNC_INVALID_EL1t\n",
	"IRQ_INVALID_EL1t\n",		
	"FIQ_INVALID_EL1t\n",		
	"ERROR_INVALID_EL1T\n",		

	"SYNC_INVALID_EL1h\n",		
	"IRQ_INVALID_EL1h\n",		
	"FIQ_INVALID_EL1h\n",		
	"ERROR_INVALID_EL1h\n",		

	"SYNC_INVALID_EL0_64\n",		
	"IRQ_INVALID_EL0_64\n",		
	"FIQ_INVALID_EL0_64\n",		
	"ERROR_INVALID_EL0_64\n",	

	"SYNC_INVALID_EL0_32\n",		
	"IRQ_INVALID_EL0_32\n",		
	"FIQ_INVALID_EL0_32\n",		
	"ERROR_INVALID_EL0_32\n"	
};

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address){
	uart_puts("ERROR TYPE ");
    uart_puts(entry_error_messages[type]);
	uart_puts("\r\n");
	uart_puts("Exception class (EC) ");
	uart_put_hex(esr>>26);
	uart_puts("Instruction specific syndrome (ISS) ");
    uart_put_hex(esr & 0xfff);	
}

void handle_sync_el1(unsigned long esr_el1, unsigned long elr_el1){
	uart_puts("handle_sync_el1\n");

}
void handle_sync_el0(unsigned long esr_el1, unsigned long elr_el1){
	uart_puts("handle_sync_el0\n");

/*
	unsigned long esr,elr,spsr;
	asm volatile("mrs %0, esr_el1	\n":"=r"(esr):);
	asm volatile("mrs %0, elr_el1	\n":"=r"(elr):);
	asm volatile("mrs %0, spsr_el1	\n":"=r"(spsr):);
	uart_puts("---------------------\n");
	uart_puts("= SPSR : ");
	uart_put_hex(spsr);
	uart_puts(" =\n\r");
	uart_puts("= ELR  : ");
	uart_put_hex(elr);
	uart_puts(" =\n\r");	
	uart_puts("= ESR  : ");
	uart_put_hex(esr);
	uart_puts(" =\n\r");		
	uart_puts("---------------------\n");
	*/

}

void handle_el1_irq(){
	disable_interrupt();
	//uart_puts("handle_el1_irq\n");
    unsigned int uart = (*IRQ_PENDING_1 & AUX_IRQ);
    unsigned int core_timer = (*CORE0_INTERRUPT_SOURCE & 0x2);
    if(uart){
        //uart_puts("uart el1\r\n");
        uart_handler();
    }
    else if(core_timer){// CNTPNIRQ
        //uart_puts("core timer el1\r\n");
        core_timer_handler();
    }
	enable_interrupt();
}
void handle_el0_irq(){
	disable_interrupt();
	//uart_puts("handle_el0_irq\n");
    unsigned int uart = (*IRQ_PENDING_1 & AUX_IRQ);
    unsigned int core_timer = (*CORE0_INTERRUPT_SOURCE & 0x2);
    if(uart){
        //uart_puts("uart\r\n");
        uart_handler();
    }
    else if(core_timer){
        //uart_puts("core time\r\n");
        el0_timer_irq();
    }
	enable_interrupt();
}

void print_el(){
	int el = get_el();
	uart_puts("Exception Level:");
	uart_put_int(el);
	uart_puts("\n");
}

