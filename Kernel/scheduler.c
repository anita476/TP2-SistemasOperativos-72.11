#include <scheduler.h>

/*
process control block 
process queue 
process unblock
process switch
kill current / get current
yield 

*/
extern void * createProcessStack(int argc, const char *const argv[], void *rsp, ProcessStart start);

static PCB processTable[MAX_PROCESSES];
static pid currentPID;
static pid nextPID;
static uint8_t currentQuantum;


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
    return 0;
}
int processWasKilled(pid pid){
    PCB * pcb ;

}

