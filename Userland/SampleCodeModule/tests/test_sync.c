#include "test_util.h"
#include <commands.h>
#include <defs.h>
#include <libSysCalls.h>

/* Constants */
#define SEM_ID               "sem"
#define TOTAL_PAIR_PROCESSES 2

int64_t global;  // shared memory

void slowInc(int64_t *p, int64_t inc) {
  uint64_t aux = *p;
  aux += inc;
  yield();
  *p = aux;
}

void myProcessInc(int argc, char *argv[]) {
  uint64_t n;
  int8_t inc;
  int8_t use_sem;
  char buffer[20];

  if (argc != 3) {
    fprintf(STDERR,"Must receive three arguments: n, inc, useSem\n");
    return;
  }

  if ((n = satoi(argv[0])) <= 0)
    return;
  if ((inc = satoi(argv[1])) == 0)
    return;
  if ((use_sem = satoi(argv[2])) < 0)
    return;

  fprintf(STDOUT,"Process PID: ");
  itoa(getpid(), buffer, 10);
  fprintf(STDOUT,buffer);
  fprintf(STDOUT," starting with inc=");
  itoa(inc, buffer, 10);
  fprintf(STDOUT, buffer);
  fprintf(STDOUT,"\n");

  sem sem;

  if (use_sem) {
    fprintf(STDOUT,"PID ");
    itoa(getpid(), buffer, 10);
    fprintf(STDOUT, buffer);
    fprintf(STDOUT, ": Opening semaphore\n");
    if ((sem = sem_open(SEM_ID, 1)) < 0) {
      fprintf(STDERR,"testSync: ERROR opening semaphore\n");
      return;
    }
  } else {
    fprintf(STDOUT, "Not using sems\n");
  }

  uint64_t i;
  for (i = 0; i < n; i++) {
    if (use_sem) {
      fprintf(STDOUT,"PID ");
      itoa(getpid(), buffer, 10);
      fprintf(STDOUT,buffer);
      fprintf(STDOUT,": Waiting on semaphore\n");
      sem_wait(sem);
    }
    int64_t before = global;
    slowInc(&global, inc);
    int64_t after = global;

    fprintf(STDOUT,"PID ");
    itoa(getpid(), buffer, 10);
    fprintf(STDOUT,buffer);
    fprintf(STDOUT,": ");
    itoa(before, buffer, 10);
    fprintf(STDOUT,buffer);
    fprintf(STDOUT , " -> ");
    itoa(after, buffer, 10);
    fprintf(STDOUT,buffer);
    fprintf(STDOUT,"\n");

    if (use_sem)
      sem_post(sem);
  }

  if (use_sem)
    sem_close(sem);

  fprintf(STDOUT,"Process PID: ");
  itoa(getpid(), buffer, 10);
  fprintf(STDOUT,buffer);
  fprintf(STDOUT," finishing\n");
}

void testSync(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(STDERR,"testsync: usage: testsync [n] [use_sem]\n");
    return;
  }

  char bufferaa[20];
  fprintf(STDOUT,"Starting test with parameters:\n");
  fprintf(STDOUT,"Iterations: ");
  itoa(satoi(argv[0]), bufferaa, 10);
  fprintf(STDOUT,bufferaa);
  fprintf(STDOUT,"\nUse semaphore: ");
  itoa(satoi(argv[1]), bufferaa, 10);
  fprintf(STDOUT,bufferaa);
  fprintf(STDOUT,"\n");

  char *argvDec[] = {argv[0], "-1", argv[1], NULL};
  char *argvInc[] = {argv[0], "1", argv[1], NULL};

  int fg_flag = isForeground(getpid());

  createProcessInfo decInfo = {.name = "processDec",
                               .fg_flag = fg_flag,
                               .priority = DEFAULT_PRIORITY,
                               .start = (ProcessStart) myProcessInc,
                               .argc = 3,
                               .argv = (const char *const *) argvDec};

  createProcessInfo incInfo = {.name = "processInc",
                               .fg_flag = fg_flag,
                               .priority = DEFAULT_PRIORITY,
                               .start = (ProcessStart) myProcessInc,
                               .argc = 3,
                               .argv = (const char *const *) argvInc};

  global = 0;

  int pids[2 * TOTAL_PAIR_PROCESSES];

  int i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = createProcess(&decInfo);
    pids[i + TOTAL_PAIR_PROCESSES] = createProcess(&incInfo);
  }

  for (i = 0; i < 2 * TOTAL_PAIR_PROCESSES; i++) {
    char buffer[20];
    itoa(pids[i], buffer, 10);
    fprintf(STDOUT,"Created process with PID: ");
    fprintf(STDOUT,buffer);
    fprintf(STDOUT,"\n");
  }

  waitForChildren();  // this makes it wait until ALL children are dead
  fprintf(STDOUT,"Final value:");
  char buffer[300];
  itoa(global, buffer, 10);
  fprintf(STDOUT, buffer);
  fprintf(STDOUT,"\n");

  return;
}

void testNoSync(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(STDERR,"testsync: usage: testsync [n] [use_sem]\n");
    return;
  }

  char *argvDec[] = {argv[0], "-1", argv[1], NULL};
  char *argvInc[] = {argv[0], "1", argv[1], NULL};

  fprintf(STDOUT,"Starting test with parameters:\n");
  fprintf(STDOUT, argv[0]);
  fprintf(STDOUT,"\t");
  fprintf(STDOUT,argv[1]);
  fprintf(STDOUT,"\n");
  fprintf(STDOUT,"Iterations: ");
  fprintf(STDOUT,argv[0]);
  fprintf(STDOUT,"\nUse semaphore: ");
  fprintf(STDOUT,argv[1]);
  fprintf(STDOUT,"\n");

  int fg_flag = isForeground(getpid());

  createProcessInfo decInfo = {.name = "processDec",
                               .fg_flag = fg_flag,
                               .priority = DEFAULT_PRIORITY,
                               .start = (ProcessStart) myProcessInc,
                               .argc = 3,
                               .argv = (const char *const *) argvDec};

  createProcessInfo incInfo = {.name = "processInc",
                               .fg_flag = fg_flag,
                               .priority = DEFAULT_PRIORITY,
                               .start = (ProcessStart) myProcessInc,
                               .argc = 3,
                               .argv = (const char *const *) argvInc};

  global = 0;

  int pids[2 * TOTAL_PAIR_PROCESSES];

  int i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = createProcess(&decInfo);
    pids[i + TOTAL_PAIR_PROCESSES] = createProcess(&incInfo);
  }

  for (i = 0; i < 2 * TOTAL_PAIR_PROCESSES; i++) {
    char buffer[20];
    itoa(pids[i], buffer, 10);
    fprintf(STDOUT,"Created process with PID: ");
    fprintf(STDOUT,buffer);
    fprintf(STDOUT,"\n");
  }

  waitForChildren();  // this makes it wait until ALL children are dead
  fprintf(STDOUT,"Final value:");
  char buffer[300];
  itoa(global, buffer, 10);
  fprintf(STDOUT,buffer);
  fprintf(STDOUT,"\n");

  return;
}