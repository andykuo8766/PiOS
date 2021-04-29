#ifndef _UTILS_H
#define _UTILS_H
#include "uart.h"

#define NULL   ((void *)0) 

void memzero(unsigned long src, unsigned long n);
void memcpy(unsigned long dst, unsigned long src, unsigned long n);
void *memset(void *s, int c, unsigned long n);
int strcmp(const char *a, const char *b);
int strlen(const char* s);
unsigned long hextoint(char* addr, const int size);
void swap(void **a, void **b) ;
void delay (unsigned long);
void set_pgd(unsigned long pgd);
unsigned long get_pgd();




#endif /*_UTILS_H */
