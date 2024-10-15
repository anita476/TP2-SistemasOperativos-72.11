#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stdlib.h>
#include "defs.h"
#include "memoryManagement.h"
#include "lib.h"
#include "videoDriver.h"
#include "priorityQueue.h"

// #include <processes.h>

// #define MAX_PROCESSES 256

#define BASE_QUANTUM 100 //In ms
#define NO_PROC -2 
#define PID_KERNEL -1


// #define MAX_PRIORITY 10
// #define DEFAULT_PRIORITY 5
// #define MIN_PRIORITY 0

// #define ERROR 1 
#define SUCCESS 0 
// typedef struct {
//     pid_t pid; 
//     priority priority;
//     status processStatus;
//     void * processRSP;
//     int remainingQuantum;
// } PCB;

extern void * createProcessStack(int argc, const char *const argv[], void *rsp, ProcessStart start);

void init_scheduler();
int processWasKilled(pid pid); // Scheduler actions when process is terminated
int processWasCreated(pid pid, int argc, const char * const argv[], priority priority, ProcessStart entryPoint, void * processRSP ); // Scheduler actions when process is created
void yield(); // Relinquish cpu usage to next process
int block(pid pid);
int unblock(pid pid);
priority setPriority(pid pid, priority newPrio);
void * switchP(void * cRSP);

int killForeground(); // it kills the foreground process except the shell


int getPInfo(pid pid, ProcessInfo * pInfo); // Get process info
pid getpid(); // Current process' pid

static void idleProcess();

void schedulerTick();

pid createProcess(createProcessInfo *info);

int kill(pid pid);

#endif