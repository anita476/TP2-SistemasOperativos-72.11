// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "shell.h"

int handle_piped_process(int producerArgc, char **producerArgv, int consumerArgc, char **consumerArgv);

int bg_flag = 0;
static Command commandList[COMMANDS_SIZE] = {
    {.name = "help", .isPipeable = 0, .numberArgs = 0, .start = (ProcessStart) help, .usage = "Usage: help"},
    {.name = "time", .isPipeable = 0, .numberArgs = 0, .start = (ProcessStart) time, .usage = "Usage: time"},
    {.name = "eliminator",
     .isPipeable = 0,
     .numberArgs = 0,
     .start = (ProcessStart) eliminator,
     .usage = "Usage: eliminator"},
    {.name = "regs", .isPipeable = 0, .numberArgs = 0, .start = (ProcessStart) regs, .usage = "Usage: regs"},
    {.name = "clear", .isPipeable = 0, .numberArgs = 0, .start = (ProcessStart) clearScreen, .usage = "Usage: clear"},
    {.name = "scaledown",
     .isPipeable = 0,
     .numberArgs = 0,
     .start = (ProcessStart) scaleDownCommand,
     .usage = "Usage: scaledown"},
    {.name = "scaleup",
     .isPipeable = 0,
     .numberArgs = 0,
     .start = (ProcessStart) scaleUpCommand,
     .usage = "Usage: scaleup"},
    {.name = "divzero", .isPipeable = 0, .numberArgs = 0, .start = (ProcessStart) divzero, .usage = "Usage: divzero"},
    {.name = "invalidopcode",
     .isPipeable = 0,
     .numberArgs = 0,
     .start = (ProcessStart) invalidOpCode,
     .usage = "Usage: invalidopcode"},
    {.name = "testmm",
     .isPipeable = 0,
     .numberArgs = 1,
     .start = (ProcessStart) test_mm,
     .usage = "Usage: testmm [max_mem]"},
    {.name = "testproc",
     .isPipeable = 0,
     .numberArgs = 1,
     .start = (ProcessStart) test_processes,
     .usage = "Usage: testproc [max_procs]"},
    {.name = "testprio",
     .isPipeable = 0,
     .numberArgs = 0,
     .start = (ProcessStart) test_prio,
     .usage = "Usage: testprio"},
    {.name = "testsync",
     .isPipeable = 0,
     .numberArgs = 2,
     .start = (ProcessStart) testSync,
     .usage = "Usage: testsync [n_value] [use_sem]"},
    {.name = "ps", .isPipeable = 0, .numberArgs = 0, .start = (ProcessStart) ps, .usage = "Usage: ps"},
    {.name = "loop", .isPipeable = 0, .numberArgs = 0, .start = (ProcessStart) loop, .usage = "Usage: loop"},
    {.name = "kill", .isPipeable = 0, .numberArgs = 1, .start = (ProcessStart) kill, .usage = "Usage: kill [pid]"},
    {.name = "block", .isPipeable = 0, .numberArgs = 1, .start = (ProcessStart) block, .usage = "Usage: block [pid]"},
    {.name = "unblock",
     .isPipeable = 0,
     .numberArgs = 1,
     .start = (ProcessStart) unblock,
     .usage = "Usage: unblock [pid]"},
    {.name = "nice",
     .isPipeable = 0,
     .numberArgs = 1,
     .start = (ProcessStart) nice,
     .usage = "Usage: nice [pid] [new_prio]"},
    {.name = "mmstate",
     .isPipeable = 0,
     .numberArgs = 0,
     .start = (ProcessStart) memory_manager_state,
     .usage = "Usage: mmstate"},
    {.name = "testpipe",
     .isPipeable = 0,
     .numberArgs = 0,
     .start = (ProcessStart) test_pipe,
     .usage = "Usage: testpipe"},
    {.name = "cat", .isPipeable = 1, .numberArgs = 0, .start = (ProcessStart) cat, .usage = "Usage: cat"}};

int findCommand(char *command) {
  for (int i = 0; i < COMMANDS_SIZE; i++) {
    if (strcmp(commandList[i].name, command) == 0) {
      return i;
    }
  }
  return -1;
}

