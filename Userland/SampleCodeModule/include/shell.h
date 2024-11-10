#ifndef SHELL_H
#define SHELL_H
#include "commands.h"
#include "eliminator.h"
#include "libSysCalls.h"
#include "music.h"
#include "phylo.h"
#include "shellUtils.h"
#include "test_mm.h"
#include "test_prio.h"
#include "test_processes.h"
#include "test_sync.h"
#include "test_util.h"
#include <_loader.h>
#include <test_pipe.h>

#define COMMANDS_SIZE 25
#define BUFFER_SIZE   1024

/*
    @brief Invalid opcode exception
*/
extern void invalid_opcode();

/*
    @brief Division by zero exception
*/
extern void div_zero();

typedef struct Command {
  char *name;
  unsigned int isPipeable;
  unsigned int numberArgs;
  ProcessStart start;
  char *usage;
} Command;

/*
    @brief Finds a command
*/
int find_command(char *command);

/*
    @brief Starts the shell
*/
void shell();

#endif
