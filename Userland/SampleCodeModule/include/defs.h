#ifndef _DEFS_
#define _DEFS_

#define MAX_PRIORITY 5
#define DEFAULT_PRIORITY 1
#define MIN_PRIORITY 0

#define MAX_PROCESSES 10
#define MAX_NAME_LENGTH 20

typedef void (*ProcessStart)(int argc, char *argv[]);
typedef int pid;
typedef int priority;
typedef enum {READY = 0, RUNNING = 1, BLOCKED = 2, KILLED= 3} status;

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

#endif