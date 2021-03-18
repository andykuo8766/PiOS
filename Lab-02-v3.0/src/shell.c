#include "shell.h"
#include "string.h"
#include "command.h"
#include "uart.h"
#include "uart_boot.h"
#include "cpio.h"


extern volatile unsigned char _binary_ramdisk_start;

extern char __cpio_buf[];


void shell_start () {
    int buffer_counter = 0;
    char input_char;
    char buffer[MAX_BUFFER_LEN];
    enum SPECIAL_CHARACTER input_parse;

    strset (buffer, 0, MAX_BUFFER_LEN);   
    // new line head
    uart_puts(USER_NAME);
    uart_puts("@");
    uart_puts(MACHINE_NAME);
	uart_puts(":~$ ");
    // read input
    while(1)
    {
        input_char = uart_recv();
        input_parse = parse ( input_char );
        command_controller ( input_parse, input_char, buffer, &buffer_counter);
    }
}

enum SPECIAL_CHARACTER parse ( char c )
{
    if ( !(c < 128 && c >= 0) )
        return UNKNOWN;
    if ( c == BACK_SPACE )
        return BACK_SPACE;
    else if ( c == LINE_FEED || c == CARRIAGE_RETURN )
        return NEW_LINE;
    else
        return REGULAR_INPUT;    
}

void command_controller ( enum SPECIAL_CHARACTER input_parse, char c, char buffer[], int * counter )
{
	int flag = 0;
    if ( input_parse == UNKNOWN )
        return;
    
    // Special key
    if ( input_parse == BACK_SPACE )
    {
        if (  (*counter) > 0 )
            (*counter) --;
        
        uart_send(c);
        uart_send(' ');
        uart_send(c);
    }
    else if ( input_parse == NEW_LINE )
    {
        uart_send(c);

        if ( (*counter) == MAX_BUFFER_LEN ) 
        {
            input_buffer_overflow_message(buffer);
        }
        else 
        {
            buffer[(*counter)] = '\0';
            uart_puts("\r\n");
            if      ( !strcmp(buffer, "help"        ) ) command_help();
            else if ( !strcmp(buffer, "hello"       ) ) command_hello();
            else if ( !strcmp(buffer, "reboot"      ) ) {reset();flag = 1;}
			else if ( !strcmp(buffer, "cancel"      ) ) cancel_reset();
			else if ( !strcmp(buffer, "loadimg"      ) ) loadimg();
			//else if ( !strcmp(buffer, "cpio"      ) ) cpio_ls((char*)&_binary_ramdisk_start,MAX_BUFFER_LEN);
			else if ( !strcmp(buffer, "cpio"      ) ) cpio_ls((char*)__cpio_buf,MAX_BUFFER_LEN);
            else  {
				if ( !strcmp(buffer, ""      ) )     uart_puts("");
				else command_not_found(buffer);
            	}
        }
            
        (*counter) = 0;
        strset (buffer, 0, MAX_BUFFER_LEN); 

        // new line head;
        if(flag != 1){
            // new line head
    		uart_puts(USER_NAME);
    		uart_puts("@");
    		uart_puts(MACHINE_NAME);
			uart_puts(":~$ ");
        }
    }
    else if ( input_parse == REGULAR_INPUT )
    {
        uart_send(c);

        if ( *counter < MAX_BUFFER_LEN)
        {
            buffer[*counter] = c;
            (*counter) ++;
        }
    }
}

int read_command(char *buff, unsigned int size){
	int count = 0;
	char char_recv;
	while (count < size){
		char_recv = uart_getc();
		uart_putc(0, char_recv);
		if (char_recv == '\n'){
			return count;
		} else if (char_recv == '\177'){// backspace
			if (count >= 1){
				uart_puts("\b \b");
				count --;
			}
		} else {
			buff[count] = char_recv;
			count ++;
		}
	}
	return -1; // out of buff	
}


