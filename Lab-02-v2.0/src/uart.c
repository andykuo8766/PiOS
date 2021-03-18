#include "gpio.h"
#include "uart.h"
#include "utils.h"



/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init()
{
    register unsigned int reg;

    /* initialize UART */
    *AUX_ENABLE     |= 1;       /* enable mini UART */
    *AUX_MU_CNTL     = 0;       /* Disable transmitter and receiver during configuration. */
    *AUX_MU_IER      = 0;       /* Disable interrupt */
    *AUX_MU_LCR      = 3;       /* Set the data size to 8 bit. */
    *AUX_MU_MCR      = 0;       /* Don’t need auto flow control. */
    *AUX_MU_BAUD     = 270;     /* 115200 baud */
    *AUX_MU_IIR      = 6;       /* No FIFO */
    

    /* map UART1 to GPIO pins */
    reg = *GPFSEL1;
    reg &= ~((7<<12)|(7<<15));  /* address of gpio 14, 15 */
    reg |=   (2<<12)|(2<<15);   /* set to alt5 */
    *GPFSEL1 = reg;             /* enable gpio 14 and 15 */

    *GPPUD = 0;                 /*  disable pull-up/down */
	/*  disable need time */
    reg=150;
    while ( reg-- )
    { 
        asm volatile("nop"); 
    }
    
    *GPPUDCLK0 = (1<<14)|(1<<15);
	/*  disable need time */
    reg=150; 
    while ( reg-- )
    {
        asm volatile("nop");
    }
    *GPPUDCLK0 = 0;             /* flush GPIO setup */

    *AUX_MU_CNTL = 3;           /* Enable the transmitter and receiver. */
}


/**
 * Send a character by uart
 */
void uart_send(char c) {
    /* Wait until we can send */
    do {
        asm volatile("nop");
    } while( ! ( *AUX_MU_LSR&0x20 ));
    /* write the character to the buffer */   
    *AUX_MU_IO = c;
}

/**
 * Receivce character by uart
 */
char uart_recv() {
    char r;
    /* wait until something is in the buffer */
    do{        
        asm volatile("nop");        
    } while ( ! ( *AUX_MU_LSR&0x01 ) );
    /* read it and return */
    r = ( char )( *AUX_MU_IO );
    return r;
}

/**
 * UART user interface receive character
 */
char uart_getc() {
    char r = uart_recv();
    /* convert carrige return to newline */
    return r == '\r' ? '\n' : r;
}

/**
 * UART user interface put character
 */
void uart_putc(void* p, char c) {
    /* convert newline to carrige return + newline */
    if(c == '\n')
        uart_send('\r');
    uart_send(c);
}

/**
 * UART user interface put string
 */
void uart_puts(char *s) {
    while(*s) {
        uart_putc(0, *s++);
    }
}

int uart_read_int() {
    int num = 0;
    for (int i = 0; i < 4; i++) {
        char c = uart_recv();
        num = num << 8;
        num += (int)c;
    }
    return num;
}

void uart_send_int(int number) {
    uart_send((char)((number >> 24) & 0xFF));
    uart_send((char)((number >> 16) & 0xFF));
    uart_send((char)((number >> 8) & 0xFF));
    uart_send((char)(number & 0xFF));
}

/**
 * Display a binary value in hexadecimal
 */
void uart_hex(unsigned int d) {
    unsigned int n;
    int c;
    for(c=28;c>=0;c-=4) {
        // get highest tetrad
        n=(d>>c)&0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n+=n>9?0x37:0x30;
        uart_send(n);
    }
}

