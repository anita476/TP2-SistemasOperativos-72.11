#include <stdint.h>
#include <unistd.h>
#include <defs.h>

/*
    @brief Reads the buffer
    @param buffer The buffer
    @param length The length
*/
uint64_t readBuffer(int fileDes, char* buffer, uint64_t length);

/*
    @brief Gets a character from the buffer
*/
char getChar();

/*
    @brief Puts a character to the buffer
    @param c The character
*/
void putChar(char c);    

/*
    @brief Prints a buffer
    @param buffer The buffer
*/
uint64_t fprintf(fd fileDes, char* buffer);                       

/*
    @brief Gets the time
    @returns The time
*/
uint64_t getTime();

/*
    @brief Clears the screen
*/
void clearScreen();

/*
    @brief Clears a line
    @param line The line
*/
void clearLine(uint64_t line);

/*
    @brief Draws a rectangle
    @param color The color
    @param posX The x coordinate
    @param posY The y coordinate
    @param width The width
    @param height The height
*/
void drawRectangle(uint64_t color, uint64_t posX, uint64_t posY, uint64_t width, uint64_t height);

/* 
    @brief Scales the font up
*/
void scaleUp();

/*
    @brief Scales the fontdown
*/
void scaleDown();

/*
    @brief Makes a sound
    @param note The note
    @param duration The duration
    @param wait The wait
*/
void make_sound(int note, int duration, int wait);

/*
    @brief Gets the max height
    @returns The max height
*/
uint64_t getMaxHeight();

/*
    @brief Gets the max width
    @returns The max width
*/
uint64_t getMaxWidth();

/*
    @brief Gets the pixel color
    @param x The x coordinate
    @param y The y coordinate
    @returns The pixel color
*/
uint64_t getPixelColor(uint64_t x, uint64_t y);

/*
    @brief Sets the cursor to a line
    @param line The line
*/
void setCursorToLine(uint64_t line);

/*
    @brief Gets the registers
    @param buffer The buffer
    @returns The registers
*/
char getRegisters(uint64_t * buffer);

/*
    @brief Sets the cursor
    @param posx The x coordinate
    @param line The line
*/
void setCursor(uint64_t posx, uint64_t line);

/*
    @brief Allocates memory
    @param bytes The bytes to allocate
    @returns The pointer to the allocated memory
*/
void * malloc(size_t bytes);

/*
    @brief Frees memory
    @param ptr The pointer
*/
void free(void * ptr);

/*
    @brief Creates a process
    @param info The process info
    @returns The pid of the created process
*/
pid createProcess(createProcessInfo * info);

/*
    @brief Gets the pid
    @returns The pid
*/
pid getpid();

/*
    @brief Kills a process
    @param pid The pid of the process
    @returns 0 if successful, -1 otherwise
*/
int kill(pid pid);

/*
    @brief Blocks a process
    @param pid The pid of the process
    @returns 0 if successful, -1 otherwise
*/
int block(pid pid);

/*
    @brief Unblocks a process
    @param pid The pid of the process
    @returns 0 if successful, -1 otherwise
*/
int unblock(pid pid);

/*
    @brief Makes the current process yield
*/
void yield();

/*
    @brief Sets the priority of a process
    @param pid The pid of the process
    @param priority The priority
    @returns 0 if successful, -1 otherwise
*/
int setPriority(pid pid, priority priority);

/*
    @brief Lists the processes info
    @param processes The processes
    @param max_proc The max processes
    @returns The number of processes listed
*/
int listProcessesInfo(ProcessInfo * processes, int max_proc);

/*
    @brief Gets the process info
    @param pid The pid of the process
    @param processInfo The process info to fill
    @returns 0 if successful, -1 otherwise
*/
int getProcessInfo(pid pid, ProcessInfo * processInfo);

/*
    @brief Checks if a process is foreground
    @param pid The pid of the process
    @returns 1 if in foreground, 0 otherwise
*/
int isForeground(pid pid);

/*
    @brief Exits the process
*/
void exit_process();

/*
    @brief Waits for a certain amount of time
    @param ms The time to wait
*/
void wait(uint64_t ms);

/*
    @brief Changes the priority of a process
    @param pid The pid of the process
    @param newPrio The new priority
    @returns 0 if successful, -1 otherwise
*/
int nice(pid pid, priority newPrio);

/*
    @brief Waits for the children
*/
void waitForChildren();

/*
    @brief Opens a semaphore
    @param semName The name of the semaphore
    @param initValue The initial value
    @returns The semaphore
*/
sem sem_open(sem_name semName, int initValue);

/*
    @brief Closes a semaphore
    @param sem The semaphore
    @returns 0 if successful, -1 otherwise
*/
int sem_close(sem sem);

/*
    @brief Posts a semaphore
    @param sem The semaphore
    @returns 0 if successful, -1 otherwise
*/
int sem_post(sem sem);

/*
    @brief Waits for a semaphore
    @param sem The semaphore
    @returns 0 if successful, -1 otherwise
*/
int sem_wait(sem sem);

/* @brief Prints memory manager info 
*/

void memory_manager_state(); 


int open_pipe(unsigned int pipe_id);
int close_pipe(unsigned int pipe_id);

int read_from_pipe(unsigned int pipe_id, char * dest, unsigned int bytes);
int write_to_pipe(unsigned int pipe_id, char * src, unsigned int bytes);
int get_pipe_info(unsigned int pipe_id, pipeInfo * info);