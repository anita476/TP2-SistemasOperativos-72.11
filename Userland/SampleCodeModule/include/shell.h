#ifndef SHELL_H
#define SHELL_H
#include "shellUtils.h"
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

#define COMMANDS_SIZE 21
#define BUFFER_SIZE   1024

/* 
    @brief Invalid opcode exception
*/
extern void invalidOpCode();

/*
    @brief Division by zero exception
*/
extern void divzero();

typedef struct Command{
    char * name;
    unsigned int isProcess;
    unsigned int isPipeable; /* refers to whether they can be consumers */
    unsigned int numberArgs;
    ProcessStart start;
    char * usage;
}Command;

static Command commandList[COMMANDS_SIZE] = {
    {.name = "help", .isPipeable = 0, .isProcess = 0, .numberArgs = 0, .start = (ProcessStart)help, .usage = "Usage: help"},
    {.name = "time", .isPipeable = 0, .isProcess = 0, .numberArgs = 0, .start = (ProcessStart)time, .usage = "Usage: time"},
    {.name = "eliminator", .isPipeable = 0, .isProcess = 0, .numberArgs = 0, .start = (ProcessStart)eliminator , .usage = "Usage: eliminator"},
    {.name = "regs", .isPipeable = 0, .isProcess = 0, .numberArgs = 0, .start = (ProcessStart)regs, .usage = "Usage: regs"},
    {.name = "clear", .isPipeable = 0 , .isProcess = 0, .numberArgs = 0, .start = (ProcessStart)clearScreen, .usage = "Usage: clear"},
    {.name = "scaledown", .isPipeable = 0, .isProcess = 0, .numberArgs = 0, .start = (ProcessStart)scaleDownCommand , .usage= "Usage: scaledown"},
    {.name = "scaleup", .isPipeable = 0, .isProcess = 0, .numberArgs = 0, .start = (ProcessStart)scaleUpCommand, .usage = "Usage: scaleup"},
    {.name = "divzero", .isPipeable = 0, .isProcess = 0, .numberArgs = 0, .start = (ProcessStart)divzero, .usage = "Usage: divzero"},
    {.name = "invalidopcode", .isPipeable = 0, .isProcess = 0, .numberArgs = 0, .start = (ProcessStart)invalidOpCode, .usage = "Usage: invalidopcode"},
    {.name = "testmm", .isPipeable = 0, .isProcess = 1, .numberArgs = 1, .start = (ProcessStart)test_mm, .usage = "Usage: testmm [max_mem]"},
    {.name = "testproc", .isPipeable = 0, .isProcess = 1, .numberArgs = 1, .start= (ProcessStart)test_processes , .usage = "Usage: testproc [max_procs]"},
    {.name = "testprio", .isPipeable = 0, .isProcess = 1, .numberArgs = 0, .start = (ProcessStart)test_prio, .usage = "Usage: testprio"},
    {.name = "testsync", .isPipeable = 0, .isProcess = 1, .numberArgs = 2, .start = (ProcessStart)testSync, .usage = "Usage: testsync [n_value] [use_sem]"},
    {.name = "ps", .isPipeable = 0, .isProcess = 0, .numberArgs = 0, .start = (ProcessStart)ps, .usage = "Usage: ps"},
    {.name = "loop", .isPipeable = 0, .isProcess = 1, .numberArgs = 0, .start = (ProcessStart)loop, .usage = "Usage: loop"},
    {.name = "kill", .isPipeable = 0, .isProcess = 0, .numberArgs = 1, .start = (ProcessStart)kill, .usage = "Usage: kill [pid]"},
    {.name = "block", .isPipeable = 0, .isProcess = 0, .numberArgs = 1, .start = (ProcessStart)block, .usage = "Usage: block [pid]"},
    {.name = "unblock", .isPipeable = 0, .isProcess = 0, .numberArgs = 1, .start = (ProcessStart)unblock, .usage = "Usage: unblock [pid]"},
    {.name = "nice", .isPipeable = 0, .isProcess = 0, .numberArgs = 1, .start = (ProcessStart)nice, .usage = "Usage: nice [pid] [new_prio]"},
    {.name = "mmstate", .isPipeable = 0, .isProcess = 0, .numberArgs = 0, .start = (ProcessStart)memory_manager_state, .usage = "Usage: mmstate"},
    {.name = "testpipe", .isPipeable = 0, .isProcess = 1, .numberArgs = 0, .start = (ProcessStart)test_pipe, .usage = "Usage: testpipe"}
};

/*
    @brief Starts the shell
*/	
void shell();
#endif // !SHELL_H
