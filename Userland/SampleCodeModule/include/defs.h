#ifndef _DEFS_
#define _DEFS_

#define MAX_PRIORITY     10
#define DEFAULT_PRIORITY 3
#define MIN_PRIORITY     0
#define NO_PROC          -1

#define MAX_PROCESSES   15
#define MAX_NAME_LENGTH 20

#include <stdint.h>
#include <unistd.h>

typedef void (*ProcessStart)(int argc, char *argv[]);
typedef int pid;
typedef int priority;
typedef enum { READY = 0, RUNNING = 1, BLOCKED = 2, KILLED = 3 } status;

typedef struct {
  pid pid;
  char name[MAX_NAME_LENGTH + 1];
  void *stackStart;
  void *stackEnd;
  int fg_flag;
  priority priority;
  status status;
  pid parent;
  unsigned int input;
  unsigned int output;
} ProcessInfo;

typedef struct {
  const char *name;
  ProcessStart start;
  int fg_flag;
  priority priority;
  int argc;
  const char *const *argv;
  unsigned int input;
  unsigned int output;
} createProcessInfo;

typedef struct memoryInfo {
  size_t totalSize;
  size_t freeSize;
  size_t allocatedSize;
} memoryInfo;

#define MAX_SEMAPHORES 255
#define MAX_SEM_LENGTH 16
#define MAX_SEMS_ERROR -2

#define INVALID_VALUE_ERROR -3

typedef int sem;
typedef char *sem_name;

#define STDIN  0
#define STDOUT 1
#define STDERR 2
#define KBDIN  3

typedef int fd;

#define INVALID_PIPE (-1)
#define NO_SPACE     (-2)
#define PIPE_ERROR   (-3)

typedef struct pipeInfo {
  unsigned int id;
  unsigned int amount;
  uint8_t eof;
} pipeInfo;

#define EOF (-1)

#endif