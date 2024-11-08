#include <stdint.h>

/*
    @brief Checks if a key is available
    @return 1 if a key is available, 0 otherwise
*/
char isKeyAvailable();

/*
    @brief Handles the keyboard
*/
void keyboardHandler();

/*
    @brief Removes a character from the buffer
*/
void removeCharFromBuffer();

/*
    @brief Gets a character from the buffer
*/
char getKeyFromBuffer();

/*
    @brief Gets the buffer
    @param dest: The destination buffer
    @param size: The size of the buffer
    @return The number of characters read
*/
unsigned int getBuffer(char *dest, unsigned int size);

/*
    @brief Cleans the buffer
*/
void cleanBuffer();

/*
    @brief Prints the buffer
*/
void printBuffer();

/*
    @brief Gets the last character from the buffer
*/
char getLastChar();

/*
    @brief Cleans the read
*/
void cleanRead();