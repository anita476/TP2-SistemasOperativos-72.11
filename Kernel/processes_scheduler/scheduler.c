// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <lib.h>
#include <scheduler.h>
#include <videoDriver.h>

extern void *create_process_stack(int argc, const char *const argv[], void *rsp, ProcessStart start);
extern void int81();

int get_state(pid, PCB **pcb);
PCB *get_current_process();
int get_quantum(pid pid);
pid get_next_ready();

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

int process_was_created(pid pid, int argc, const char *const argv[], priority priority, ProcessStart entryPoint,
                      void *currentRSP) {
  if (priority < MIN_PRIORITY || priority > MAX_PRIORITY) {
    priority = DEFAULT_PRIORITY;
  }
  processTable[pid].processStatus = READY;
  processTable[pid].priority = priority;
  processTable[pid].currentRSP = create_process_stack(argc, argv, currentRSP, entryPoint);
  if (processTable[pid].currentRSP == NULL) {
    return -1;
  }
  return 0;
}

int process_was_killed(pid pid) {
  PCB *pcb;
  if (get_state(pid, &pcb)) {
    return 1;
  }
  if (pcb->processStatus == KILLED) {
    return 0;
  }
  pcb->processStatus = KILLED;
  pcb->currentRSP = NULL;
  if (currentPID == pid) {
    currentPID = NO_PROC;
  }
  return 0;
}

void wait_for_children() {
  block(currentPID);
  yield();
}

void wait_for_pid(pid pid) {
  PCB *pcb;
  while (get_state(pid, &pcb) == 0 && pcb->processStatus != KILLED) {
    block(get_pid());
    yield();
  }
}

void yield() {
  currentQuantum = 0;
  int81();
}

void *switch_process(void *cRSP) {
  if (currentPID == PID_KERNEL) {
    mainRSP = cRSP;
  }

  else if (currentPID >= 0) {
    processTable[currentPID].currentRSP = cRSP;
    if (processTable[currentPID].processStatus == RUNNING) {
      processTable[currentPID].processStatus = READY;
    }
  }
  if ((processTable[nextPID].currentRSP != NULL) && processTable[nextPID].processStatus == READY) {
    currentPID = nextPID;
    nextPID = NO_PROC;
    currentQuantum = get_quantum(currentPID);
  }

  else if ((processTable[currentPID].currentRSP == NULL) || (processTable[currentPID].processStatus != READY) ||
           (currentQuantum == 0)) {
    currentPID = get_next_ready();
    if (currentPID == PID_KERNEL) {
      currentQuantum = 0;
      return mainRSP;
    }
    currentQuantum = get_quantum(currentPID);
  }

  else {
    currentQuantum -= 1;
  }

  processTable[currentPID].processStatus = RUNNING;
  return processTable[currentPID].currentRSP;
}

int block(pid pid) {
  PCB *pcb;
  if (get_state(pid, &pcb)) {
    char buffer[10];
    int_to_str(pid, buffer, 10);
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
  if (get_state(pid, &pcb)) {
    return -1;
  }
  if (processTable[pid].processStatus == READY || processTable[pid].processStatus == RUNNING) {
    return 0;
  }
  processTable[pid].processStatus = READY;
  if (processTable[pid].priority >= (MAX_PRIORITY / 2)) {
    nextPID = pid;
  }
  return 0;
}

int nice(pid pid, priority newPrio) {
  PCB *pcb;
  if (get_state(pid, &pcb) < 0) {
    return -1;
  }

  if (newPrio < MIN_PRIORITY || newPrio > MAX_PRIORITY) {
    return -1;
  }

  pcb->priority = newPrio;

  if (newPrio >= (MAX_PRIORITY / 2) && pcb->processStatus == READY) {
    nextPID = pid;
  }

  return 0;
}

int get_quantum(pid currentPID) { return (QUANTUM + processTable[currentPID].priority); }

pid get_next_ready() {
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

int kill_current() {
  PCB *pcb = get_current_process();
  if (pcb == NULL) {
    return 1;
  }
  pcb->processStatus = KILLED;
  pcb->currentRSP = NULL;
  kill(currentPID);
  return 0;
}

PCB *get_current_process() {
  if (currentPID > 0 && processTable[currentPID].processStatus == RUNNING) {
    return &processTable[currentPID];
  }
  return NULL;
}

int get_process_info(pid pid, ProcessInfo *processInfo) {
  PCB *pcb;
  if (get_state(pid, &pcb)) {
    return -1;
  }
  processInfo->status = pcb->processStatus;
  processInfo->priority = pcb->priority;
  return 0;
}

int get_state(pid pid, PCB **pcb) {
  if (pid < 0 || pid >= MAX_PROCESSES || (processTable[pid].currentRSP == NULL)) {
    return -1;
  }
  *pcb = &processTable[pid];
  return 0;
}

pid get_pid() { return currentPID; }

int set_priority(pid pid, priority newPrio) {
  if (newPrio < MIN_PRIORITY || newPrio > MAX_PRIORITY) {
    return -1;
  }
  PCB *pcb;
  if (get_state(pid, &pcb)) {
    return -1;
  }
  pcb->priority = newPrio;
  return 0;
}
