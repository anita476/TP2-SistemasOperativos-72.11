#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <sys/types.h>

size_t strlen(const char *str);

char * strcpy(char *dst, const char *src);
int strcmp(const char* s1, const char* s2);

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);

char *cpuVendor(char *result);

void intToStr(int num, char* str, int base);

#endif