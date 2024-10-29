#include "test_util.h"
#include <defs.h>
#include <commands.h>
#include <libSysCalls.h>   


/* Constants */
#define SEM_ID               "sem"
#define TOTAL_PAIR_PROCESSES 2

int64_t global;  // shared memory

void
slowInc(int64_t *p, int64_t inc) {
    uint64_t aux = *p;
    aux += inc;
    yield();
    *p = aux;
}

void
myProcessInc(int argc, char *argv[]) {
    uint64_t n;
    int8_t inc;
    int8_t use_sem;

    if (argc != 3) {
        print("Must receive three arguments: n, inc, useSem\n");
        return;
    }

    if ((n = satoi(argv[0])) <= 0)
        return;
    if ((inc = satoi(argv[1])) == 0)
        return;
    if ((use_sem = satoi(argv[2])) < 0)
        return;

    


    sem sem;

    if (use_sem) {
        print("Using semaphores\n");
        if ((sem = sem_open(SEM_ID, 1)) < 0) {
            print("testSync: ERROR opening semaphore\n");
            return;
        }
    }

    uint64_t i;
    for (i = 0; i < n; i++) {
        if (use_sem)
            sem_wait(sem);
        slowInc(&global, inc);
        if (use_sem)
            sem_post(sem);
    }

    if (use_sem)
        sem_close(sem);
}

void
testSync(int argc, char *argv[]) {
    uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

    if (argc != 2) {
        print("testsync: usage: testsync [n] [use_sem]\n");
        return;
    }
    
    char * argv2 [] = {"1","1"};
    char *argvDec[] = {argv2[0], "-1", argv2[1], NULL};
    char *argvInc[] = {argv2[0], "1", argv2[1], NULL};

    createProcessInfo decInfo = {.name = "processDec",
                                 .fg_flag = 1,
                                 .priority = DEFAULT_PRIORITY,
                                 .start = (ProcessStart) myProcessInc,
                                 .argc = 3,
                                 .argv = (const char *const *) argvDec};

    createProcessInfo incInfo = {.name = "processInc",
                                 .fg_flag = 1,
                                 .priority = DEFAULT_PRIORITY,
                                 .start = (ProcessStart) myProcessInc,
                                 .argc = 3,
                                 .argv = (const char *const *) argvInc};

    global = 0;

    uint64_t i;
    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        pids[i] =  createProcess(&decInfo);
        pids[i + TOTAL_PAIR_PROCESSES] = createProcess(&incInfo);
    }
    waitForChildren(); //this makes it wait until ALL children are dead
    print("Final value:");
    char buffer[300];
    itoa(global,buffer,10);
    print(buffer);
    print("\n");

    return;
}