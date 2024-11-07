#include <processes.h>
#include <scheduler.h>
#include <semaphores.h>
#include <sys/types.h>
#include <lib.h>
#include <videoDriver.h>

/* for critical region entering */
extern void acquire(uint8_t *lock);
extern void release(uint8_t *lock);

uint8_t lock = 1;  // global lock to access semaphore resources and to guarantee no two processes try to grab resources
                   // at the same time

typedef struct Semaphore {
  sem_name name;
  int sem_value;
  uint8_t lock;
  pid interestedProcesses[MAX_PROCESSES];   /* not necesary to use queues because max quantity of processes is 10, maybe we can chaange it later tho */
  int numberInterestedProcesses; 
  pid waitingProcesses[MAX_PROCESSES];
  int numberWaitingProcesses;
} Semaphore;

Semaphore semaphoreList[MAX_SEMAPHORES] = {{0}};
int active = 0;

static int grabSemaphore(sem sem) {
  acquire(&lock);

  if (sem < 0 || sem > MAX_SEMAPHORES) {
    release(&lock);
    return INVALID_VALUE_ERROR;
  }
  acquire(&(semaphoreList[sem].lock));
  release(&lock);
  return 0;
}

static int isInterested(sem sem, pid pid) {
    for (int i = 0; i < semaphoreList[sem].numberInterestedProcesses; i++) {
        if (semaphoreList[sem].interestedProcesses[i] == pid) {
            return 1;
        }
    }
    return 0;
}

int sem_open(sem_name semName, int initValue) {
    if (semName == NULL) {
        print(STDERR, "Invalid semaphore name\n");
        return INVALID_VALUE_ERROR;
    }

    pid currentPid = getpid();
    sem semId = sem_get(semName);

    if (semId == -1) {  // need to create sem
        acquire(&lock);  // Get global lock for creation

        print(STDERR, "Creating semaphore ");
        print(STDERR, semName);
        print(STDERR, "\n");

        if (active >= MAX_PROCESSES) {
            print(STDERR, "Error: Maximum semaphores reached\n");
            release(&lock);
            return MAX_SEMS_ERROR;
        }
        if (initValue < 0) {
            release(&lock);
            return INVALID_VALUE_ERROR;
        }

        for (int i = 0; i < MAX_SEMAPHORES; i++) {
            if (semaphoreList[i].numberInterestedProcesses == 0) {
                semaphoreList[i].name = malloc(strlen(semName) + 1);
                if (semaphoreList[i].name == NULL) {
                    release(&lock);
                    return -1;
                }
                if (strcpy(semaphoreList[i].name, semName) == NULL) {
                    free(semaphoreList[i].name);
                    release(&lock);
                    return -1;
                }
                semaphoreList[i].sem_value = initValue;
                semaphoreList[i].interestedProcesses[0] = currentPid;
                semaphoreList[i].numberInterestedProcesses = 1;
                semaphoreList[i].lock = 1;
                active++;
                release(&lock);
                return i;
            }
        }
        release(&lock);
        print(STDERR, "Error: No available semaphore slots\n");
        return -1;
    }

    if (grabSemaphore(semId) != 0) {
        return -1;
    }

    for (int i = 0; i < semaphoreList[semId].numberInterestedProcesses; i++) {
        if (semaphoreList[semId].interestedProcesses[i] == currentPid) {
            release(&(semaphoreList[semId].lock));
            return semId;
        }
    }

    if (semaphoreList[semId].numberInterestedProcesses < MAX_PROCESSES) {
        semaphoreList[semId].interestedProcesses[semaphoreList[semId].numberInterestedProcesses++] = currentPid;
        release(&(semaphoreList[semId].lock));
        return semId;
    }

    release(&(semaphoreList[semId].lock));
    print(STDERR, "Error: Maximum interested processes reached for semaphore\n");
    return -1;
}

