#include <semaphores.h>
#include <processes.h>
#include <scheduler.h>
#include <videoDriver.h>


/* Constants */
#define SEM_ID               "sem"
#define TOTAL_PAIR_PROCESSES 2

int64_t global;  // shared memory

void
slowInc(int64_t *p, int64_t inc) {
    uint64_t aux = *p;
    yield();  // This makes the race condition highly probable
    aux += inc;
    *p = aux;
}

waitpid(pid PID) {
    pid currentPid = getpid();
    if (PID == currentPid)
        return 1;

    block(currentPid);
    yield();
    return 0;
}

// Parameters
int64_t satoi(char *str) {
  uint64_t i = 0;
  int64_t res = 0;
  int8_t sign = 1;

  if (!str)
    return 0;

  if (str[i] == '-') {
    i++;
    sign = -1;
  }

  for (; str[i] != '\0'; ++i) {
    if (str[i] < '0' || str[i] > '9')
      return 0;
    res = res * 10 + str[i] - '0';
  }

  return res * sign;
}

void
myProcessInc(int argc, char *argv[]) {
    print("Process inc\n");
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
        if ((sem = sem_open(SEM_ID, 1)) < 0) {
            print("testSync: ERROR opening semaphore\n");
            return;
        }
    }
    char buff[10];
    intToStr(sem, buff,10);
    print("SEM FROM SEMOPEN: ");
    print(buff);
    print("\n");

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
    
    char * argv2 [] = {"3","1"};
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
        print("HLLO\n");
    }

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        waitpid(pids[i]) ;
        print("HI");
        waitpid(pids[i + TOTAL_PAIR_PROCESSES]);
        print("Hllo2\n");
    }

    print("Final value:");
    char buffer[300];
    intToStr(global,buffer,10);
    print(buffer);
    print("\n");

    return;
}