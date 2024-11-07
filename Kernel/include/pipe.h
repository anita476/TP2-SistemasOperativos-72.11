#ifndef PIPE_H
#define PIPE_H

#include <stdint.h>
#include <stdlib.h>
#include <semaphores.h>

#define INVALID_PIPE -1
#define NO_SPACE -2
#define PIPE_ERROR -3 /* For when theres a read or write error, but i havent reached EOF ! */
#define EOF -1
#define MAX_PIPES 20
#define PIPE_SIZE 1024

typedef struct pipeInfo {
    unsigned int id;
	unsigned int amount;
	uint8_t eof;
	//smth else
}pipeInfo;

/*
	@brief Retrieves information from a pipe
	@param pipe_id The pipe in question
	@param info The information struct in which to pour the information
	@return 0 if successful, -1 if error has occurred
*/
int get_pipe_info(unsigned int pipe_id, pipeInfo * info);

/* 
	@brief Send an EOF signal to a pipe. If pipe does not exist, does nothing
	@param pipe_id The pipe in question
*/
void signal_eof(unsigned int pipe_id); 

/*
	@brief Opens a pipe, or creates one if NULL is passed as parameter
	@param pipe_id The pipe in question
	@returns pipe_id if succesful, -1 if error has occurred
	@warning Does not create pipe if pipe_id is not null
*/
int pipe(unsigned int pipe_id);

/*
	@brief Closes a pipe
	@param pipe_id The pipe in question
	@returns 0 if succesful, -1 if error has occurred
*/
int close_pipe(unsigned int pipe_id);

void pipe_close(unsigned int pipe_id); //not necessary methinks
/*
	@brief Reads from a certain pipe if available to read, otherwise blocks process. Reads until bytes is up 
		or pipe sends EOF.
	@param pipe_id The pipe in question
	@param dest Buffer where to leave the information read.
	@param bytes Number of bytes to read
	@returns bytes if succesful read, -1 if error has occurred
 */
int read_from_pipe(unsigned int pipe_id, char * dest, unsigned int bytes);

/*
	@brief Writes to a certain pipe if available to write, otherwise blocks process. Writes until bytes is up, or
			source sends EOF.
	@param pipe_id The pipe in question
	@param src Buffer to write the information to
	@param bytes Number of bytes to write to pipe
	@returns bytes if succesful write, -1 if error has occurred
 */
int write_to_pipe(unsigned int pipe_id, const char * src, unsigned int bytes);

#endif
