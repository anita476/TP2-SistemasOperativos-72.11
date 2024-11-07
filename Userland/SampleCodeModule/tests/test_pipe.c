
#include <test_pipe.h>
/* 
IDEA FOR A PIPE TEST */


static char * string = "This is a super long string and is supposed to test pipes. Write will write parts of the string to the pipe and read will read and check that its correct. All the while, we will be using busy waiting to make this process longer...";
#define STRING_LENGTH 230
#define AMOUNT 10
int pipe_id;

void read_pipe(){

	fprintf(STDOUT, "Starting to read from pipe\n");
	int id = open_pipe(pipe_id);
	if(id != pipe_id){
		fprintf(STDERR,"Error opening pipe from read end\n");
	}
	char buffer[20] = {0};

	for(int i=0; i<STRING_LENGTH; ){
		read_from_pipe(pipe_id, buffer, AMOUNT);

		if(strncmp(buffer, string + i, AMOUNT)){
			fprintf(STDERR, "Error in pipe: text doesnt match source\n");
		}
		i += AMOUNT;
	}

	close_pipe(pipe_id);
	fprintf(STDOUT,"Finished writing to pipe\n");
}

void write_pipe(){
	fprintf(STDOUT, "Starting to write to pipe\n");
	int id = open_pipe(pipe_id);
	if(id != pipe_id){
		fprintf(STDERR,"Error opening pipe from write end\n");
	}

	for(int i=0; i<STRING_LENGTH; ){
		write_to_pipe(pipe_id, string + i, AMOUNT);
		i += AMOUNT;
		bussy_wait(50000000);
	}

	close_pipe(pipe_id);
	fprintf(STDOUT,"Finished writing to pipe\n");
}


void test_pipe(){

	int id = open_pipe(0); // open a new pipe

	if(id < 0){
		fprintf(STDERR, "Error creating pipe in test!\n");
		return;
	}
	else{
		pipe_id = id;
	}

	createProcessInfo readProcessInfo = {.name = "read",
                                  .fg_flag = 1,
                                  .priority = DEFAULT_PRIORITY,
                                  .start = (ProcessStart) &read_pipe,
                                  .argc = 0,
                                  .argv = (const char *const *) NULL,
                                  .input = STDIN,
                                  .output = STDOUT};
    createProcess(&readProcessInfo);

	createProcessInfo writeProcessInfo = {.name = "read",
                                  .fg_flag = 1,
                                  .priority = DEFAULT_PRIORITY,
                                  .start = (ProcessStart) &write_pipe,
                                  .argc = 0,
                                  .argv = (const char *const *) NULL,
                                  .input = STDIN,
                                  .output = STDOUT};
    createProcess(&writeProcessInfo);

	waitForChildren();

	close_pipe(pipe_id);
} 