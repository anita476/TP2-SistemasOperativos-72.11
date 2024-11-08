// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <_loader.h>
#include <commands.h>
#include <eliminator.h>

#define TIME_LENGTH 9

void help() {
  fprintf(STDOUT, "Welcome to caOS shell! Please input your command\n");
  fprintf(STDOUT, "Choose from the following:");
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
  fprintf(STDOUT, "\n * ps: List all processes");
  fprintf(STDOUT, "\n * loop: Run an endless loop");
  fprintf(STDOUT, "\n * kill: Kill a process by its PID");
  fprintf(STDOUT, "\n * block: Block a process by its PID");
  fprintf(STDOUT, "\n * unblock: Unblock a process by its PID");
  fprintf(STDOUT, "\n * nice: Change the priority of a process by its PID");
  fprintf(STDOUT, "\n * mmstate: Display the current state of the memory manager");
  fprintf(STDOUT, "\n ----------------------------Tests----------------------------");
  fprintf(STDOUT, "\n * testmm: Run a memory management test in an endless loop");
  fprintf(STDOUT, "\n * testprio: Run a priority test");
  fprintf(STDOUT,
          "\n * testproc: Run a process management test in an endless loop. Receives max processes as parameter");
  fprintf(STDOUT, "\n * testsync: Run synchronization test using semaphores");
  fprintf(STDOUT, "\n * testnosync: Run synchronization test without semaphores");
  fprintf(STDOUT, "\n * testpipe: Run a pipe usage test. No parameters needed");
  fprintf(STDOUT, "\n");
}

void time() {
  uint64_t time = getTime();
  char toReturn[TIME_LENGTH] = {'\0'};

  fprintf(STDOUT, "Current time is: ");
  for (int i = TIME_LENGTH - 2; i >= 0; i--) {
    if (i == 2 || i == 5)
      toReturn[i] = ':';
    else {
      toReturn[i] = (time % 10) + '0';
      time = time / 10;
    }
  }
  fprintf(STDOUT, toReturn);
  fprintf(STDOUT, "\n");
  return;
}

char *itoa(int num, char *str, int base) {
  int i = 0;
  char isNegative = 0;

  // If the number is zero...
  if (num == 0) {
    str[i++] = '0';
    str[i] = '\0';
    return str;
  }

  // If the number is negative...
  if (num < 0 && base == 10) {
    isNegative = 1;
    num = -num;
  }

  while (num != 0) {
    int rem = num % base;
    str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
    num = num / base;
  }

  // If the number is negative...
  if (isNegative)
    str[i++] = '-';

  str[i] = '\0';

  // Reverse the string
  int start = 0;
  int end = i - 1;
  while (start < end) {
    char temp = str[start];
    str[start] = str[end];
    str[end] = temp;
    start++;
    end--;
  }

  return str;
}

void regs() {
  uint64_t buffer[17];
  char getRegsUnsuccessful = getRegisters(buffer);
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
    if (i == 8 || i == 9)
      fprintf(STDOUT, " ");
    fprintf(STDOUT, itoa(buffer[i], str, 16));
    fprintf(STDOUT, "h");
    fprintf(STDOUT, "\n");
  }
  return;
}

char *getFDName(unsigned int fd) {
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
  int count = listProcessesInfo(array, MAX_PROCESSES);

  // Print header with fixed column widths
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

    // Convert numbers to strings
    itoa(array[i].pid, pidStr, 10);
    itoa(array[i].priority, prioStr, 10);
    itoa(array[i].fg_flag, fgStr, 10);

    if (array[i].parent == NO_PROC)
      strcpy(parentStr, "-");
    else
      itoa(array[i].parent, parentStr, 10);

    // Build the output string with fixed column widths
    strcpy(buffer, pidStr);
    for (int j = strlen(pidStr); j < 8; j++)
      strcat(buffer, " ");  // PID width: 8

    strcat(buffer, array[i].name);
    for (int j = strlen(array[i].name); j < 15; j++)
      strcat(buffer, " ");  // Name width: 15

    strcat(buffer, status);
    for (int j = strlen(status); j < 11; j++)
      strcat(buffer, " ");  // Status width: 12

    strcat(buffer, prioStr);
    for (int j = strlen(prioStr); j < 13; j++)
      strcat(buffer, " ");  // Priority width: 12

    strcat(buffer, fgStr);
    for (int j = strlen(fgStr); j < 15; j++)
      strcat(buffer, " ");  // Foreground width: 14

    strcat(buffer, parentStr);
    for (int j = strlen(parentStr); j < 11; j++)
      strcat(buffer, " ");  // Parent width: 11

    strcat(buffer, getFDName(array[i].input));
    for (int j = strlen(getFDName(array[i].input)); j < 10; j++)
      strcat(buffer, " ");  // Input width: 10

    strcat(buffer, getFDName(array[i].output));
    strcat(buffer, "\n");

    fprintf(STDOUT, buffer);
  }

  return 0;
}

void loop() {
  pid_t pid = getpid();
  char buffer[10];

  while (1) {
    itoa(pid, buffer, 10);
    fprintf(STDOUT, "Hello from process ");
    fprintf(STDOUT, buffer);
    fprintf(STDOUT, "!\n");
    wait(2000);
  }
}