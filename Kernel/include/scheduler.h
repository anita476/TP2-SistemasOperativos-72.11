#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <lib.h>
#include <memoryManagement.h>
#include <processes.h>
#include <stdint.h>
#include <stdlib.h>

#define QUANTUM    5
#define NO_PROC    -2
#define PID_KERNEL -1

#define MAX_PRIORITY     10
#define DEFAULT_PRIORITY 3
#define MIN_PRIORITY     0

typedef struct {
  priority priority;
  status processStatus;
  void *currentRSP;
} PCB;

/*
    @brief Creates a process stack
    @param argc The number of arguments
    @param argv The arguments
    @param rsp The current RSP
    @param start The start of the process
    @returns The process stack
*/
extern void *create_process_stack(int argc, const char *const argv[], void *rsp, ProcessStart start);

/*
    @brief Initializes the scheduler
*/
void init_scheduler();

/*
    @brief Checks if a process was killed
    @param pid The PID of the process
    @returns 1 if the process was killed, 0 otherwise
*/
int process_was_killed(pid pid);  // Scheduler actions when process is terminated

/*
    @brief Checks if a process was created
    @param pid The PID of the process
    @param argc The number of arguments
    @param argv The arguments
    @param priority The priority of the process
    @param entryPoint The entry point of the process
    @param currentRSP The current RSP
*/
int process_was_created(pid pid, int argc, const char *const argv[], priority priority, ProcessStart entryPoint,
                        void *currentRSP);

/*
    @brief Yields the CPU usage to the next process
*/
void yield();

/*
    @brief Blocks a process
    @param pid The PID of the process
    @returns 0 if successful, -1 if error
*/
int block(pid pid);

/*
    @brief Unblocks a process
    @param pid The PID of the process
    @returns 0 if successful, -1 if error
*/
int unblock(pid pid);

/*
    @brief Changes the priority of a process
    @param pid The PID of the process
    @param newPrio The new priority of the process
*/
int nice(pid pid, priority newPrio);

/*
    @brief Waits for children
*/
void wait_for_children();

/*
    @brief Waits for a specific process
    @param pid The PID of the process
*/
void wait_for_pid(pid pid);

/*
    @brief Kills the current process
    @returns 0 if successful, -1 if error
*/
int kill_current();

/*
    @brief Sets the priority of a process
    @param pid The PID of the process
    @param newPrio The new priority of the process
*/
int set_priority(pid pid, priority newPrio);

/*
    @brief Switches the process
    @param cRSP The current RSP
    @returns The new RSP
*/
void *switch_process(void *cRSP);

/*
    @brief Gets the process info
    @param pid The PID of the process
    @param processInfo The process info to be filled
    @returns 0 if successful, -1 if error
*/
int get_process_info(pid pid, ProcessInfo *processInfo);

/*
    @brief Gets the current process
    @returns The current process
*/
PCB *get_current_process();

/*
    @brief Gets the current process' PID
    @returns The current process' PID
*/
pid get_pid();

#endif