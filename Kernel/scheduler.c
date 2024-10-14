#include <scheduler.h>
#include <videoDriver.h>
#include <lib.h>

int getState(pid, PCB ** pcb);
int getQuantum(pid pid);
pid getNextReady();
PCB * getCurrentProcess();

extern void * createProcessStack(int argc, const char *const argv[], void *rsp, ProcessStart start);
extern void int81();

static PCB processTable[MAX_PROCESSES];
static pid currentPID;
static pid nextPID;
static uint8_t currentQuantum;

extern void haltcpu();

static void * mainRSP;

void init_scheduler(){
    nextPID = NO_PROC;
    currentPID = PID_KERNEL;
    currentQuantum = 0;
}

int processWasCreated(pid pid, int argc, const char * const argv[], priority priority, ProcessStart entryPoint, void * currentRSP ){
    if(priority < MIN_PRIORITY || priority > MAX_PRIORITY){
        priority = DEFAULT_PRIORITY;
    }
    processTable[pid].processStatus = READY;
    processTable[pid].priority = priority;
    processTable[pid].currentRSP = createProcessStack(argc, argv, currentRSP, entryPoint);
    if(processTable[pid].currentRSP == NULL){
        return 1;
    }
    return 0;
}


int processWasKilled(pid pid){
    PCB * pcb;
    if(getState(pid, &pcb)){
        return 1;
    }
    if(pcb->processStatus == KILLED){
        return 0;
    }
    pcb->processStatus = KILLED;
    pcb->currentRSP = NULL; 

    // if the process running is the one terminated
    if(currentPID == pid){
        currentPID = NO_PROC;
    }
    return 0;
}

void yield(){
    currentQuantum = 0;
    int81();
}

void * switchP(void *cRSP) {
    // If im in kernel:
    if (currentPID == PID_KERNEL) {
        //print("KERNEL\n");
        mainRSP = cRSP;
    }

    // If im in a "normal process"
    else if (currentPID >= 0) {
        processTable[currentPID].currentRSP = cRSP;
        if(processTable[currentPID].processStatus == RUNNING){
            processTable[currentPID].processStatus = READY;

        }
    }
    if((processTable[nextPID].currentRSP != NULL) && processTable[nextPID].processStatus == READY){ //it never gets here!!
    //next never changes, its always NO_PROC -> FIX ASAP
        print("Next pid RSP is dif from null\n");
        currentPID = nextPID;
        nextPID = NO_PROC;
        //print("More time?\n");
        //assign how much more time based on process priority
        currentQuantum = getQuantum(currentPID);
    }
    else if ( (processTable[currentPID].currentRSP == NULL) || (processTable[currentPID].processStatus != READY) || (currentQuantum == 0)){
        currentPID = getNextReady();
        if(currentPID == PID_KERNEL){
            currentQuantum = 0;
            return mainRSP;
        }
        currentQuantum = getQuantum(currentPID);
    }
    else{
        //keep running the same procs
        currentQuantum -= 1;
    }
     processTable[currentPID].processStatus = RUNNING;
/*   print("Current pid: ");
    char buf[10];
    intToStr(currentPID, buf,10);
    print(buf);
    intToStr(currentQuantum,buf,10);
    print("Quantum:");
    print(buf);
    print("\n");  */
    return processTable[currentPID].currentRSP;
}

int block(pid pid){
    PCB * pcb;
    if(getState(pid, &pcb)){
        print("Get state failed\n");
        return 1;
    }
    processTable[pid].processStatus = BLOCKED;
    if(currentPID == PID_KERNEL){
        currentQuantum = 0;
    }
    return 0;
}

int unblock(pid pid){
    PCB * pcb;
    if(getState(pid, &pcb)){
        return 1;
    }
    if(processTable[pid].processStatus == READY || processTable[pid].processStatus == RUNNING){
        return 0;
    }
    processTable[pid].processStatus = READY;
    //if its of "high priority" -> run it next
    if( processTable[pid].priority >= (MAX_PRIORITY /2) ){
        nextPID = pid;
    }
    return 0;
}

int getQuantum(pid currentPID){ //asigns time based on priority
    return (MAX_PRIORITY - processTable[currentPID].priority);
}

pid getNextReady(){ //todo fix -> ASAP
    pid first = currentPID < 0 ? 0 : currentPID;
    pid next = first;
    do {
        next = (next + 1) % MAX_PROCESSES;
        if (next>= 0 && processTable[next].currentRSP != NULL && processTable[next].processStatus == READY) {
            return next;
        }
    } while (next != first);

    return PID_KERNEL;
}

int killCurrent(){
    PCB * pcb = getCurrentProcess();
    if(pcb == NULL){
        return 1;
    }   
    pcb->processStatus = KILLED;
    pcb->currentRSP = NULL;
    kill(currentPID);
    return 0;
}

PCB * getCurrentProcess(){
    if (currentPID > 0 && processTable[currentPID].processStatus == RUNNING){
        return &processTable[currentPID];
    }
    return NULL;
}

//same as get process by pid -< if it exists return its struct
int getState(pid pid, PCB ** pcb){
    if(pid< 0|| pid >= MAX_PROCESSES || (processTable[pid].currentRSP == NULL)){
        return 1;
    }
    *pcb = &processTable[pid];
    return 0;
}

pid getpid() {
    return currentPID;
}
int setPriority(pid pid, priority newPrio) {
    if(newPrio < MIN_PRIORITY || newPrio > MAX_PRIORITY){
        return 1;
    }
    PCB *pcb;
    if (getState(pid, &pcb)){
        print("Hello\n");
        return 1;
    }
    pcb->priority = newPrio;
        
    return 0;
    
}

