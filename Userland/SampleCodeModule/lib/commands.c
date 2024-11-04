// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <commands.h>
#include <eliminator.h>
#include <_loader.h>

#define TIME_LENGTH 9

void help() {
    print("Welcome to caOS Shell Manual! Here's the list of commands available:");
    print("\n * clear: Clean the screen");
    print("\n * divzero: Divide by 0 to trigger a zero division exception");
    print("\n * eliminator: Play Eliminator against yourself or a friend. Use WASD to move Player 1, IJKL to move Player 2");
    print("\n * invalidopcode: Make an invalid opcode to trigger an invalid opcode exception");
    print("\n * regs: Display the last saved value of registers, press CTRL to save the registers");
    print("\n * scaledown: Reduce the text size (min: 1, default: 1)");
    print("\n * scaleup: Increment the text size (max: 4, default: 1)");
    print("\n * time: Display the current time");
    print("\n * testmm: Run a memory management test in an endless loop");
    print("\n * testprio: Run a priority test");
    print("\n * testproc: Run a process management test in an endless loop");
    print("\n * testsync: Run a semaphore test");
    print("\n * testsync: Run synchronization test using semaphores (10 iterations)");
    print("\n * testnosync: Run synchronization test without semaphores (10 iterations)");
    print("\n * ps: Display the current processes");
    print("\n * loop: Run an endless loop");
    print("\n * kill: Kill a process by its PID");
    print("\n * block: Block a process by its PID");
    print("\n * unblock: Unblock a process by its PID");
    print("\n * nice: Change the priority of a process by its PID");
    print("\n");
}

void time() {
    uint64_t time = getTime();
    char toReturn[TIME_LENGTH] = {'\0'};

    print("Current time is: ");
    for (int i = TIME_LENGTH-2; i >= 0 ; i--) {
        if (i == 2 || i == 5) toReturn[i] = ':';
        else {
            toReturn[i] = (time % 10) + '0';
            time = time / 10;
        }
    }
    print(toReturn);
    print("\n");
    return;
}

char* itoa(int num, char* str, int base) {
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
    if (isNegative) str[i++] = '-';

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
        print("Press ctrl key to save registers\n");
        return;
    }
    char * registerNames[] = {" RAX", " RBX", " RCX", " RDX", " RSI", " RDI", " RBP", " RSP", " R8", " R9", " R10", " R11", " R12",
        " R13", " R14", " R15", " RIP"};
    for (int i = 0; i < 17; i++) {
        char str[8] = {0};
        print(registerNames[i]); print(": ");
        if (i == 8 || i == 9) print(" ");
        print(itoa(buffer[i], str, 16));
        print("h");
        print("\n");
    }
    return;
}

int ps() {
    ProcessInfo array[MAX_PROCESSES];
    int count = listProcessesInfo(array, MAX_PROCESSES);
    
    // Print header
    print("PID    Name           Status    Priority    Foreground    Parent\n");
    print("---    ----           ------    --------    ----------    ------\n");

    for (int i = 0; i < count; i++) {
        const char *status = array[i].status == READY     ? "READY   "
                            : array[i].status == RUNNING  ? "RUNNING "
                            : array[i].status == BLOCKED  ? "BLOCKED "
                            : array[i].status == KILLED   ? "KILLED  "
                                                         : "UNKNOWN ";
        
        char buffer[300];
        char pidStr[10], prioStr[10], fgStr[10], parentStr[10];
        
        itoa(array[i].pid, pidStr, 10);
        itoa(array[i].priority, prioStr, 10);
        itoa(array[i].fg_flag, fgStr, 10);
        
        if (array[i].parent == NO_PROC) strcpy(parentStr, "NONE");
        else itoa(array[i].parent, parentStr, 10);
        
        strcpy(buffer, pidStr);
        strcat(buffer, "      ");
        strcat(buffer, array[i].name);
        int nameLen = strlen(array[i].name);
        for (int j = nameLen; j < 15; j++) {
            strcat(buffer, " ");
        }
        strcat(buffer, status);
        strcat(buffer, "  ");
        strcat(buffer, prioStr);
        strcat(buffer, "           ");
        strcat(buffer, fgStr);
        strcat(buffer, "             ");
        strcat(buffer, parentStr);
        strcat(buffer, "\n");
        
        print(buffer);
    }

    return 0;
}

void loop() {
    pid_t pid = getpid();
    char buffer[10];
    
    while (1) {
        itoa(pid, buffer, 10);
        print("Hello from process ");
        print(buffer);
        print("!\n");
        wait(2000);
    }
}