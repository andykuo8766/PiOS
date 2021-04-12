#ifndef _EXECEPTION_H
#define _EXECEPTION_H
#include "uart.h"

void dumpState();
void exception_handler();
void interrupt_handler();
void error_handler();
void print_el();
#endif