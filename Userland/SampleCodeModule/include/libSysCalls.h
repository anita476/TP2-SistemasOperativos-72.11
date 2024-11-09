#include <defs.h>
#include <stdint.h>
#include <unistd.h>

/*
    @brief Reads the buffer
    @param buffer The buffer
    @param length The length
*/
uint64_t sys_read(int fileDes, char *buffer, uint64_t length);

/*
    @brief Gets a character from the buffer
*/
char sys_get_char();

/*
    @brief Puts a character to the buffer
    @param c The character
*/
void sys_put_char(char c);

/*
    @brief Prints a buffer
    @param buffer The buffer
*/
uint64_t fprintf(fd fileDes, char *buffer);

/*
    @brief Gets the time
    @returns The time
*/
uint64_t sys_get_time();

/*
    @brief Clears the screen
*/
void sys_clear_screen();

/*
    @brief Clears a line
    @param line The line
*/
void sys_clear_line(uint64_t line);

/*
    @brief Draws a rectangle
    @param color The color
    @param posX The x coordinate
    @param posY The y coordinate
    @param width The width
    @param height The height
*/
void sys_draw_rectangle(uint64_t color, uint64_t posX, uint64_t posY, uint64_t width, uint64_t height);

/*
    @brief Scales the font up
*/
void sys_scale_up();

/*
    @brief Scales the fontdown
*/
void sys_scale_down();

/*
    @brief Makes a sound
    @param note The note
    @param duration The duration
    @param wait The wait
*/
void sys_make_sound(int note, int duration, int wait);

/*
    @brief Gets the max height
    @returns The max height
*/
uint64_t sys_get_max_height();

/*
    @brief Gets the max width
    @returns The max width
*/
uint64_t sys_get_max_width();

/*
    @brief Gets the pixel color
    @param x The x coordinate
    @param y The y coordinate
    @returns The pixel color
*/
uint64_t sys_get_pixel_color(uint64_t x, uint64_t y);

/*
    @brief Sets the cursor to a line
    @param line The line
*/
void sys_set_cursor_to_line(uint64_t line);

/*
    @brief Gets the registers
    @param buffer The buffer
    @returns The registers
*/
char sys_get_registers(uint64_t *buffer);

/*
    @brief Sets the cursor
    @param posx The x coordinate
    @param line The line
*/
void sys_set_cursor(uint16_t posx, uint16_t line);

/*
    @brief Allocates memory
    @param bytes The bytes to allocate
    @returns The pointer to the allocated memory
*/
void *sys_malloc(size_t bytes);

/*
    @brief Frees memory
    @param ptr The pointer
*/
void sys_free(void *ptr);

/*
    @brief Creates a process
    @param info The process info
    @returns The pid of the created process
*/
pid sys_create_process(createProcessInfo *info);

/*
    @brief Gets the pid
    @returns The pid
*/
pid sys_get_pid();

/*
    @brief Kills a process
    @param pid The pid of the process
    @returns 0 if successful, -1 otherwise
*/
int sys_kill(pid pid);

/*
    @brief Blocks a process
    @param pid The pid of the process
    @returns 0 if successful, -1 otherwise
*/
int sys_block(pid pid);

/*
    @brief Unblocks a process
    @param pid The pid of the process
    @returns 0 if successful, -1 otherwise
*/
int sys_unblock(pid pid);

/*
    @brief Makes the current process yield
*/
void sys_yield();

/*
    @brief Sets the priority of a process
    @param pid The pid of the process
    @param priority The priority
    @returns 0 if successful, -1 otherwise
*/
int sys_set_priority(pid pid, priority priority);

/*
    @brief Lists the processes info
    @param processes The processes
    @param max_proc The max processes
    @returns The number of processes listed
*/
int sys_list_processes_info(ProcessInfo *processes, int max_proc);

/*
    @brief Gets the process info
    @param pid The pid of the process
    @param processInfo The process info to fill
    @returns 0 if successful, -1 otherwise
*/
int sys_get_process_info(pid pid, ProcessInfo *processInfo);

/*
    @brief Checks if a process is foreground
    @param pid The pid of the process
    @returns 1 if in foreground, 0 if not and -1 if error
*/
int sys_is_foreground(pid pid);

/*
    @brief Exits the process
*/
void sys_exit_process();

/*
    @brief Waits for a certain amount of time
    @param ms The time to wait
*/
void sys_wait(uint64_t ms);

/*
    @brief Changes the priority of a process
    @param pid The pid of the process
    @param newPrio The new priority
    @returns 0 if successful, -1 otherwise
*/
int sys_nice(pid pid, priority newPrio);

/*
    @brief Waits for the children
*/
void sys_wait_for_children();

/*
    @brief Opens a semaphore
    @param semName The name of the semaphore
    @param initValue The initial value
    @returns The semaphore
*/
sem sys_sem_open(sem_name semName, int initValue);

/*
    @brief Closes a semaphore
    @param sem The semaphore
    @returns 0 if successful, -1 otherwise
*/
int sys_sem_close(sem sem);

/*
    @brief Posts a semaphore
    @param sem The semaphore
    @returns 0 if successful, -1 otherwise
*/
int sys_sem_post(sem sem);

/*
    @brief Waits for a semaphore
    @param sem The semaphore
    @returns 0 if successful, -1 otherwise
*/
int sys_sem_wait(sem sem);

/*
    @brief Prints memory manager info
*/
void sys_memory_manager_state();

/*
    @brief Opens a pipe
    @param pipe_id The id of the pipe
    @returns pipe_id if successful, -1 otherwise
*/
int sys_open_pipe(unsigned int pipe_id);

/*
    @brief Closes a pipe
    @param pipe_id The id of the pipe
    @returns 0 if successful, -1 otherwise
*/
int sys_close_pipe(unsigned int pipe_id);

/*
    @brief Reads from a pipe
    @param pipe_id The id of the pipe
    @param dest The destination
    @param bytes The number of bytes to read
    @returns bytes read if successful, -1 otherwise
*/
int sys_read_from_pipe(unsigned int pipe_id, char *dest, unsigned int bytes);

/*
    @brief Writes to a pipe
    @param pipe_id The id of the pipe
    @param src The source
    @param bytes The number of bytes to write
    @returns bytes written if successful, -1 otherwise
*/
int sys_write_to_pipe(unsigned int pipe_id, char *src, unsigned int bytes);

/*
    @brief Gets the pipe info
    @param pipe_id The id of the pipe
    @param info The info to fill
    @returns 0 if successful, -1 otherwise
*/
int sys_get_pipe_info(unsigned int pipe_id, pipeInfo *info);

/*
    @brief Waits for a process
    @param pid The pid of the process
*/
void sys_wait_for_pid(pid pid);