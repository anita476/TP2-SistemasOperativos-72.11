// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <_loader.h>
#include <commands.h>
#include <defs.h>
#include <eliminator.h>
#include <libSysCalls.h>
#include <utils.h>
#include <test_util.h>

#define TIME_LENGTH 9

void help() {
  fprintf(STDOUT, "Welcome to caOS shell! Please input your command\n");
  fprintf(STDOUT, "Choose from the following:");
  fprintf(STDOUT, "\n ----------------------------General--------------------------");
  fprintf(STDOUT, "\n * clear: Clean the screen");
  fprintf(STDOUT, "\n * divzero: Divide by 0 to trigger a zero division exception");
  fprintf(STDOUT, "\n * eliminator: Play Eliminator against yourself or a friend. Use WASD to move Player 1, IJKL to "
                  "move Player 2");
  fprintf(STDOUT, "\n * help: Lists the available commands");
  fprintf(STDOUT, "\n * invalidopcode: Make an invalid opcode to trigger an invalid opcode exception");
  fprintf(STDOUT, "\n * regs: Display the last saved value of registers, press CTRL to save the registers");
  fprintf(STDOUT, "\n * scaledown: Reduce the text size (min: 1, default: 1)");
  fprintf(STDOUT, "\n * scaleup: Increment the text size (max: 4, default: 1)");
  fprintf(STDOUT, "\n * time: Display the current time");
  fprintf(STDOUT, "\n ----------------------------Memory---------------------------");
  fprintf(STDOUT, "\n * mem: Display the current state of the memory manager");
  fprintf(STDOUT, "\n ---------------------------Processes-------------------------");
  fprintf(STDOUT, "\n * ps: List all processes");
  fprintf(STDOUT, "\n * loop: Run an endless loop");
  fprintf(STDOUT, "\n * kill: Kill a process by its PID");
  fprintf(STDOUT, "\n * block: Block a process by its PID");
  fprintf(STDOUT, "\n * unblock: Unblock a process by its PID");
  fprintf(STDOUT, "\n * nice: Change the priority of a process by its PID");
  fprintf(STDOUT, "\n -----------------------------IPC-----------------------------");
  fprintf(STDOUT, "\n The following commands can be connected using pipes (-):");
  fprintf(STDOUT, "\n * cat: Print the contents of a file or echo the input");
  fprintf(STDOUT, "\n * wc: Count the number of lines, words, and bytes");
  fprintf(STDOUT, "\n * filter: Filter the input by removing vowels");
  fprintf(STDOUT, "\n * phylo: Dining Philosophers problem");
  fprintf(STDOUT, "\n ----------------------------Tests----------------------------");
  fprintf(STDOUT, "\n * testmm: Run a memory management test in an endless loop");
  fprintf(STDOUT, "\n * testprio: Run a priority test");
  fprintf(STDOUT, "\n * testproc: Run a process management test in an endless loop");
  fprintf(STDOUT, "\n * testsync: Run synchronization test with or without semaphores");
  fprintf(STDOUT, "\n * testpipe: Run a pipe usage test");
  fprintf(STDOUT, "\n");
}

void time() {
  uint64_t time = sys_get_time();
  char toReturn[TIME_LENGTH] = {'\0'};

  fprintf(STDOUT, "Current time is: ");
  for (int i = TIME_LENGTH - 2; i >= 0; i--) {
    if (i == 2 || i == 5) {
      toReturn[i] = ':';
    } else {
      toReturn[i] = (time % 10) + '0';
      time = time / 10;
    }
  }
  fprintf(STDOUT, toReturn);
  fprintf(STDOUT, "\n");
  return;
}

void regs() {
  uint64_t buffer[17];
  char getRegsUnsuccessful = sys_get_registers(buffer);
  if (getRegsUnsuccessful) {
    fprintf(STDERR, "Press ctrl key to save registers\n");
    return;
  }
  char *registerNames[] = {" RAX", " RBX", " RCX", " RDX", " RSI", " RDI", " RBP", " RSP", " R8",
                           " R9",  " R10", " R11", " R12", " R13", " R14", " R15", " RIP"};
  for (int i = 0; i < 17; i++) {
    char str[8] = {0};
    fprintf(STDOUT, registerNames[i]);
    fprintf(STDOUT, ": ");
    if (i == 8 || i == 9) {
      fprintf(STDOUT, " ");
    }
    fprintf(STDOUT, itoa(buffer[i], str, 16));
    fprintf(STDOUT, "h");
    fprintf(STDOUT, "\n");
  }
  return;
}

char *get_fd_name(unsigned int fd) {
  switch (fd) {
  case STDIN:
    return "STDIN ";
  case STDOUT:
    return "STDOUT";
  case STDERR:
    return "STDERR";
  case KBDIN:
    return "KBDIN ";
  default:
    return "PIPE  ";
  }
}

