#include <stdint.h>
#include "lib.h"
#include <pipe.h>

/*
    @brief Gets the width of the screen in pixels
*/
uint16_t getWidthPixels();

/*
    @brief Gets the height of the screen in pixels
*/
uint16_t getHeightPixels();

/*
    @brief Gets the width of the screen in characters
*/
uint16_t getWidthChars();

/*
    @brief Gets the height of the screen in characters
*/
uint16_t getHeightChars();

/* 
    @brief Scales the screen up
*/
int scaleUp();

/*
    @brief Scales the screen down
*/
int scaleDown();

/*
    @brief Gets the scale of the screen
*/
uint8_t getScale();


/*
    @brief Puts a pixel on the screen
    @param hexColor The color of the pixel
    @param x The x coordinate
    @param y The y coordinate
*/
int putPixel(uint32_t hexColor, uint64_t x, uint64_t y);

/*
    @brief Puts a character on the screen
    @param c The character
    @param x The x coordinate
    @param y The y coordinate
*/
int putChar(char c, uint64_t x, uint64_t y);

/*
    @brief Puts a character on the screen at the cursor
    @param c The character
*/
int putCharCursor(char c);

/*
    @brief Prints a string on the screen
    @param fileDes File descriptor
    @param str The string
*/
void print(fd fileDes, char * str);

/*
    @brief Prints a string on the screen and adds a new line
    @param str The string
*/
void printlnchar(char * str);

/*
    @brief Adds a new line to the screen
*/
void newLine();

/*
    @brief Clears the screen
*/
void clearScreen();

/*
    @brief Sets the cursor to the specified line
    @param line The line
*/
int setCursorLine(unsigned int line);

/*
    @brief Sets the cursor to the specified coordinates
    @param x The x coordinate
    @param y The y coordinate
*/
void setCursor(uint16_t x, uint16_t y);

/*
    @brief Sets the color of the screen
    @param newColor The new color
*/
void setColor(uint32_t newColor);

/*
    @brief Gets the color of the pixel at the specified coordinates
    @param x The x coordinate
    @param y The y coordinate
*/
uint32_t getPixelColor(uint64_t x, uint64_t y);

/*
    @brief Draws a rectangle on the screen
    @param hexColor The color of the rectangle
    @param x The x coordinate
    @param y The y coordinate
    @param width The width of the rectangle
    @param height The height of the rectangle
*/
int drawRectangle(uint32_t hexColor, uint64_t x, uint64_t y, int width, int height);

/*
    @brief Converts a line to a height
    @param line The line
*/
uint16_t lineToHeight(unsigned int line);

/*
    @brief Converts a height to a line
    @param height The height
*/
int heightToLine(uint16_t height);