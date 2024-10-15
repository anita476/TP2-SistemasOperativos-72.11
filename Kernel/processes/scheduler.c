// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "scheduler.h"

PriorityQueue* readyQueue; 
PCB processTable[MAX_PROCESSES];
pid currentPID = NO_PROC; 
PCB* idle = NULL;

extern void* createProcessStack(int argc, const char *const argv[], void *rsp, ProcessStart start);
extern void int81();

void init_scheduler() {
    readyQueue = createPriorityQueue();
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processTable[i].pid = NO_PROC;
        processTable[i].processStatus = KILLED;
    }

    // createProcessInfo idleInfo = {.name = "idle",
    //                                  .fg_flag = 1,
    //                                  .priority = MIN_PRIORITY,
    //                                  .start = (ProcessStart) idleProcess,
    //                                  .argc = 0,
    //                                  .argv = (const char * const *) NULL};
    // currentPID = createProcess(&idleInfo);
    // idle = &processTable[currentPID];
    // idle->processStatus = RUNNING;
}

pid createProcess(createProcessInfo *info) {
    pid pid = 0;

    for (; pid < MAX_PROCESSES && processTable[pid].stackEnd != NULL; pid++);

    if (pid >= MAX_PROCESSES || info->argc < 0 || info->name == NULL) {
        print("Failed to create process: Invalid parameters\n");
        return -1;
    }

    PCB *process = &processTable[pid];
    process->pid = pid;
    process->priority = info->priority;
    process->processStatus = READY;
    process->remainingQuantum = BASE_QUANTUM + process->priority;
    process->stackEnd = malloc(STACK_SIZE);
    if (process->stackEnd == NULL) {
        print("Failed to allocate stack\n");
        return -1;
    }
    process->processRSP = createProcessStack(info->argc, info->argv, process->stackEnd + STACK_SIZE, info->start);
    process->fg_flag = info->fg_flag;

    if (pid != 0) {
        priorityQueueInsert(readyQueue, process->priority, pid);
    }

    return pid;
}

int kill(pid pid) {
    if (pid < 0 || pid >= MAX_PROCESSES || processTable[pid].processStatus == KILLED) {
        print("Invalid PID or Process already terminated\n");
        return ERROR;
    }

    PCB *process = &processTable[pid];
    process->processStatus = KILLED;
    free(process->stackEnd);

    if (process->processStatus == READY) {
        priorityQueueRemove(readyQueue, pid);
    }

    if (pid == currentPID) {
        yield();  // Force a context switch if the current process is killed
    }

    return SUCCESS;
}

int processWasCreated(pid pid, int argc, const char* const argv[], priority priority, ProcessStart entryPoint, void* processRSP) {
    if (pid < 0 || pid >= MAX_PROCESSES) {
        print("Error: Invalid PID\n");
        return ERROR;
    }

    if (priority < MIN_PRIORITY || priority > MAX_PRIORITY) {
        print("Priority out of bounds: switching to default priority 5\n");
        priority = DEFAULT_PRIORITY;
    }

    PCB* pcb = &processTable[pid];
    pcb->pid = pid; 
    pcb->priority = priority;
    pcb->processStatus = READY;
    pcb->processRSP = createProcessStack(argc, argv, processRSP, entryPoint);
    pcb->remainingQuantum = BASE_QUANTUM + priority; // ??? not sure about this one 

    if (pid != 0) {  // Don't add idle process to ready queue
        priorityQueueInsert(readyQueue, priority, pid);
    }

    return SUCCESS;
}

int processWasKilled(pid pid) {
    if (pid < 0 || pid >= MAX_PROCESSES) {
        print("Error: Invalid PID\n");
        return ERROR;
    }

    PCB* pcb = &processTable[pid];
    if (pcb->processStatus == KILLED) {
        print("Process already terminated\n");
        return SUCCESS; 
    } 

    pcb->processStatus = KILLED;
    pcb->processRSP = NULL; 

    if (pid == currentPID) {
        currentPID = NO_PROC;
        yield(); // Force a context switch if the process is killed
    } else if (pcb->processStatus == READY) {
        priorityQueueRemove(readyQueue, pid);
    }
    return SUCCESS;

}

void yield() {
    if (currentPID != NO_PROC && currentPID != PID_KERNEL) {
        PCB *current = &processTable[currentPID];
        current->remainingQuantum = 0;
    }
    int81();
}



// void* switchP(void *cRSP) {

//     // dentro del timer handler o en algun lugar deberia decrementar el quantum del proceso actual y si el current process quantum es menor a la cantidad de ticks deberia despertar otro proceso y sacarlo de la lista de readys
//     if (currentPID != NO_PROC && currentPID != PID_KERNEL) {
//         PCB* currentProcess = &processTable[currentPID];
//         currentProcess->processRSP = cRSP;
//         if (currentProcess->processStatus == RUNNING) {
//             currentProcess->processStatus = READY;
//             if (currentProcess->pid != 0) {  // Don't requeue idle process
//                 priorityQueueInsert(readyQueue, currentProcess->priority, currentProcess->pid);
//             }        
//         }
//     }

