#include "utils.h"



void *memset(void *s, int c, unsigned long n){
    char *xs = s;
    while (n--)
        *xs++ = c;
    return s;
}


int strcmp(const char *a, const char *b){
    if(!(*a | *b)) return 0;
    return (*a != *b)? *a - *b : strcmp(++a, ++b);
}
unsigned int strncmp(const char *X, const char *Y, unsigned int n){
    while(n--){
        if (*X != *Y){
            return 1;
        }
        X++;
        Y++;
    } 
    return 0;
}


int strlen(const char* s){
    int len = 0;
    while(s[len] != '\0'){
        len++;
    }
    return len;
}

unsigned long hextoint(char* addr, const int size){
    unsigned long res = 0;
    char c;
    for(int i = 0 ;i < size; ++i){
        res <<= 4;
        c = *(addr + i);
        if(c >= '0' && c <= '9') res += c - '0';
        else if(c >= 'A' && c <= 'F') res += c - 'A' + 10;
        else if(c >= 'a' && c <= 'f') res += c - 'a' + 10;
    }
    return res;
}


void swap(void **a, void **b) {
  void *tmp = *a;
  *a = *b;
  *b = tmp;
}

void strrev(char *head){
    if(!head){
        return;
    }

    char *tail = head;
    while(*tail){
        tail++;
    };
    tail--;

    while(head < tail){
        char h = *head;
        *head++ = *tail;
        *tail-- = h;
    }
}

unsigned int uitoa(unsigned int num, char *buffer, unsigned int buffer_size, char base){
    if(buffer_size < 2 || base < 2 || base > 16){
        return 1;
    }

    unsigned int i = 0;
    if(!num){
        buffer[i++] = '0';
    }

    while(num){
        if(i == buffer_size-1){
            buffer[i] = '\0';
            return 1;
        }
        unsigned int rem = num % base;
        buffer[i++] = (rem > 9)? (rem - 10) + 'a' : rem + '0';
        num /= base;
    }

    if(base == 16){
        buffer[i++] = 'x';
        buffer[i++] = '0';
    }

    if(base == 2){
        buffer[i++] = 'b';
        buffer[i++] = '0';
    }

    buffer[i] = '\0';
    strrev(buffer);
    return 0;
}

void strncpy(char *to, const char *from, long int n){
    for(long int i=0; i<n; i++){
        to[i] = from[i];
        if((n < 0) && (from[i] == '\0')){
            break;
        }
    }
}

