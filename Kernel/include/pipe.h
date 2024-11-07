#ifndef PIPE_H
#define PIPE_H

#include <stdint.h>
#include <stdlib.h>
#include <semaphores.h>

#define INVALID_PIPE -1
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
	@brief Opens a pipe, or creates it if the given id is not in use
	@param pipe_id The pipe in question
	@returns 0 if succesful, -1 if error has occurred
*/
int pipe(unsigned int pipe_id); /* opens or creates pipe*/

int close_pipe(unsigned int pipe_id);

void pipe_close(unsigned int pipe_id); //not necessary methinks
/*
	@brief Reads from a certain pipe if available to read, otherwise blocks process. Reads until count is up 
		or pipe sends EOF.
	@param pipe_id The pipe in question
	@param dest Buffer where to leave the information read.
	@param count Number of bytes to read
	@returns Count if succesful read, -1 if error has occurred
 */
int read_from_pipe(unsigned int pipe_id, char * dest, unsigned int count);

/*
	@brief Writes to a certain pipe if available to write, otherwise blocks process. Writes until count is up, or
			source sends EOF.
	@param pipe_id The pipe in question
	@param src Buffer to write the information to
	@param count Number of bytes to write to pipe
	@returns Count if succesful write, -1 if error has occurred
 */
int write_to_pipe(unsigned int pipe_id, const char * src, unsigned int count);

#endif
