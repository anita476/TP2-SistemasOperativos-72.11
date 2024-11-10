// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <lib.h>
#include <processes.h>
#include <scheduler.h>
#include <semaphores.h>
#include <sys/types.h>
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
  pid interestedProcesses[MAX_PROCESSES]; /* not necesary to use queues because max quantity of processes is 10, maybe
                                             we can chaange it later tho */
  int numberInterestedProcesses;
  pid waitingProcesses[MAX_PROCESSES];
  int numberWaitingProcesses;
} Semaphore;

Semaphore semaphoreList[MAX_SEMAPHORES] = {{0}};
int active = 0;

static int grab_semaphore(sem sem) {
  acquire(&lock);

  if (sem < 0 || sem >= MAX_SEMAPHORES) {
    release(&lock);
    return INVALID_VALUE_ERROR;
  }
  acquire(&(semaphoreList[sem].lock));
  release(&lock);
  return 0;
}

static int is_interested(sem sem, pid pid) {
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

  pid currentPid = get_pid();
  sem semId = sem_get(semName);

  if (semId == -1) {  // need to create sem
    acquire(&lock);   // Get global lock for creation

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
        strcpy(semaphoreList[i].name, semName);
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

  if (grab_semaphore(semId) != 0) {
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
  if (grab_semaphore(sem) != 0) {
    return INVALID_VALUE_ERROR;
  }

  pid currentPid = get_pid();
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

  if (grab_semaphore(sem) != 0) {
    print(STDERR, "COULDNT GRAB SEMAPHORE\n");
    return -1;
  }

  pid currentPid = get_pid();
  int found = is_interested(sem, currentPid);

  if (!found) {
    release(&(semaphoreList[sem].lock));
    //print(STDERR, "Error: Process hasn't opened this semaphore therefore it cannot wait\n");
    return INVALID_VALUE_ERROR;
  }

  while (semaphoreList[sem].sem_value <= 0) {
    semaphoreList[sem].waitingProcesses[semaphoreList[sem].numberWaitingProcesses++] = currentPid;
    release(&(semaphoreList[sem].lock));
    block(currentPid);
    yield();

    if (grab_semaphore(sem) != 0)
      return -1;
  }
  semaphoreList[sem].sem_value--;

  release(&(semaphoreList[sem].lock));
  return 0;
}

int sem_post(sem sem) {

  if (grab_semaphore(sem) != 0) {
    return -1;
  }

  pid currentPid = get_pid();
  int found = is_interested(sem, currentPid);

  if (!found) {
    release(&(semaphoreList[sem].lock));
    //print(STDERR, "Error: Process hasn't opened this semaphore therefore it cannot post\n");
    return INVALID_VALUE_ERROR;
  }

  semaphoreList[sem].sem_value++;

  if (semaphoreList[sem].numberWaitingProcesses > 0) {
    // Unblock first process in the list
    pid toUnblock = semaphoreList[sem].waitingProcesses[0];

    // Shift remaining processes
    for (int i = 0; i < semaphoreList[sem].numberWaitingProcesses - 1; i++) {
      semaphoreList[sem].waitingProcesses[i] = semaphoreList[sem].waitingProcesses[i + 1];
    }
    semaphoreList[sem].numberWaitingProcesses--;
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
