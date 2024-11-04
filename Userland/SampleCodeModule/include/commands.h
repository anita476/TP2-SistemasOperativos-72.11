#include "libSysCalls.h"

/*
    @brief Prints all the commands available
*/
void help();

/*
    @brief Prints the time
*/
void time();

/*
    @brief Prints the registers
*/
void regs();

/*
    @brief Converts an integer to a string
    @param num The integer
    @param str The result buffer
    @param base The base
*/
char* itoa(int num, char* str, int base);

/*
    @brief Prints the processes and their information
*/
int ps();

/*
    @brief Starts an endless loop
*/
void loop();