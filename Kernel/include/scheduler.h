#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stdlib.h>
#include <memoryManagement.h>
#include <lib.h>
#include <processes.h>

#define QUANTUM  5 //in ms
#define NO_PROC -2 
#define PID_KERNEL -1


#define MAX_PRIORITY 5
#define DEFAULT_PRIORITY 1
#define MIN_PRIORITY 0


typedef struct {
    priority priority;
    status processStatus;
    void * currentRSP;
} PCB;

extern void * createProcessStack(int argc, const char *const argv[], void *rsp, ProcessStart start);

void init_scheduler();
int processWasKilled(pid pid); //scheduler actions when process is terminated
int processWasCreated(pid pid, int argc, const char * const argv[], priority priority, ProcessStart entryPoint, void * currentRSP ); // scheduler actions when process is created
void yield(); //relinquish cpu usage to next process
int block(pid pid);
int unblock(pid pid);
int killCurrent();
int setPriority(pid pid, priority newPrio);
void * switchP(void * cRSP);

int getPInfo(pid pid, ProcessInfo * pInfo); // get process info
pid getpid(); // current process' pid




#endif