int sem_close(sem sem) {
  if (grabSemaphore(sem) != 0) {
    return INVALID_VALUE_ERROR;
  }

  pid currentPid = getpid();
  int found = 0;

  for (int i = 0; i < semaphoreList[sem].numberInterestedProcesses; i++) {
    if (semaphoreList[sem].interestedProcesses[i] == currentPid) {
      found = 1;
      
      int currentCount = semaphoreList[sem].numberInterestedProcesses;

      
      if (currentCount == 1) {
        acquire(&lock);
        free(semaphoreList[sem].name);
        semaphoreList[sem].name = NULL;
        semaphoreList[sem].sem_value = 0;
        semaphoreList[sem].numberInterestedProcesses = 0;
        memset(semaphoreList[sem].interestedProcesses, 0, sizeof(pid) * MAX_PROCESSES);
        active--;
        release(&lock);
      } else {
        // Shift remaining processes left
        for (int j = i; j < semaphoreList[sem].numberInterestedProcesses - 1; j++) {
          semaphoreList[sem].interestedProcesses[j] = semaphoreList[sem].interestedProcesses[j + 1];
        }
        semaphoreList[sem].numberInterestedProcesses--;
      }
      break;
    }
  }

  release(&(semaphoreList[sem].lock));
  return found ? 0 : -1;
}


int sem_wait(sem sem) {   

    if (grabSemaphore(sem) != 0){
      print(STDERR, "COULDNT GRAB SEMAPHORE\n");
      return -1;
    }
        

    pid currentPid = getpid();
    char buf2[10];
    intToStr(currentPid, buf2, 10);
    print(STDERR, "PID: ");
    print(STDERR,buf2);
     print(STDERR, "Releasing sem\n"); 

    int found = isInterested(sem, currentPid);

    if (!found) {
        release(&(semaphoreList[sem].lock));
        print(STDERR, "Error: Process hasn't opened this semaphore therefore it cannot wait\n");
        return INVALID_VALUE_ERROR; 
    }

    while (semaphoreList[sem].sem_value <= 0) {
        semaphoreList[sem].waitingProcesses[semaphoreList[sem].numberWaitingProcesses++] = currentPid;
        release(&(semaphoreList[sem].lock));
        block(currentPid);
        yield();
        
        if (grabSemaphore(sem) != 0)
            return -1;
    }
    semaphoreList[sem].sem_value--;
    
    release(&(semaphoreList[sem].lock));
    return 0;
}


int sem_post(sem sem) {

   

    if (grabSemaphore(sem) != 0) {
        return -1;
    }

    pid currentPid = getpid();
    char buf[10];
    intToStr(currentPid, buf, 10);
    print(STDERR, "PID: ");
    print(STDERR, buf);
     print(STDERR, "Releasing sem\n"); 

    int found = isInterested(sem, currentPid);

    if (!found) {
        release(&(semaphoreList[sem].lock));
        print(STDERR, "Error: Process hasn't opened this semaphore therefore it cannot post\n");
        return INVALID_VALUE_ERROR; 
    }

    semaphoreList[sem].sem_value++;
    
    if (semaphoreList[sem].numberWaitingProcesses > 0) {
        // print(STDERR, "Posting semaphore\n");
        // Unblock first process in the list
        pid toUnblock = semaphoreList[sem].waitingProcesses[0];
        
        // Shift remaining processes
        for (int i = 0; i < semaphoreList[sem].numberWaitingProcesses - 1; i++) {
            semaphoreList[sem].waitingProcesses[i] = semaphoreList[sem].waitingProcesses[i + 1];
        }
        semaphoreList[sem].numberWaitingProcesses--;
        char buffer[20];
        print(STDERR, "Processes waiting:");
        for (int i = 0; i < semaphoreList[sem].numberWaitingProcesses; i++) {
            print(STDERR, "PID ");
            intToStr(semaphoreList[sem].waitingProcesses[i], buffer,10);
            print(STDERR, buffer);
            print(STDERR, " ");
        }
        print(STDOUT, "\n");
        
        unblock(toUnblock);
    }

    release(&semaphoreList[sem].lock);
    return 0;
}

int sem_value(sem sem) {
  if (sem < 0 || sem > MAX_PROCESSES) {
    return -1;
  }
  return semaphoreList[sem].sem_value;
}

sem sem_get(sem_name semName) {
  for (int i = 0; i < active; i++) {
    if (strcmp(semaphoreList[i].name, semName) == 0) {
      return i;  // number in list is the "id" we use
    }
  }
  return -1;
}
