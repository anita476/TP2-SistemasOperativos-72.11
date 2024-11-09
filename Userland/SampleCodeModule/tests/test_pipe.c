
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <test_pipe.h>

static char *string = "abcdefghijklmnopqrstwxyzabcdefghijklmnopqrstuvwyz";
#define STRING_LENGTH 50
#define AMOUNT        1
int pipe_id;

void read_pipe() {

  fprintf(STDOUT, "Starting to read from pipe\n");
  int id = open_pipe(pipe_id);
  if (id != pipe_id) {
    fprintf(STDERR, "Error opening pipe from read end\n");
  }
  char buffer[20] = {0};

  for (int i = 0; i < STRING_LENGTH;) {
    read_from_pipe(pipe_id, buffer, AMOUNT);

    if (strncmp(buffer, string + i, AMOUNT)) {
      fprintf(STDERR, "Error in pipe: text doesnt match source\n");
    }
    i += AMOUNT;
    fprintf(STDOUT, buffer);
  }
  fprintf(STDOUT, "\n");

  close_pipe(pipe_id);
  fprintf(STDOUT, "\nFinished reading from pipe\n");
}

void write_pipe() {
  fprintf(STDOUT, "Starting to write to pipe\n");
  int id = open_pipe(pipe_id);
  if (id != pipe_id) {
    fprintf(STDERR, "Error opening pipe from write end\n");
  }

  for (int i = 0; i < STRING_LENGTH;) {
    write_to_pipe(pipe_id, string + i, AMOUNT);
    i += AMOUNT;
    bussy_wait(5000000);
  }

  close_pipe(pipe_id);
  fprintf(STDOUT, "\nFinished writing to pipe\n");
}

void test_pipe() {

  int id = open_pipe(0);  // open a new pipe

  if (id < 0) {
    fprintf(STDERR, "Error creating pipe in test!\n");
    return;
  } else {
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
  createProcessInfo writeProcessInfo = {.name = "write",
                                        .fg_flag = 1,
                                        .priority = DEFAULT_PRIORITY,
                                        .start = (ProcessStart) &write_pipe,
                                        .argc = 0,
                                        .argv = (const char *const *) NULL,
                                        .input = STDIN,
                                        .output = STDOUT};

  create_process(&writeProcessInfo);
  create_process(&readProcessInfo);
  wait_for_children();
  fprintf(STDOUT, "Finished pipe test!\n");
  close_pipe(pipe_id);
}