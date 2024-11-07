#ifndef _DEFS_
#define _DEFS_

#define MAX_PRIORITY 10
#define DEFAULT_PRIORITY 3
#define MIN_PRIORITY 0
#define NO_PROC -1

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
    pid parent;
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


/* Semaphore related defs*/
#define MAX_SEMAPHORES 255
#define MAX_SEM_LENGTH 16
#define MAX_SEMS_ERROR -2

#define INVALID_VALUE_ERROR -3

typedef int sem; /* sem ids can be a max of 255 */
typedef char * sem_name;



/* Defines for file descriptors */
#define STDIN  0
#define STDOUT 1
#define STDERR 2
#define KBDIN  3


typedef int fd;

#endif