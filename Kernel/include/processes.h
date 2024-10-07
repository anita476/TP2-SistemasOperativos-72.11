#ifndef PROCESSES_H
#define PROCESSES_H

#include <stdint.h>
#include <stdlib.h>
#include <lib.h>
#include <memoryManagement.h>

#define FOREGROUND 1
#define BACKGROUND 0


#define MAX_PROCESSES 10
#define MAX_NAME_LENGTH 20
#define STACK_SIZE 4096

typedef void (*ProcessStart)(int argc, char *argv[]);

typedef int pid;
typedef int priority;
typedef enum {READY = 0, RUNNING = 1, BLOCKED = 2, KILLED= 3} status;

/* what is saved */ /* priority is saved why scheduling queue*/
typedef struct {
    char * name;
    void * stackStart;
    void * stackEnd;
    int fg_flag;
    char ** argv;
    int argc;
    void **memory;
    unsigned int memoryCount, memoryBufSize;
} ProcessS;

/* what is shown when requested*/
typedef struct{
    pid pid;
    char name[MAX_NAME_LENGTH + 1];
    void * stackStart;
    void * stackEnd;
    int fg_flag;
    priority priority;
    status status;

} ProcessInfo;

/*what is needed in order to create a process -> maybe it would be better passed as only args idk*/ 
typedef struct{
    const char *name;
    ProcessStart start;
    int fg_flag;
    priority priority;
    int argc;
    const char *const *argv;
} createProcessInfo;

pid createProcess(createProcessInfo * info);

int kill(pid pid);

int sendToBackground(pid pid);

int bringToForeground(pid pid);

int isForeground(pid pid);

int listProcessesInfo(ProcessInfo * processes, int max_processes);

#endif