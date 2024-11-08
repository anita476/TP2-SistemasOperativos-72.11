// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "shell.h"

#define COMMANDS_SIZE 21

int bg_flag = 1;


int findCommand(char *command) {
  for(int i = 0; i < COMMANDS_SIZE; i++){
    if(strcmp(commandList[i].name, command)==0){
      return i;
    }
  }
  return -1;
}

interpret(char ** args, int argc){
  char * command = args[0];
  int pos = findCommand(args[0]);
  if(pos < 0){
    fprintf(STDERR, "Unknown command \"");
    fprintf(STDERR, command);
    fprintf(STDERR, "\"\n");
    return 1;
  }
  for(int i = 1; i < argc; i++){
    if(strcmp(args[i],"-b") == 0){
      fprintf(STDERR, "Found -b\n");
      bg_flag = 1;
      argc--;
      for(int j = i; j < argc - 1 ; j++){
        args[i] = args[i+1];
      }
    }
    if(strcmp(args[i],"|")){
      //handle piped processescase
    }
  }
  if( (argc - 1) != commandList[pos].numberArgs){
    fprintf(STDERR, commandList[pos].usage);
    fprintf(STDERR, "\n");
    return 1;
  }
  //single process handler;
  char buf[4];
  itoa(bg_flag,buf,10);
  fprintf(STDERR, buf);

  fprintf(STDERR, "Arguments are: \n");
      for(int k = 0; k<argc; k++ ){
        fprintf(STDERR, args[k]);
        fprintf(STDERR, "\n");
      }
    
  createProcessInfo commandProc = {
    .name = commandList[pos].name,
    .argc = argc -1,
    .argv = args +1,
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