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
#define BUFFER_SIZE 1024
#define COMMANDS_SIZE 12
#define MAXMEMORY  (0x2000000 - 0xF00000) 

static char* commands[] = {"help", "time", "eliminator", "regs", "clear", "scaledown", "scaleup", "divzero", "invalidopcode","testmm", "testproc","testprio"};

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
            char * argv[1] = {"500000"};
            if (test_mm(1, argv) == (-1)) {
                  print("Memory test failed\n");
            } 
            break;
      case 10:
            char * argv2[1] = {"8"};
            if (test_processes(1, argv2) == (-1)) {
                  print("Process test failed\n");
            }
            break;
      case 11:
            test_prio();
            break;
      default: 
            print("Unrecognized command\n");
            errorSound();
            break;
      }
}

void insertCommand() {
      print("caOS>");
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
      insertCommand();
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
      print("\n * testprio : Run a priority test");
      print("\n * testproc : Run a process management test in an endless loop. Receives max processes as parameter");
      print("\n");
      insertCommand();
}

