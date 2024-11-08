// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "shell.h"

#define COMMANDS_SIZE 21

int bg_flag = 0;
static Command commandList[COMMANDS_SIZE] = {
    {.name = "help", .isPipeable = 0, .numberArgs = 0, .start = (ProcessStart)help, .usage = "Usage: help"},
    {.name = "time", .isPipeable = 0, .numberArgs = 0, .start = (ProcessStart)time, .usage = "Usage: time"},
    {.name = "eliminator", .isPipeable = 0, .numberArgs = 0, .start = (ProcessStart)eliminator , .usage = "Usage: eliminator"},
    {.name = "regs", .isPipeable = 0, .numberArgs = 0, .start = (ProcessStart)regs, .usage = "Usage: regs"},
    {.name = "clear", .isPipeable = 0 , .numberArgs = 0, .start = (ProcessStart)clearScreen, .usage = "Usage: clear"},
    {.name = "scaledown", .isPipeable = 0, .numberArgs = 0, .start = (ProcessStart)scaleDownCommand , .usage= "Usage: scaledown"},
    {.name = "scaleup", .isPipeable = 0, .numberArgs = 0, .start = (ProcessStart)scaleUpCommand, .usage = "Usage: scaleup"},
    {.name = "divzero", .isPipeable = 0, .numberArgs = 0, .start = (ProcessStart)divzero, .usage = "Usage: divzero"},
    {.name = "invalidopcode", .isPipeable = 0, .numberArgs = 0, .start = (ProcessStart)invalidOpCode, .usage = "Usage: invalidopcode"},
    {.name = "testmm", .isPipeable = 0, .numberArgs = 1, .start = (ProcessStart)test_mm, .usage = "Usage: testmm [max_mem]"},
    {.name = "testproc", .isPipeable = 0, .numberArgs = 1, .start= (ProcessStart)test_processes , .usage = "Usage: testproc [max_procs]"},
    {.name = "testprio", .isPipeable = 0, .numberArgs = 0, .start = (ProcessStart)test_prio, .usage = "Usage: testprio"},
    {.name = "testsync", .isPipeable = 0, .numberArgs = 2, .start = (ProcessStart)testSync, .usage = "Usage: testsync [n_value] [use_sem]"},
    {.name = "ps", .isPipeable = 0, .numberArgs = 0, .start = (ProcessStart)ps, .usage = "Usage: ps"},
    {.name = "loop", .isPipeable = 0, .numberArgs = 0, .start = (ProcessStart)loop, .usage = "Usage: loop"},
    {.name = "kill", .isPipeable = 0, .numberArgs = 1, .start = (ProcessStart)kill, .usage = "Usage: kill [pid]"},
    {.name = "block", .isPipeable = 0, .numberArgs = 1, .start = (ProcessStart)block, .usage = "Usage: block [pid]"},
    {.name = "unblock", .isPipeable = 0, .numberArgs = 1, .start = (ProcessStart)unblock, .usage = "Usage: unblock [pid]"},
    {.name = "nice", .isPipeable = 0, .numberArgs = 1, .start = (ProcessStart)nice, .usage = "Usage: nice [pid] [new_prio]"},
    {.name = "mmstate", .isPipeable = 0, .numberArgs = 0, .start = (ProcessStart)memory_manager_state, .usage = "Usage: mmstate"},
    {.name = "testpipe", .isPipeable = 0, .numberArgs = 0, .start = (ProcessStart)test_pipe, .usage = "Usage: testpipe"}
};


int findCommand(char *command) {
  for(int i = 0; i < COMMANDS_SIZE; i++){
    if(strcmp(commandList[i].name, command)==0){
      return i;
    }
  }
  return -1;
}

int interpret(char ** args, int argc){
  int auxArgc = argc;
  fprintf(STDERR, "ARGC IS: ");
  char bu2f[4];
  itoa(auxArgc, bu2f, 10);
  fprintf(STDERR, bu2f);
  fprintf(STDERR, "\n");
  int pos = findCommand(args[0]);
  if(pos < 0){
    fprintf(STDERR, "Unknown command \"");
    fprintf(STDERR, args[0]);
    fprintf(STDERR, "\"\n");
    return 1;
  }
    int l;
  for(l = 0; l < auxArgc - 1 ; l++){
        args[l] = args[l+1];
  }
  args[l] = NULL;
  auxArgc--;
  for(int i = 0; i < auxArgc; i++){
    if(strcmp(args[i],"-b") == 0){
      fprintf(STDERR, "Found -b\n");
      bg_flag = 1;
      for(int j = i; j < auxArgc - 1 ; j++){
        args[j] = args[j+1];
      }
      auxArgc--;
    }
    if(strcmp(args[i],"|")){
      //handle piped processescase
    }
  }
  if( (auxArgc) != commandList[pos].numberArgs){
    fprintf(STDERR, commandList[pos].usage);
    fprintf(STDERR, "\n");
    return 1;
  }
  //single process handler;
  char buf[4];
  itoa(bg_flag,buf,10);
  fprintf(STDERR, buf);

  fprintf(STDERR, "Arguments are: \n");
      for(int k = 0; k<auxArgc; k++ ){
        fprintf(STDERR, args[k]);
        fprintf(STDERR, "\n");
      }
  createProcessInfo commandProc = {
    .name = commandList[pos].name,
    .argc = auxArgc,
    .argv = (const char * const *)args,
    .fg_flag = !bg_flag,
    .input = STDIN,
    .output = STDOUT,
    .priority = DEFAULT_PRIORITY,
    .start = commandList[pos].start
  };
  int pid = createProcess(&commandProc);
  if(isForeground(pid)){
    fprintf(STDERR, "PROCESS IS IN FOREGROUND\n");
  }
  ps();
  if(!bg_flag){
    waitForPID(pid);
  }
  bg_flag = 0;
  return 0;
}

void insertCommand() {
  char buffer[BUFFER_SIZE] = {'\0'};
  int bufferIndex = 0;
  char c = 0;

  while ((c = getChar()) != '\n' && bufferIndex < BUFFER_SIZE) {
    if (c != '\0') {
      if (c == '\b' && bufferIndex > 0) {
        buffer[--bufferIndex] = '\0';
        putChar(c);
      } else if (c != '\b') {
        buffer[bufferIndex++] = c;
        putChar(c);
      }
    }
  }
  fprintf(STDOUT,"\n");
  char *args[BUFFER_SIZE] = {NULL};
  int argc = tokenize(buffer, args);
  if (argc > 0) { //if at least one command
    interpret(args, argc);
  }
}

void shell() {
  help();
  while (1) {
    fprintf(STDOUT, "caOS>");
    insertCommand();
  }
}