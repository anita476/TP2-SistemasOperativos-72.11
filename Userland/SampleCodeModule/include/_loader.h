#include <stdint.h>
#include <sys/types.h>

/*
    @brief Sets a memory block to a specific value
    @param destiny The destination
    @param c The value
    @param length The length
    @return The destination
*/
void *memset(void *destiny, int32_t c, uint64_t length);

/*
    @brief Gets the length of a string
    @param str The string
    @return The length
*/
size_t strlen(const char *str);

/*
    @brief Copies a string
    @param dst The destination
    @param src The source
    @return The destination
*/
char *strcpy(char *dst, const char *src);

/*
    @brief Compares two strings
    @param s1 The first string
    @param s2 The second string
    @return 0 if the strings are equal, -1 if s1 is less than s2, 1 if s1 is greater than s2
*/
int strcmp(const char *s1, const char *s2);

/*
    @brief Concatenates two strings
    @param destination The destination
    @param source The source
    @return The destination
*/
char *strcat(char *destination, const char *source);

/*
    @brief Compares two strings up to a certain length
    @param s1 The first string
    @param s2 The second string
    @param n The length
    @return 0 if the strings are equal, -1 if s1 is less than s2, 1 if s1 is greater than s2
*/
int strncmp(const char *s1, const char *s2, int n);

/*
    @brief Copies a memory block
    @param destination The destination
    @param source The source
    @param length The length
    @return The destination
*/
void *memcpy(void *destination, const void *source, uint64_t length);