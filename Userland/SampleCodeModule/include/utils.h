#ifndef UTILS_H
#define UTILS_H

/*
    @brief Converts an integer to a string
    @param dest The destination
    @param format The format
    @param number The number
*/
void sprintf(char *dest, const char *format, int number);

/*
    @brief Converts an integer to a string
    @param num The number
    @param str The destination
    @param base The base
    @returns The string
*/
char *itoa(int num, char *str, int base);

#endif