#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "commonHeader.h"
#include "processes.h"

typedef struct {
    priority priority;
    status processStatus;
    void * currentRSP;
} PCB;

extern void * createProcessStack(int argc, const char *const argv[], void *rsp, ProcessStart start);

void init_scheduler();
int processWasKilled(pid pid); // Scheduler actions when process is terminated
int processWasCreated(pid pid, int argc, const char * const argv[], priority priority, ProcessStart entryPoint, void * currentRSP ); // Scheduler actions when process is created
void yield(); // Relinquish cpu usage to next process
int block(pid pid);
int unblock(pid pid);
int killCurrent();
int setPriority(pid pid, priority newPrio);
void * switchP(void * cRSP);

int getPInfo(pid pid, ProcessInfo * pInfo); // Get process info
pid getpid(); // Current process' pid




#endif