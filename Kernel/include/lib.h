#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <sys/types.h>



/* Defines for file descriptors */
#define STDIN  0
#define STDOUT 1
#define STDERR 2
#define KBDIN  3

#define EOF_CHAR 4

typedef int fd;

/*
    @brief Calculates the length of a string
    @param str The string
    @returns The length of the string
*/
size_t strlen(const char *str);

/*
    @brief Copies a string
    @param dst The destination
    @param src The source
    @returns The destination
*/
char * strcpy(char *dst, const char *src);

/*
    @brief Compares two strings
    @param s1 The first string
    @param s2 The second string
    @returns 0 if the strings are equal, -1 if the first string is less than the second, 1 if the first string is greater than the second
*/
int strcmp(const char* s1, const char* s2);

/*
    @brief Sets a memory block to a specific value
    @param destination The destination
    @param character The character
    @param length The length
*/
void * memset(void * destination, int32_t character, uint64_t length);

/*
    @brief Copies a memory block
    @param destination The destination
    @param source The source
    @param length The length
*/
void * memcpy(void * destination, const void * source, uint64_t length);

/*
    @brief Gets the CPU vendor
    @param result The result buffer
    @returns The CPU vendor
*/
char *cpuVendor(char *result);

/*
    @brief Converts an integer to a string
    @param num The integer
    @param str The result buffer
    @param base The base
*/
void intToStr(int num, char* str, int base);


char * strcat(char *dest, const char *src);

#endif