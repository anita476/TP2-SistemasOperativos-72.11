#ifndef _TIME_H_
#define _TIME_H_
#include <stdint.h>

/*
    @brief Handles the timer
*/
void timer_handler();

/*
    @brief Gets the elapsed ticks
    @returns The elapsed ticks
*/
int ticks_elapsed();

/*
    @brief Gets the elapsed seconds
    @returns The elapsed seconds
*/
int seconds_elapsed();

/*
    @brief Gets the elapsed milliseconds
    @returns The elapsed milliseconds
*/
int millis_elapsed();

/*
    @brief Waits for the specified time in milliseconds
    @param delta The time in milliseconds
*/
void timer_wait(int delta);

#endif
