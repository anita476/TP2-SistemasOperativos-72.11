/***************************************************
  Defs.h
****************************************************/

#ifndef _defs_
#define _defs_

/* Flags para derechos de acceso de los segmentos */
#define ACS_PRESENT     0x80            /* segmento presente en memoria */
#define ACS_CSEG        0x18            /* segmento de codigo */
#define ACS_DSEG        0x10            /* segmento de datos */
#define ACS_READ        0x02            /* segmento de lectura */
#define ACS_WRITE       0x02            /* segmento de escritura */
#define ACS_IDT         ACS_DSEG
#define ACS_INT_386 	  0x0E		        /* Interrupt GATE 32 bits */
#define ACS_INT         (ACS_PRESENT | ACS_INT_386)

#define ACS_CODE        (ACS_PRESENT | ACS_CSEG | ACS_READ)
#define ACS_DATA        (ACS_PRESENT | ACS_DSEG | ACS_WRITE)
#define ACS_STACK       (ACS_PRESENT | ACS_DSEG | ACS_WRITE)


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