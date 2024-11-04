#ifndef _TIME_H_
#define _TIME_H_
#include <stdint.h>

/* 
    @brief Handles the timer
*/
void timerHandler();

/*
    @brief Gets the elapsed ticks
*/
int ticksElapsed();

/*
    @brief Gets the elapsed seconds
*/
int secondsElapsed();

/*
    @brief Gets the elapsed milliseconds
*/
int millisElapsed();

/*
    @brief Waits for the specified time in milliseconds
    @param delta The time in milliseconds
*/
void timer_wait(int delta);

#endif
