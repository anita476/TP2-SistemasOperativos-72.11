#include <pipe.h>

#define START_ID 4 //not the cleanest but its to simplify the code since a pipe is "another file"

typedef struct Pipe{
	unsigned int pipeID; /* has to be bigger than 3*/

	sem read_sem;
	sem write_sem;
	sem_name read_sem_name;
	sem_name write_sem_name;

	unsigned int write_pos;
	unsigned int read_pos;
	uint8_t * pipe;
	unsigned int amount;

	uint8_t eof;
}Pipe;

static Pipe pipeList[MAX_PIPES]; 


int find_pipe(unsigned int id){
	if(id <= 3 || (pipeList[id + START_ID].pipeID != id)){
		return INVALID_PIPE;
	}
	return (id + START_ID);
}

int get_pipe_info(unsigned int pipe_id, pipeInfo * info){

}


void signal_eof(unsigned int pipe_id){
	;
}

int pipe(unsigned int pipe_id){
	int pos = find_pipe(pipe_id);
	if(pos < 0 && pipe_id > 3){ // create sem since its not used currently

	}
	else{ // open new sems for process
		sem_open()

	}
	return 0;
}



int read_from_pipe(unsigned int pipe_id, char * dest, unsigned int count){
	return 0;
}

int write_to_pipe(unsigned int pipe_id, const char * src, unsigned int count){
	int pos = find_pipe(pipe_id);
	if(pos == INVALID_PIPE)
		return INVALID_PIPE_ID;
	
	for(int i=0; i<count; i++){
		wait_sem(pipe_info[pos].write_sem_id);

		pipe_info[pos].pipe[pipe_info[pos].write_pos] = src[i];
		INCREASE_MOD(pipe_info[pos].write_pos, PIPE_SIZE);
		pipe_info[pos].amount++;

		signal_sem(pipe_info[pos].read_sem_id);
	}
	return count;
}
int close_pipe(){

}

/*
pipe idea so i dont forget:
-> when process wants to use a pipe -> 
-> when process wants to print to stdout or stderr && isPiped -> doesnt know it but its input fileDes is different -> write handles it (calls write to pipe if necessary)
-> read does the same thing 

-> when using pipes read and write is blocking !


*/