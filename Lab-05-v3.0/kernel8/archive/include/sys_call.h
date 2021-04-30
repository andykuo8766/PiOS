#ifndef __SYS_CALL_H
#define __SYS_CALL_H

extern void delay(int cycles);

int getpid();
unsigned int uart_read(char buf[], unsigned int size);
unsigned int uart_write(const char buf[], unsigned int size);
int exec(const char* name, char *const argv[]);
void exit();
int fork();

#endif
