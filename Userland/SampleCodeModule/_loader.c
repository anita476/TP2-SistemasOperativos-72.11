// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/* _loader.c */
#include <_loader.h>
#include <libSysCalls.h>
#include <defs.h>

extern char bss;
extern char endOfBinary;

int main();

void * memset(void * destiny, int32_t c, uint64_t length);

int _start() {
	//Clean BSS
	memset(&bss, 0, &endOfBinary - &bss);
	return main();
}

void * memset(void * destiation, int32_t c, uint64_t length) {
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destiation;
	while (length--) dst[length] = chr;
	return destiation;
}

size_t strlen(const char * str) {
    size_t l;
    for (l = 0; *str != 0; str++, l++);
    return l;
}

char * strcpy(char * dst, const char * src) {
    char * w;
    for (w = dst; * src != '\0'; * (w++) = * (src++));
    * w = '\0';
    return dst;
}

int strcmp(const char * s1, const char * s2){
    while(* s1 && (* s1 == * s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char * strcat(char * dst, const char * src) {
    char * rdest = dst;
    while (* dst) dst++;
    while ((* dst++ = * src++));
    return rdest;
}

int isForeground(pid pid) {
    ProcessInfo info;
    if (getProcessInfo(pid, &info) != 0) return -1;
    return info.fg_flag;
}