int interpret(char **args, int argc) {
  int auxArgc = argc;

  for (int i = 0; i < auxArgc; i++) {
    if (strcmp(args[i], "-b") == 0) {
      fprintf(STDERR, "Found -b\n");
      bg_flag = 1;
      for (int j = i; j < auxArgc - 1; j++) {
        args[j] = args[j + 1];
      }
      auxArgc--;
    }
    if (strcmp(args[i], "-") == 0) { /* QEMU doesnt let me put "|" */
      int argc1 = i;
      int argc2 = auxArgc - (i + 1);
      char *argv1[argc1];
      char *argv2[argc2];
      for (int j = 0; j < argc1; j++) {
        argv1[j] = args[j];
      }
      for (int j = 0; j < argc2; j++) {
        argv2[j] = args[i + j + 1];
      }
      int res = handle_piped_process(argc1, argv1, argc2, argv2);
      return res;
    }
  }

  // single process handler;

  int pos = findCommand(args[0]);
  if (pos < 0) {
    fprintf(STDERR, "Unknown command \"");
    fprintf(STDERR, args[0]);
    fprintf(STDERR, "\"\n");
    return 1;
  }
  int l;
  for (l = 0; l < auxArgc - 1; l++) {
    args[l] = args[l + 1];
  }
  args[l] = NULL;
  auxArgc--;
  if ((auxArgc) != commandList[pos].numberArgs) {
    fprintf(STDERR, commandList[pos].usage);
    fprintf(STDERR, "\n");
    return 1;
  }
  createProcessInfo commandProc = {.name = commandList[pos].name,
                                   .argc = auxArgc,
                                   .argv = (const char *const *) args,
                                   .fg_flag = !bg_flag,
                                   .input = STDIN,
                                   .output = STDOUT,
                                   .priority = DEFAULT_PRIORITY,
                                   .start = commandList[pos].start};
  int pid = createProcess(&commandProc);
  if (!bg_flag) {
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
  fprintf(STDOUT, "\n");
  char *args[BUFFER_SIZE] = {NULL};
  int argc = tokenize(buffer, args);
  if (argc > 0) {  // if at least one command
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

int handle_piped_process(int producerArgc, char **producerArgv, int consumerArgc, char **consumerArgv) {
  if (producerArgc < 1 || consumerArgc < 1) {
    return -1;
  }
  int pos1 = findCommand(producerArgv[0]);
  int pos2 = findCommand(consumerArgv[0]);
  if (pos1 < 0 || pos2 < 0) {
    fprintf(STDERR, "Unknown commands\n");
    return -1;
  }
  int l;
  for (l = 0; l < producerArgc - 1; l++) {
    producerArgv[l] = producerArgv[l + 1];
  }
  producerArgv[l] = NULL;
  for (l = 0; l < consumerArgc - 1; l++) {
    consumerArgv[l] = consumerArgv[l + 1];
  }
  producerArgv[l] = NULL;

  if (!commandList[pos2].isPipeable) {
    fprintf(STDERR, "Command is not pipeable\n");
    return -1;
  }

  int pipeFD = open_pipe(0);
  if (pipeFD < 0) {
    fprintf(STDERR, "Error opening pipe\n");
    return -1;
  }
  createProcessInfo producerInfo = {.argc = producerArgc,
                                    .argv = (const char *const *) producerArgv,
                                    .name = commandList[pos1].name,
                                    .fg_flag = !bg_flag,
                                    .input = STDIN,
                                    .output = pipeFD,
                                    .priority = DEFAULT_PRIORITY,
                                    .start = commandList[pos1].start};
  createProcessInfo consumerInfo = {.argc = consumerArgc,
                                    .argv = (const char *const *) consumerArgv,
                                    .name = commandList[pos2].name,
                                    .fg_flag = !bg_flag,
                                    .input = pipeFD,
                                    .output = STDOUT,
                                    .priority = DEFAULT_PRIORITY,
                                    .start = commandList[pos2].start};
  int pid1 = createProcess(&producerInfo);
  if (pid1 < 0) {
    fprintf(STDERR, "Error creating producer process\n");
    return -1;
  }
  int pid2 = createProcess(&consumerInfo);
  if (pid2 < 0) {
    fprintf(STDERR, "Error creating consumer process\n");
    return -1;
  }
  if (!bg_flag) {
    waitForPID(pid2);
  }
  return 0;
}