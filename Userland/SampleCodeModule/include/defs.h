#ifndef _DEFS_
#define _DEFS_


#define FOREGROUND 1
#define BACKGROUND 0

#define MAX_PRIORITY 5
#define DEFAULT_PRIORITY 1
#define MIN_PRIORITY 0

#define MAX_PROCESSES 10
#define MAX_NAME_LENGTH 20
#define STACK_SIZE 4096


typedef void (*ProcessStart)(int argc, char *argv[]);
typedef int pid;
typedef int priority;
typedef enum status {
    READY, 
    RUNNING, 
    BLOCKED, 
    KILLED} status;
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


typedef struct {
    pid pid; 
    char* name; 
    void* stackStart; 
    void* stackEnd; 
    int fg_flag; // just call it isForeground for better readability? still ok i guess 
    char** argv; 
    int argc; 
    void** memory; // ?? 
    unsigned int memoryCount, memoryBufSize; // ?? 
    priority priority;
    status processStatus;
    void* processRSP;
    int remainingQuantum; 
} PCB;


#endif