int ps() {
  ProcessInfo array[MAX_PROCESSES];
  int count = sys_list_processes_info(array, MAX_PROCESSES);

  fprintf(STDOUT, "PID     Name           Status     Priority     Foreground     Parent     Input     Output\n");
  fprintf(STDOUT, "---     ----           ------     --------     ----------     ------     -----     ------\n");

  for (int i = 0; i < count; i++) {
    const char *status = array[i].status == READY     ? "READY   "
                         : array[i].status == RUNNING ? "RUNNING "
                         : array[i].status == BLOCKED ? "BLOCKED "
                         : array[i].status == KILLED  ? "KILLED  "
                                                      : "UNKNOWN ";

    char buffer[300] = {0};
    char pidStr[10], prioStr[10], fgStr[10], parentStr[10];

    itoa(array[i].pid, pidStr, 10);
    itoa(array[i].priority, prioStr, 10);
    itoa(array[i].fg_flag, fgStr, 10);

    if (array[i].parent == NO_PROC) {
      strcpy(parentStr, "-");
    } else {
      itoa(array[i].parent, parentStr, 10);
    }

    strcpy(buffer, pidStr);
    for (int j = strlen(pidStr); j < 8; j++) {
      strcat(buffer, " ");
    }

    strcat(buffer, array[i].name);
    for (int j = strlen(array[i].name); j < 15; j++) {
      strcat(buffer, " ");
    }

    strcat(buffer, status);
    for (int j = strlen(status); j < 11; j++) {
      strcat(buffer, " ");
    }

    strcat(buffer, prioStr);
    for (int j = strlen(prioStr); j < 13; j++) {
      strcat(buffer, " ");
    }

    strcat(buffer, fgStr);
    for (int j = strlen(fgStr); j < 15; j++) {
      strcat(buffer, " ");
    }

    strcat(buffer, parentStr);
    for (int j = strlen(parentStr); j < 11; j++) {
      strcat(buffer, " ");
    }

    strcat(buffer, get_fd_name(array[i].input));
    for (int j = strlen(get_fd_name(array[i].input)); j < 10; j++) {
      strcat(buffer, " ");
    }

    strcat(buffer, get_fd_name(array[i].output));
    strcat(buffer, "\n");

    fprintf(STDOUT, buffer);
  }

  return 0;
}

void loop() {
  pid_t pid = sys_get_pid();
  char buffer[10];
  // PVS marks this as an infinite loop. It is correct, that's the idea
  while (1) {
    itoa(pid, buffer, 10);
    fprintf(STDOUT, "Hello from process ");
    fprintf(STDOUT, buffer);
    fprintf(STDOUT, "!\n");
    sys_wait(2000);
  }
}

void cat() {
  char buffer[BUFFER_SIZE] = {0};
  while (1) {
    int res = sys_read(STDIN, buffer, 10);
    if (res < 0) {
      return;
    }
    if (buffer[0] == (EOF)) {
      return;
    }
    for (int i = res; i < BUFFER_SIZE; i++) {
      buffer[i] = 0;
    }
    fprintf(STDOUT, buffer);
  }
}

void scale_down_command() {
  sys_scale_down();
  sys_clear_screen();
}

void scale_up_command() {
  sys_scale_up();
  sys_clear_screen();
}

int wc() {
  char buffer[BUFFER_SIZE];
  int bytesRead;
  int lineCount = 0;
  char lastWasNewline = 1;

  while ((bytesRead = sys_read(STDIN, buffer, 1)) > 0) {
    if (buffer[0] == '\n') {
      lineCount++;
      lastWasNewline = 1;
    } else if (buffer[0] == -1) {
      if (!lastWasNewline) {
        lineCount++;
      }
      break;
    } else {
      lastWasNewline = 0;
    }
  }
  char result[20];
  itoa(lineCount, result, 10);
  fprintf(STDOUT, "Number of lines written: ");
  fprintf(STDOUT, result);
  fprintf(STDOUT, "\n");

  return 0;
}

int is_vowel(char c) {
  char vowels[] = {'a', 'e', 'i', 'o', 'u', 'A', 'E', 'I', 'O', 'U'};
  for (int i = 0; i < 10; i++) {
    if (c == vowels[i]) {
      return 1;
    }
  }
  return 0;
}

int filter() {
  char buffer[BUFFER_SIZE];
  char output[BUFFER_SIZE];
  int bytesRead;

  while (1) {
    bytesRead = sys_read(STDIN, buffer, BUFFER_SIZE);
    if (bytesRead <= 0) {
      break;
    }

    int outIndex = 0;
    for (int i = 0; i < bytesRead; i++) {
      if ((int) buffer[i] == EOF) {
        return 0;
      }
      if (!is_vowel(buffer[i])) {
        output[outIndex++] = buffer[i];
      }
    }

    if (outIndex > 0) {
      output[outIndex] = '\0';
      fprintf(STDOUT, output);
    }
  }

  return 0;
}

void print_memory_info() {
  memoryInfo *info = sys_get_memory_info();
  char buffer[32];

  fprintf(STDOUT, "=== Memory Manager State ===\n");

  fprintf(STDOUT, "Total memory: ");
  itoa(info->totalSize, buffer, 10);
  fprintf(STDOUT, buffer);
  fprintf(STDOUT, " bytes\n");

  fprintf(STDOUT, "Free memory: ");
  itoa(info->freeSize, buffer, 10);
  fprintf(STDOUT, buffer);
  fprintf(STDOUT, " bytes\n");

  fprintf(STDOUT, "Allocated memory: ");
  itoa(info->allocatedSize, buffer, 10);
  fprintf(STDOUT, buffer);
  fprintf(STDOUT, " bytes\n");
}

int nice_command( uint64_t argc, char *argv[] ){
  int pid = satoi(argv[0]);
  int prio = satoi(argv[1]);
  return sys_nice(pid, prio);
}