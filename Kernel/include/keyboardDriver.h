#include <lib.h>
#include <processes.h>
#include <scheduler.h>
#include <stdint.h>
#include <videoDriver.h>

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
    @returns The character
*/
char getKeyFromBuffer();

/*
    @brief Gets the buffer
    @param dest: The destination buffer
    @param size: The size of the buffer
    @returns The number of characters read
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

/*
    @brief Adds a pid to the blocking queue
    @param pid The pid to add
*/
void addToBlockingQueueRead(pid pid);

/*
    @brief Removes a pid from the blocking queue
*/
void removeFromBlockingQueue();