//     if (priorityQueueIsEmpty(readyQueue)) {
//         currentPID = 0; // run idle process
//         processTable[0].processStatus = RUNNING;
//         return processTable[0].processRSP;
//     }

//     currentPID = priorityQueuePop(readyQueue);
//     PCB* next = &processTable[currentPID];
//     next->processStatus = RUNNING;
//     if (next->remainingQuantum <= 0) {
//         next->remainingQuantum = BASE_QUANTUM + next->priority;
//     }
//     return next->processRSP;

// }
void* switchP(void *cRSP) {
    if (currentPID != NO_PROC && currentPID != PID_KERNEL) {
        PCB *currentProcess = &processTable[currentPID];
        currentProcess->processRSP = cRSP;
        
        // Decrement quantum
        if (currentProcess->remainingQuantum > 0) {
            currentProcess->remainingQuantum--;
        }
        
        if (currentProcess->remainingQuantum > 0 && currentProcess->processStatus == RUNNING) {
            // Continue running the current process
            return cRSP;
        }

        // Time to switch: reset quantum and change status
        currentProcess->remainingQuantum = BASE_QUANTUM + currentProcess->priority;
        if (currentProcess->processStatus == RUNNING) {
            currentProcess->processStatus = READY;
            if (currentProcess->pid != idle->pid) {
                priorityQueueInsert(readyQueue, currentProcess->priority, currentProcess->pid);
            }
        }
    }

    if (priorityQueueIsEmpty(readyQueue)) {
        currentPID = idle->pid;
        idle->processStatus = RUNNING;
        idle->remainingQuantum = BASE_QUANTUM + idle->priority;
        return idle->processRSP;
    }

    currentPID = priorityQueuePop(readyQueue);
    PCB *nextProcess = &processTable[currentPID];
    nextProcess->processStatus = RUNNING;
    nextProcess->remainingQuantum = BASE_QUANTUM + nextProcess->priority;

    return nextProcess->processRSP;
}

int block(pid pid) {
    if (pid < 0 || pid >= MAX_PROCESSES) return ERROR;
    
    PCB* pcb = &processTable[pid];
    if (pcb->processStatus != RUNNING && pcb->processStatus != READY) {
        print("Process cannot be blocked since it is not running or ready\n");
        return ERROR;
    }

    pcb->processStatus = BLOCKED;
    if (pid != currentPID) {
        priorityQueueRemove(readyQueue, pid);
    }
    return SUCCESS;
}

int unblock(pid pid) {
    if (pid < 0 || pid >= MAX_PROCESSES) return 1;
    
    PCB* pcb = &processTable[pid];
    if (pcb->processStatus != BLOCKED) return 1;

    pcb->processStatus = READY;
    pcb->priority = MAX_PRIORITY; // Increases priority when unblocking
    priorityQueueInsert(readyQueue, pcb->priority, pid);
    return 0;

}

int setPriority(pid pid, int newPriority) {
    if (pid < 0 || pid >= MAX_PROCESSES) return 1;
    if (newPriority < MIN_PRIORITY || newPriority > MAX_PRIORITY) return 1;

    PCB* pcb = &processTable[pid];
    pcb->priority = newPriority;

    if (pcb->processStatus == READY) {
        priorityQueueRemove(readyQueue, pid);
        priorityQueueInsert(readyQueue, newPriority, pid);
    }
    return 0;
}

pid getpid() {
    return currentPID;
}

int getPInfo(pid pid, ProcessInfo* pInfo) {
    if (pid < 0 || pid >= MAX_PROCESSES) return 1;
    
    PCB* pcb = &processTable[pid];
    pInfo->pid = pcb->pid;
    pInfo->priority = pcb->priority;
    pInfo->status = pcb->processStatus;
    // pInfo->processRSP = pcb->processRSP;
    print("------------------Process info:------------------ \n");
    print("PID: ");
    print("Name: ");
    print("Priority: ");
    print("Status: ");
    return 0;
}

static void idleProcess() {
    while(1) {
        _hlt();
    }
}

void schedulerTick() {
    if (currentPID != NO_PROC && currentPID != PID_KERNEL && currentPID != idle->pid) {
        PCB *current = &processTable[currentPID];
        current->remainingQuantum--;

        if (current->remainingQuantum <= 0) {
            // print("No quantum\n");
            // current->processStatus = READY;
            // priorityQueueInsert(readyQueue, current->priority, currentPID);
            yield();
        }
    }
}