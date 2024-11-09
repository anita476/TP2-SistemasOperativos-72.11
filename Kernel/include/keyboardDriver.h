#include <lib.h>
#include <processes.h>
#include <scheduler.h>
#include <stdint.h>
#include <videoDriver.h>

/*
    @brief Checks if a key is available
    @return 1 if a key is available, 0 otherwise
*/
char is_key_available();

/*
    @brief Handles the keyboard
*/
void keyboard_handler();

/*
    @brief Removes a character from the buffer
*/
void remove_char_from_buffer();

/*
    @brief Gets a character from the buffer
    @returns The character
*/
char get_key_from_buffer();

/*
    @brief Gets the buffer
    @param dest: The destination buffer
    @param size: The size of the buffer
    @returns The number of characters read
*/
unsigned int get_buffer(char *dest, unsigned int size);

/*
    @brief Cleans the buffer
*/
void clean_buffer();

/*
    @brief Prints the buffer
*/
void print_buffer();

/*
    @brief Gets the last character from the buffer
*/
char get_last_char();

/*
    @brief Cleans the read
*/
void clean_read();

/*
    @brief Adds a pid to the blocking queue
    @param pid The pid to add
*/
void add_to_blocking_queue_read(pid pid);

/*
    @brief Removes a pid from the blocking queue
*/
void remove_from_blocking_queue();