#ifndef __STR_H__
#define __STR_H__

#include <stdint.h>
#include <stddef.h>

void itoa(long int n, char* str);
void htoa(long int n, char* str);
void utoa(long unsigned int n, char* str);

void reverse(char* s);
int strlen(char* s);
char* strcat(char* dest, char* src);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);
int strcmp(const char* str1, const char* str2);
int strncmp(const char* str1, const char* str2, size_t n);
char* strtok(char* str, const char* delimiter);
char* strchr(const char* str, int c);

void append(char* s, char c);

#endif
