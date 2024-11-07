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
    pid parent;
    unsigned int memoryCount, memoryBufSize;
    unsigned int input;
    unsigned int output;
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
    pid parent;
    unsigned int input;
    unsigned int output;
} ProcessInfo;

/*what is needed in order to create a process*/ 
typedef struct{
    const char *name;
    ProcessStart start;
    int fg_flag;
    priority priority;
    int argc;
    const char *const *argv;
    unsigned int input;
    unsigned int output;
} createProcessInfo;

typedef struct{ /* not good if number of processes increases, but i think its fine for this project*/
    pid childrenArr[MAX_PROCESSES];
    int numberOfChildren;
} familyUnit; 

/* 
    @brief Creates a process
    @param info The process info
    @returns The PID of the process
*/
pid createProcess(createProcessInfo * info);

/* 
    @brief Kills a process
    @param pid The PID of the process
    @returns 0 if successful, -1 if error
*/
int kill(pid pid);

/* 
    @brief Kills a process with its children
    @param pid The PID of the process
    @returns 0 if successful, -1 if error
*/
int killWithChildren(pid pid);

/* 
    @brief Sends a process to the background
    @param pid The PID of the process
    @returns 0 if successful, -1 if error
*/
int sendToBackground(pid pid);

/* 
    @brief Brings a process to the foreground
    @param pid The PID of the process
    @returns 0 if successful, -1 if error
*/
int bringToForeground(pid pid);

/* 
    @brief Checks if a process is in the foreground
    @param pid The PID of the process
    @returns 1 if the process is in the foreground, 0 otherwise
*/
int isForeground(pid pid);

/* 
    @brief Lists the processes info
    @param processes The processes info
    @param maxProcesses The maximum number of processes
    @returns The number of processes listed
*/
int listProcessesInfo(ProcessInfo * processes, int maxProcesses);



int get_process_input(pid pid);

int get_process_output(pid pid);

#endif