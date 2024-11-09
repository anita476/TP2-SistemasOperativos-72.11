#include "libSysCalls.h"

#define BUFFER_SIZE 1024

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
    @brief Prints the processes and their information
*/
int ps();

/*
    @brief Starts an endless loop
*/
void loop();

/*
    @brief Prints the contents of a file or echoes the input
*/
void cat();

/*
    @brief Scales down the screen
*/
void scale_down_command();

/*
    @brief Scales up the screen
*/
void scale_up_command();

/*
    @brief Filters the input
*/
int filter();

/*
    @brief Counts the number of lines
*/
int wc();
