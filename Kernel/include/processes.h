#ifndef PROCESSES_H
#define PROCESSES_H

#include <stdint.h>
#include <stdlib.h>
#include "defs.h"
#include "lib.h"
#include "memoryManagement.h"
#include "videoDriver.h"


/* what is saved */ /* priority is saved why scheduling queue*/
// typedef struct {
//     pid pid; 
//     char * name;
//     void * stackStart;
//     void * stackEnd;
//     int fg_flag;
//     char ** argv;
//     int argc;
//     void **memory;
//     unsigned int memoryCount, memoryBufSize;
// } ProcessS;

// pid createProcess(void* start, char* name, int argc, char* argv[], int fg_flag);
// what about priority? 
pid createProcess(createProcessInfo * info);

int sendToBackground(pid pid);

int bringToForeground(pid pid);

int isForeground(pid pid);

int listProcessesInfo(ProcessInfo * processes, int max_processes);

void exit(); // finishes process execution

void printProcessesInfo(); // prints all process info

int kill(pid pid);

void * createProcessStack(int argc, const char *const argv[], void *rsp, ProcessStart start);

#endif