#ifndef _STRING_H
#define _STRING_H

int strcmp(char *, char *);
void strset (char * s1, int c, int size );
int readline(char *, int);
void *memset(void *s, int c, unsigned long n);
void *memcpy(void *dest, const void *src, unsigned long n);
int memcmp(void *s1, void *s2, int n);
int atoi(char* str);
void itohex_str ( unsigned long long d, int size, char * s );
unsigned int  hextoint(char *s);
int oct2bin(char *s, int n);

#endif /*_STRING_H */
