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
#include <_loader.h>
#define BUFFER_SIZE   1024
#define COMMANDS_SIZE 20
#define MAXMEMORY     (0x2000000 - 0xF00000)

extern void haltcpu();
void *memcpy(void *destination, const void *source, uint64_t length) {
  uint64_t i;
  if ((uint64_t) destination % sizeof(uint32_t) == 0 && (uint64_t) source % sizeof(uint32_t) == 0 &&
      length % sizeof(uint32_t) == 0) {
    uint32_t *d = (uint32_t *) destination;
    const uint32_t *s = (const uint32_t *) source;
    for (i = 0; i < length / sizeof(uint32_t); i++)
      d[i] = s[i];
  } else {
    uint8_t *d = (uint8_t *) destination;
    const uint8_t *s = (const uint8_t *) source;
    for (i = 0; i < length; i++)
      d[i] = s[i];
  }
  return destination;
}

static char *commands[] = {"help",    "time",          "eliminator", "regs",     "clear",    "scaledown", "scaleup",
                           "divzero", "invalidopcode", "testmm",     "testproc", "testprio", "testsync",  "testnosync",
                           "ps",      "loop",          "kill",       "block",    "unblock",  "nice"};

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
    createProcessInfo testmm = {.name = "memory",
                                .fg_flag = !in_bg,
                                .priority = DEFAULT_PRIORITY,
                                .start = (ProcessStart) test_mm,
                                .argc = argc,
                                .argv = (const char *const *) argv};
    createProcess(&testmm);
    break;
  case 10:
    createProcessInfo testproc = {.name = "processes",
                                  .fg_flag = !in_bg,
                                  .priority = DEFAULT_PRIORITY,
                                  .start = (ProcessStart) test_processes,
                                  .argc = argc,
                                  .argv = (const char *const *) argv};
    createProcess(&testproc);
    break;
  case 11:
    createProcessInfo testprio = {.name = "priority",
                                  .fg_flag = !in_bg,
                                  .priority = DEFAULT_PRIORITY,
                                  .start = (ProcessStart) test_prio,
                                  .argc = argc,
                                  .argv = (const char *const *) argv};
    createProcess(&testprio);
    break;
  case 12: {
    createProcessInfo decInfo = {.name = "processSynchro",
                                 .fg_flag = !in_bg,
                                 .priority = DEFAULT_PRIORITY,
                                 .start = (ProcessStart) testSync,
                                 .argc = argc,
                                 .argv = (const char *const *) argv};
    createProcess(&decInfo);
  } break;
  case 13: {
    createProcessInfo decInfo = {.name = "processNoSynchro",
                                 .fg_flag = !in_bg,
                                 .priority = DEFAULT_PRIORITY,
                                 .start = (ProcessStart) testNoSync,
                                 .argc = argc,
                                 .argv = (const char *const *) argv};
    createProcess(&decInfo);
  } break;
  case 14:
    ps();
    break;
  case 15:
    createProcessInfo loopInfo = {.name = "loop",
                                  .fg_flag = !in_bg,
                                  .priority = DEFAULT_PRIORITY,
                                  .start = (ProcessStart) loop,
                                  .argc = argc,
                                  .argv = (const char *const *) argv};
    createProcess(&loopInfo);
    break;
  case 16:
    if (argc != 1) {
      print("Usage: kill <pid>\n");
      break;
    }
    pid pid_to_kill = satoi(argv[0]);
    if (pid_to_kill <= 0) {
      print("Invalid PID\n");
      break;
    }
    if (kill(pid_to_kill) != 0) {
      print("Error killing process\n");
    }
    break;
  case 17:
    if (argc != 1) {
      print("Usage: block <pid>\n");
      break;
    }
    block(satoi(argv[0]));
    break;
  case 18:
    if (argc != 1) {
      print("Usage: unblock <pid>\n");
      break;
    }
    unblock(satoi(argv[0]));
    break;
  case 19:
    if (argc != 2) {
      print("Usage: nice <pid> <new_priority>\n");
      break;
    }
    nice(satoi(argv[0]), satoi(argv[1]));
    break;
  default:
    print("Unrecognized command\n");
    errorSound();
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
  print("\n");

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

  print("caOS>");
}

void shell() {
  print("Welcome to caOS shell! Please input your command\n");
  print("Choose from the following:");
  print("\n * clear: Clean the screen");
  print("\n * divzero: Divide by 0 to trigger a zero division exception");
  print("\n * eliminator: Play Eliminator against yourself or a friend. Use WASD to move Player 1, IJKL to move Player 2");
  print("\n * help: Lists the available commands");
  print("\n * invalidopcode: Make an invalid opcode to trigger an invalid opcode exception");
  print("\n * regs: Display the last saved value of registers, press CTRL to save the registers");
  print("\n * scaledown: Reduce the text size (min: 1, default: 1)");
  print("\n * scaleup: Increment the text size (max: 4, default: 1)");
  print("\n * time: Display the current time");
  print("\n * ps: List all processes");
  print("\n * loop: Run an endless loop");
  print("\n * kill: Kill a process by its PID");
  print("\n * block: Block a process by its PID");
  print("\n * unblock: Unblock a process by its PID");
  print("\n * nice: Change the priority of a process by its PID");
  print("\n ----------------------------Tests----------------------------");
  print("\n * testmm: Run a memory management test in an endless loop");
  print("\n * testprio: Run a priority test");
  print("\n * testproc: Run a process management test in an endless loop. Receives max processes as parameter");
  print("\n * testsync: Run synchronization test using semaphores");
  print("\n * testnosync: Run synchronization test without semaphores");
  print("\n");
  print("caOS>");
  while (1) {
    insertCommand();
  }
}
