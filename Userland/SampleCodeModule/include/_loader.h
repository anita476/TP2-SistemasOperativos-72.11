#include <stdint.h>
#include <sys/types.h>

/*
    @brief Sets a memory block to a specific value
    @param destiny The destination
    @param c The value
    @param length The length
*/
void * memset(void * destiny, int32_t c, uint64_t length);

/*
    @brief Gets the length of a string
    @param str The string
*/
size_t strlen(const char *str);

/*
    @brief Copies a string
    @param dst The destination
    @param src The source
*/
char * strcpy(char *dst, const char *src);

/*
    @brief Compares two strings
    @param s1 The first string
    @param s2 The second string
*/
int strcmp(const char* s1, const char* s2);

/*
    @brief Concatenates two strings
    @param destination The destination
    @param source The source
*/
char * strcat(char *destination, const char *source);



int strncmp(const char * s1, const char * s2, int n);