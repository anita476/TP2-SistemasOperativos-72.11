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
    char buffer[20];

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

    print("Process PID: ");
    itoa(getpid(), buffer, 10);
    print(buffer);
    print(" starting with inc=");
    itoa(inc, buffer, 10);
    print(buffer);
    print("\n");


    sem sem;

    if (use_sem) {
        print("PID ");
        itoa(getpid(), buffer, 10);
        print(buffer);
        print(": Opening semaphore\n");
        if ((sem = sem_open(SEM_ID, 1)) < 0) {
            print("testSync: ERROR opening semaphore\n");
            return;
        }
    }
    else{
        print("Not using sems\n");
    }

    uint64_t i;
    for (i = 0; i < n; i++) {
        if (use_sem) {
            print("PID ");
            itoa(getpid(), buffer, 10);
            print(buffer);
            print(": Waiting on semaphore\n");
            sem_wait(sem);
        }
        int64_t before = global;
        slowInc(&global, inc);
        int64_t after = global;
        
        print("PID ");
        itoa(getpid(), buffer, 10);
        print(buffer);
        print(": ");
        itoa(before, buffer, 10);
        print(buffer);
        print(" -> ");
        itoa(after, buffer, 10);
        print(buffer);
        print("\n");

        if (use_sem)
            sem_post(sem);
    }

    if (use_sem)
        sem_close(sem);

    print("Process PID: ");
    itoa(getpid(), buffer, 10);
    print(buffer);
    print(" finishing\n");
}

void
testSync(int argc, char *argv[]) {
    if (argc != 2) {
        print("testsync: usage: testsync [n] [use_sem]\n");
        return;
    }

    char * argv2 [] = {"10","1"};
    char *argvDec[] = {argv2[0], "-1", argv2[1], NULL};
    char *argvInc[] = {argv2[0], "1", argv2[1], NULL};
    
    print("Starting test with parameters:\n");
    print(argv2[0]);
    print(argv2[1]);
    print("\n");
    print("Iterations: ");
    print(argv2[0]);
    print("\nUse semaphore: ");
    print(argv2[1]);
    print("\n");

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

    int pids[2 * TOTAL_PAIR_PROCESSES];

    int i;
    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        pids[i] =  createProcess(&decInfo);
        pids[i + TOTAL_PAIR_PROCESSES] = createProcess(&incInfo);
    }

    for (i = 0; i < 2 * TOTAL_PAIR_PROCESSES; i++) {
    char buffer[20];
    itoa(pids[i], buffer, 10);
    print("Created process with PID: ");
    print(buffer);
    print("\n");
}

    waitForChildren(); //this makes it wait until ALL children are dead
    print("Final value:");
    char buffer[300];
    itoa(global,buffer,10);
    print(buffer);
    print("\n");

    return;
}


void testNoSync(int argc, char *argv[]) {
    if (argc != 2) {
        print("testsync: usage: testsync [n] [use_sem]\n");
        return;
    }

    char * argv2 [] = {"10","0"};
    char *argvDec[] = {argv2[0], "-1", argv2[1], NULL};
    char *argvInc[] = {argv2[0], "1", argv2[1], NULL};
    
    print("Starting test with parameters:\n");
    print(argv2[0]);
    print("\t");
    print(argv2[1]);
    print("\n");
    print("Iterations: ");
    print(argv2[0]);
    print("\nUse semaphore: ");
    print(argv2[1]);
    print("\n");

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

    int pids[2 * TOTAL_PAIR_PROCESSES];

    int i;
    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        pids[i] =  createProcess(&decInfo);
        pids[i + TOTAL_PAIR_PROCESSES] = createProcess(&incInfo);
    }

    for (i = 0; i < 2 * TOTAL_PAIR_PROCESSES; i++) {
    char buffer[20];
    itoa(pids[i], buffer, 10);
    print("Created process with PID: ");
    print(buffer);
    print("\n");
    }

    waitForChildren(); //this makes it wait until ALL children are dead
    print("Final value:");
    char buffer[300];
    itoa(global,buffer,10);
    print(buffer);
    print("\n");

    return;
}