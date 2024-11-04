#ifndef _SYSCALLHANDLER_H_
#define _SYSCALLHANDLER_H_

#include <stdint.h>
#include <processes.h>

/* 
    @brief Handles the syscalls
    @param rdi The first argument
    @param rsi The second argument
    @param rdx The third argument
    @param r10 The fourth argument
    @param r8 The fifth argument
    @param rax The syscall number
*/
uint64_t syscallHandler(uint64_t rdi, uint64_t rsi , uint64_t rdx , uint64_t r10, uint64_t r8 , uint64_t rax);

/* 
    @brief Reads from a file
    @param fileDescriptor The file descriptor
    @param buffer The buffer
    @param length The length
*/
uint64_t read(uint64_t fileDescriptor , uint64_t buffer , uint64_t length);

/* 
    @brief Writes to a file
    @param fileDescriptor The file descriptor
    @param buffer The buffer
    @param length The length
*/
uint64_t write(uint64_t fileDescriptor, uint64_t buffer , uint64_t length);

/* 
    @brief Gets the current time
*/
uint64_t get_current_time();

/* 
    @brief Gets the elapsed time in milliseconds
*/
uint64_t elapsed_millis();

/* 
    @brief Gets the height of the characters
*/
uint64_t get_height_ch();

/* 
    @brief Gets the width of the characters
*/
uint64_t get_width_ch();

/* 
    @brief Clears a line
    @param line The line
*/
void clear_line(uint64_t line);

/* 
    @brief Clears the screen
*/
void clear_screen();

/* 
    @brief Puts a pixel
    @param color The color
    @param x The x coordinate
    @param y The y coordinate
*/
uint64_t put_pixel(uint64_t color , uint64_t x, uint64_t y);

/* 
    @brief Draws a rectangle
    @param hexColor The color
    @param x The x coordinate
    @param y The y coordinate
    @param width The width
    @param height The height
*/
uint64_t draw_rect(uint64_t hexColor, uint64_t x, uint64_t y, uint64_t width , uint64_t height);

/* 
    @brief Scales the font up
*/
uint64_t scale_up();

/* 
    @brief Scales the font down
*/
uint64_t scale_down();

/* 
    @brief Makes a sound
    @param freq The frequency
    @param duration The duration
    @param wait The wait
*/
uint64_t make_sound(uint64_t freq, uint64_t duration , uint64_t wait);

/*
    @brief Gets the height of the screen in pixels
*/
uint64_t get_height_pix();

/* 
    @brief Gets the width of the screen in pixels
*/
uint64_t get_width_pix();

/* 
    @brief Gets the pixel at the specified coordinates
    @param x The x coordinate
    @param y The y coordinate
*/
uint32_t get_pix(uint64_t x, uint64_t y);

/* 
    @brief Gets the maximum number of lines that fit on the screen
*/
uint64_t get_max_lines();

/* 
    @brief Sets the cursor to the specified line
    @param line The line
*/
uint64_t set_cursor_to_line(uint64_t line);

/* 
    @brief Gets the registers
    @param buffer The buffer
*/
uint64_t get_registers(uint64_t buffer );

/* 
    @brief Waits for the specified time in milliseconds
    @param time_in_millis The time in milliseconds
*/
uint64_t wait(uint64_t time_in_millis);

#endif