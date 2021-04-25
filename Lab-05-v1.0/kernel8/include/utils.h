#ifndef _UTILS_H
#define _UTILS_H
#include "uart.h"

#define NULL   ((void *)0) 

void *memset(void *s, int c, unsigned long n);
int strcmp(const char *a, const char *b);
int strlen(const char* s);
unsigned long hextoint(char* addr, const int size);
void swap(void **a, void **b) ;
extern void _load_user_program(void*, void*);
extern int get_el(void);
void print_el();
void delay ( unsigned long);
void put32 (void *src, unsigned long int );
unsigned int get32 (void *src);
void memzero(void *src, unsigned long n);
void memcpy(void *str1, const void *str2, unsigned long n);





#endif /*_UTILS_H */
