#include <stdint.h>
#include <sys/types.h>

void * memset(void * destiny, int32_t c, uint64_t length);
size_t strlen(const char *str);
char * strcpy(char *dst, const char *src);
int strcmp(const char* s1, const char* s2);
char * strcat(char *destination, const char *source);