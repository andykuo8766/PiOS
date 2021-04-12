#ifndef _UTILS_H
#define _UTILS_H
#include "uart.h"

#define NULL                    ((void *)0) 

void *memset(void *s, int c, unsigned long n);
int strcmp(const char *a, const char *b);
int strlen(const char* s);
unsigned long hextoint(char* addr, const int size);
void swap(void **a, void **b) ;
extern int get_el ( void );
extern void el1_to_el0 (unsigned long);





#endif /*_UTILS_H */
