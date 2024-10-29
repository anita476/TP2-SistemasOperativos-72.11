// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "shell.h"
#include "libSysCalls.h"
#include "commands.h"
#include "eliminator.h"
#include "music.h"
#include "test_mm.h"
#include "test_prio.h"
#include "test_processes.h"
#include "test_util.h"
#include "test_sync.h"
#include "test_no_sync.h"
#define BUFFER_SIZE 1024
#define COMMANDS_SIZE 14
#define MAXMEMORY  (0x2000000 - 0xF00000)

extern void haltcpu();
void * memcpy(void * destination, const void * source, uint64_t length) {
	/*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
	uint64_t i;
	if ((uint64_t)destination % sizeof(uint32_t) == 0 && (uint64_t)source % sizeof(uint32_t) == 0 && length % sizeof(uint32_t) == 0) {
		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *)source;
		for (i = 0; i < length / sizeof(uint32_t); i++) d[i] = s[i];
	}
	else {
		uint8_t * d = (uint8_t*)destination;
		const uint8_t * s = (const uint8_t*)source;
		for (i = 0; i < length; i++) d[i] = s[i];
	}
	return destination;
}

size_t strlen(const char *str) {
    size_t l;
    for (l = 0; *str != 0; str++, l++);
    return l;
}

static char* commands[] = {"help", "time", "eliminator", "regs", "clear", "scaledown", "scaleup", "divzero", "invalidopcode","testmm", "testproc","testprio","testsync","testnosync"};

int isCommand(char * str, int command) {
      if (command >= COMMANDS_SIZE) return -1;

      int i = 0;
      for (; str[i] != '\0' && commands[command][i] != '\0'; i++)
            if (str[i] != commands[command][i]) return 0;   
      
      return str[i] == commands[command][i];    // Checking if they have the same length 
}

int findCommand(char * str) {
      for (int i = 0; i < COMMANDS_SIZE; i++)
            if (isCommand(str, i)) return i;
      return -1;
}

void executeCommand(char * str) {
      switch (findCommand(str)) {
      case 0: help(); break;
      case 1: time(); break;
      case 2: eliminator(); break;
      case 3: regs(); break;
      case 4: clearScreen(); break;
      case 5: 
            scaleDown();
            clearScreen(); 
            break;
      case 6: 
            scaleUp(); 
            clearScreen();
            break;
      case 7: divzero(); break;
      case 8: invalidOpCode(); break;
      case 9: 
            {
            char * argv[1] = {"500000"};
            if (test_mm(1, argv) == (-1)) {
                  print("Memory test failed\n");
            }}
            break;
      case 10:
            {
            char * argv2[1] = {0};
            createProcessInfo testproc = {.name = "processes",
                                     .fg_flag = 1,
                                     .priority = DEFAULT_PRIORITY,
                                     .start = (ProcessStart) test_processes,
                                     .argc = 0,
                                     .argv = (const char *const *) argv2};
	      createProcess(&testproc);
            }
            break;
      case 11:
            {
      	createProcessInfo testprio = {.name = "priority",
                                     .fg_flag = 1,
                                     .priority = DEFAULT_PRIORITY,
                                     .start = (ProcessStart) test_prio,
                                     .argc = 0,
                                     .argv = (const char *const *) NULL};
	      createProcess(&testprio);
            }
            break;
      case 12:
            {
	            char * argv [] = {"1","2"};
	      createProcessInfo decInfo = {.name = "processSynchro",
                                    .fg_flag = 1,
                                    .priority = DEFAULT_PRIORITY,
                                    .start = (ProcessStart) testSync,
                                    .argc = 2,
                                    .argv = (const char *const *) argv};
            createProcess(&decInfo);
            }
            break;
      case 13:
            {
                  char * argv [] = {"0","2"};
	      createProcessInfo decInfo = {.name = "processNoSynchro",
                                    .fg_flag = 1,
                                    .priority = DEFAULT_PRIORITY,
                                    .start = (ProcessStart) testNoSync,
                                    .argc = 2,
                                    .argv = (const char *const *) argv};
            createProcess(&decInfo);
            }
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
                  }
                  else if (c != '\b') {
                        buffer[bufferIndex++] = c;
                        putChar(c);
                  }
            }
      }
      print("\n");
      executeCommand(buffer);
      print("caOS>");
      //insertCommand();
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
      print("\n * testmm: Run a memory management test in an endless loop");
      print("\n * testprio: Run a priority test");
      print("\n * testproc: Run a process management test in an endless loop. Receives max processes as parameter");
      print("\n * testsync: Run synchronization test using semaphores (10 iterations)");
      print("\n * testnosync: Run synchronization test without semaphores (10 iterations)");
      print("\n");
      print("caOS>");
      while (1) {
            insertCommand();
      }
}
