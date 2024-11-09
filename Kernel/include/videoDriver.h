#include "lib.h"
#include <pipe.h>
#include <stdint.h>

/*
    @brief Gets the width of the screen in pixels
    @returns The width in pixels
*/
uint16_t get_width_pixels();

/*
    @brief Gets the height of the screen in pixels
    @returns The height in pixels
*/
uint16_t get_height_pixels();

/*
    @brief Gets the width of the screen in characters
    @returns The width in characters
*/
uint16_t get_width_chars();

/*
    @brief Gets the height of the screen in characters
    @returns The height in characters
*/
uint16_t get_height_chars();

/*
    @brief Scales the screen up
    @returns 0 if successful, 1 if error
*/
int scale_up();

/*
    @brief Scales the screen down
    @returns 0 if successful, 1 if error
*/
int scale_down();

/*
    @brief Gets the scale of the screen
    
*/
uint8_t get_scale();

/*
    @brief Puts a pixel on the screen
    @param hexColor The color of the pixel
    @param x The x coordinate
    @param y The y coordinate
    @returns 0 if successful, 1 if error
*/
int put_pixel(uint64_t hexColor, uint64_t x, uint64_t y);

/*
    @brief Puts a character on the screen
    @param c The character
    @param x The x coordinate
    @param y The y coordinate
    @returns 0 if successful, 1 if error
*/
int put_char(char c, uint64_t x, uint64_t y);

/*
    @brief Puts a character on the screen at the cursor
    @param c The character
    @returns 0 if successful, 1 if error
*/
int put_char_cursor(char c);

/*
    @brief Prints a string on the screen
    @param fileDes File descriptor
    @param str The string
*/
void print(fd fileDes, char *str);

/*
    @brief Prints a string on the screen and adds a new line
    @param str The string
*/
void println_char(char *str);

/*
    @brief Adds a new line to the screen
*/
void new_line();

/*
    @brief Clears the screen
*/
void clear_screen();

/*
    @brief Sets the cursor to the specified line
    @param line The line to set the cursor to
    @returns 0 if successful, 1 if error
*/
int set_cursor_line(unsigned int line);

/*
    @brief Sets the cursor to the specified coordinates
    @param x The x coordinate
    @param y The y coordinate
*/
void set_cursor(uint16_t x, uint16_t y);

/*
    @brief Sets the color of the screen
    @param newColor The new color
*/
void set_color(uint32_t newColor);

/*
    @brief Gets the color of the pixel at the specified coordinates
    @param x The x coordinate
    @param y The y coordinate
    @returns The color of the pixel
*/
uint32_t get_pixel_color(uint64_t x, uint64_t y);

/*
    @brief Draws a rectangle on the screen
    @param hexColor The color of the rectangle
    @param x The x coordinate
    @param y The y coordinate
    @param width The width of the rectangle
    @param height The height of the rectangle
    @returns 0 if successful, 1 if error
*/
int draw_rectangle(uint32_t hexColor, uint64_t x, uint64_t y, int width, int height);

/*
    @brief Converts a line to a height
    @param line The line
    @returns The height
*/
uint16_t line_to_height(unsigned int line);

/*
    @brief Converts a height to a line
    @param height The height
    @returns The line
*/
int height_to_line(uint16_t height);