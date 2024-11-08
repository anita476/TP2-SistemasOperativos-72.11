// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "shell.h"
#include "commands.h"
#include "eliminator.h"
#include "libSysCalls.h"
#include "music.h"
#include "test_mm.h"
#include "test_no_sync.h"
#include "test_prio.h"
#include "test_processes.h"
#include "test_sync.h"
#include "test_util.h"
#include <test_pipe.h>
#include <_loader.h>
#define BUFFER_SIZE   1024
#define COMMANDS_SIZE 21

static char *commands[] = {"help",    "time",          "eliminator", "regs",     "clear",    "scaledown", "scaleup",
                           "divzero", "invalidopcode", "testmm",     "testproc", "testprio", "testsync", "ps",
                           "loop",          "kill",       "block",    "unblock",  "nice", "mmstate", "testpipe"};

int isCommand(char *str, int command) {
  if (command >= COMMANDS_SIZE)
    return -1;

  int i = 0;
  for (; str[i] != '\0' && commands[command][i] != '\0'; i++)
    if (str[i] != commands[command][i])
      return 0;

  return str[i] == commands[command][i];
}

int findCommand(char *str) {
  for (int i = 0; i < COMMANDS_SIZE; i++)
    if (isCommand(str, i))
      return i;
  return -1;
}

void executeCommand(char *str, int argc, char *argv[]) {
  int PID;
  int in_bg = 0;
  if (argc > 0 && strcmp(argv[0], "-b") == 0) {
    in_bg = 1;
    argv++;
    argc--;
  }

  switch (findCommand(str)) {
  case 0:
    help();
    break;
  case 1:
    time();
    break;
  case 2:
    eliminator();
    break;
  case 3:
    regs();
    break;
  case 4:
    clearScreen();
    break;
  case 5:
    scaleDown();
    clearScreen();
    break;
  case 6:
    scaleUp();
    clearScreen();
    break;
  case 7:
    divzero();
    break;
  case 8:
    invalidOpCode();
    break;
  case 9:
  {
    createProcessInfo testmm = {.name = "memory",
                                .fg_flag = !in_bg,
                                .priority = DEFAULT_PRIORITY,
                                .start = (ProcessStart) test_mm,
                                .argc = argc,
                                .argv = (const char *const *) argv,
                                .input = STDIN,
                                .output = STDOUT};
                              
    PID = createProcess(&testmm);
    if(!in_bg){
      waitForPID(PID);
    }
  }
    break;
  case 10:
  {
    createProcessInfo testproc = {.name = "processes",
                                  .fg_flag = !in_bg,
                                  .priority = DEFAULT_PRIORITY,
                                  .start = (ProcessStart) test_processes,
                                  .argc = argc,
                                  .argv = (const char *const *) argv,
                                  .input = STDIN,
                                  .output = STDOUT};
    PID = createProcess(&testproc);
    if(!in_bg){
      waitForPID(PID);
    }
  }
    break;
  case 11:
  {
    createProcessInfo testprio = {.name = "priority",
                                  .fg_flag = !in_bg,
                                  .priority = DEFAULT_PRIORITY,
                                  .start = (ProcessStart) test_prio,
                                  .argc = argc,
                                  .argv = (const char *const *) argv,
                                  .input = STDIN,
                                  .output = STDOUT
                                  };
    createProcess(&testprio);
    PID = createProcess(&testprio);
    if(!in_bg){
      waitForPID(PID);
    }
  }
    break;
  case 12: {
    createProcessInfo decInfo = {.name = "processSynchro",
                                 .fg_flag = !in_bg,
                                 .priority = DEFAULT_PRIORITY,
                                 .start = (ProcessStart) testSync,
                                 .argc = argc,
                                 .argv = (const char *const *) argv,
                                 .input = STDIN,
                                 .output = STDOUT};
    PID = createProcess(&decInfo);
    if(!in_bg){
      waitForPID(PID);
    }
  } break;
  case 13:
    ps();
    break;
  case 14:
  {
    createProcessInfo loopInfo = {.name = "loop",
                                  .fg_flag = !in_bg,
                                  .priority = DEFAULT_PRIORITY,
                                  .start = (ProcessStart) loop,
                                  .argc = argc,
                                  .argv = (const char *const *) argv,
                                  .input = STDIN,
                                  .output = STDOUT};
    PID = createProcess(&loopInfo);
    if(!in_bg){
      waitForPID(PID);
    }
  }
    break;
  case 15:
  {
    if (argc != 1) {
      fprintf(STDERR,"Usage: kill <pid>\n");
      break;
    }
    pid pid_to_kill = satoi(argv[0]);
    if (pid_to_kill <= 0) {
      fprintf(STDERR, "Invalid PID\n");
      break;
    }
    if (kill(pid_to_kill) != 0) {
      fprintf(STDERR,"Error killing process\n");
    }
  }
    break;
  case 16:
  {
    if (argc != 1) {
      fprintf(STDERR, "Usage: block <pid>\n");
      break;
    }
    block(satoi(argv[0]));
  }
    break;
  case 17:
  {
    if (argc != 1) {
      fprintf(STDERR,"Usage: unblock <pid>\n");
      break;
    }
    unblock(satoi(argv[0]));
  }
    break;
  case 18:
  {
    if (argc != 2) {
      fprintf(STDERR, "Usage: nice <pid> <new_priority>\n");
      break;
    }
    nice(satoi(argv[0]), satoi(argv[1]));
  }
    break;
  case 19: 
    memory_manager_state();
    break;
  case 20:
  {
    createProcessInfo pipetestInfo = {.name = "pipe_test",
                                  .fg_flag = !in_bg,
                                  .priority = DEFAULT_PRIORITY,
                                  .start = (ProcessStart) test_pipe,
                                  .argc = argc,
                                  .argv = (const char *const *) argv,
                                  .input = STDIN,
                                  .output = STDOUT};
    PID = createProcess(&pipetestInfo);
    if(!in_bg){
      waitForPID(PID);
    }
  }
  break;
  default:
  {
    fprintf(STDERR, "Unrecognized command\n");
    errorSound();
  }
    break;
  }
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
  int argc = 0;
  char *current = buffer;
  args[argc++] = current;

  while (*current) {
    if (*current == ' ') {
      *current = '\0';
      current++;
      if (*current && argc < BUFFER_SIZE) {
        args[argc++] = current;
      }
    } else {
      current++;
    }
  }

  if (argc > 0) {
    executeCommand(args[0], argc - 1, args + 1);
  }

}

void shell() {
  help();
  while (1) {
    fprintf(STDOUT, "caOS>");
    insertCommand();
  }
}
