// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <lib.h>
#include <scheduler.h>
#include <videoDriver.h>

extern void *createProcessStack(int argc, const char *const argv[], void *rsp, ProcessStart start);
extern void int81();

int getState(pid, PCB **pcb);
PCB *getCurrentProcess();
int getQuantum(pid pid);
pid getNextReady();

static PCB processTable[MAX_PROCESSES];
static uint8_t currentQuantum;
static pid currentPID;
static pid nextPID;

static void *mainRSP;

void init_scheduler() {
  nextPID = NO_PROC;
  currentPID = PID_KERNEL;
  currentQuantum = 0;
}

int processWasCreated(pid pid, int argc, const char *const argv[], priority priority, ProcessStart entryPoint,
                      void *currentRSP) {
  if (priority < MIN_PRIORITY || priority > MAX_PRIORITY) {
    priority = DEFAULT_PRIORITY;
  }
  processTable[pid].processStatus = READY;
  processTable[pid].priority = priority;
  processTable[pid].currentRSP = createProcessStack(argc, argv, currentRSP, entryPoint);
  if (processTable[pid].currentRSP == NULL) {
    return -1;
  }
  return 0;
}

int processWasKilled(pid pid) {
  PCB *pcb;
  if (getState(pid, &pcb)) {
    return 1;
  }
  if (pcb->processStatus == KILLED) {
    return 0;
  }
  pcb->processStatus = KILLED;
  pcb->currentRSP = NULL;

  // If the process running is the one terminated
  if (currentPID == pid) {
    currentPID = NO_PROC;
  }
  return 0;
}
/* make current proc wait for all children */
void waitForChildren() {
  block(currentPID);
  yield();
}
void yield() {
  currentQuantum = 0;
  int81();
}

void *switchP(void *cRSP) {
  // If in kernel
  if (currentPID == PID_KERNEL) {
    mainRSP = cRSP;
  }

  // If in a "normal process"
  else if (currentPID >= 0) {
    processTable[currentPID].currentRSP = cRSP;
    if (processTable[currentPID].processStatus == RUNNING) {
      processTable[currentPID].processStatus = READY;
    }
  }
  if ((processTable[nextPID].currentRSP != NULL) && processTable[nextPID].processStatus == READY) {
    currentPID = nextPID;
    nextPID = NO_PROC;
    // Assign how much more time based on process priority
    currentQuantum = getQuantum(currentPID);
  }

  else if ((processTable[currentPID].currentRSP == NULL) || (processTable[currentPID].processStatus != READY) ||
           (currentQuantum == 0)) {
    currentPID = getNextReady();
    if (currentPID == PID_KERNEL) {
      currentQuantum = 0;
      return mainRSP;
    }
    currentQuantum = getQuantum(currentPID);
  }

  // Keep running the same procs
  else {
    currentQuantum -= 1;
  }

  processTable[currentPID].processStatus = RUNNING;
  return processTable[currentPID].currentRSP;
}

int block(pid pid) {
  PCB *pcb;
  if (getState(pid, &pcb)) {
    char buffer[10];
    intToStr(pid, buffer, 10);
    print(STDERR, "For pid: ");
    print(STDERR, buffer);

    print(STDERR, " Get state failed\n");
    return -1;
  }
  processTable[pid].processStatus = BLOCKED;
  if (currentPID == PID_KERNEL) {
    currentQuantum = 0;
  }
  return 0;
}

int unblock(pid pid) {
  PCB *pcb;
  if (getState(pid, &pcb)) {
    return -1;
  }
  if (processTable[pid].processStatus == READY || processTable[pid].processStatus == RUNNING) {
    return 0;
  }
  processTable[pid].processStatus = READY;
  // If its of "high priority", run it next
  if (processTable[pid].priority >= (MAX_PRIORITY / 2)) {
    nextPID = pid;
  }
  return 0;
}

int nice(pid pid, priority newPrio) {
  PCB *pcb;
  if (getState(pid, &pcb)) {
    return -1;
  }

  if (newPrio < MIN_PRIORITY || newPrio > MAX_PRIORITY) {
    return -1;
  }

  pcb->priority = newPrio;

  // If it is a high priority process, make it run next
  if (newPrio >= (MAX_PRIORITY / 2) && pcb->processStatus == READY) {
    nextPID = pid;
  }

  return 0;
}

// Assigns time based on priority -> lower priority means less time
int getQuantum(pid currentPID) { return (QUANTUM + processTable[currentPID].priority); }

// Order remains the same, what changes is the amount of time a process will be allowed to run
pid getNextReady() {
  pid first = currentPID < 0 ? 0 : currentPID;
  pid next = first;
  do {
    next = (next + 1) % MAX_PROCESSES;
    if (next >= 0 && processTable[next].currentRSP != NULL && processTable[next].processStatus == READY) {
      return next;
    }
  } while (next != first);

  return PID_KERNEL;
}

int killCurrent() {
  PCB *pcb = getCurrentProcess();
  if (pcb == NULL) {
    return 1;
  }
  pcb->processStatus = KILLED;
  pcb->currentRSP = NULL;
  kill(currentPID);
  return 0;
}

PCB *getCurrentProcess() {
  if (currentPID > 0 && processTable[currentPID].processStatus == RUNNING) {
    return &processTable[currentPID];
  }
  return NULL;
}

int getProcessInfo(pid pid, ProcessInfo *processInfo) {
  PCB *pcb;
  if (getState(pid, &pcb))
    return 1;
  processInfo->status = pcb->processStatus;
  processInfo->priority = pcb->priority;
  return 0;
}

// Same as get process by pid -> if it exists return its struct
int getState(pid pid, PCB **pcb) {
  if (pid < 0 || pid >= MAX_PROCESSES || (processTable[pid].currentRSP == NULL)) {
    return 1;
  }
  *pcb = &processTable[pid];
  return 0;
}

pid getpid() { return currentPID; }

int setPriority(pid pid, priority newPrio) {
  if (newPrio < MIN_PRIORITY || newPrio > MAX_PRIORITY) {
    return -1;
  }
  PCB *pcb;
  if (getState(pid, &pcb)) {
    return -1;
  }
  pcb->priority = newPrio;
  return 0;
}
