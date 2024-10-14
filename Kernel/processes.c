#include <processes.h>
#include <videoDriver.h>
#include <scheduler.h>

static int nameValidation(const char * name);
static int findPID(pid pid, ProcessS **pr);
/* MISSING MEMORY STATE COUUNTERSSS */

static ProcessS processArr[MAX_PROCESSES]; //we store all of our proccesses info here
int lastPID = 0;

pid createProcess(createProcessInfo * info){
    pid pid = 0;
    //find first empty slot
    for (; pid < MAX_PROCESSES && processArr[pid].stackEnd != NULL; pid++)
        ;
    if(pid >= MAX_PROCESSES || info->argc < 0 || !nameValidation(info->name)){
        if(pid>= MAX_PROCESSES){
        }
        if(info->argc<0){
        }
        if(!nameValidation(info->name)){
        }
        return -1;
    }
    void *stackEnd = NULL;
    char *nameCopy = NULL;
    char **argvCopy = NULL;

    //i allocate space for each field 
    //maybe later design specific error messages for each case 
    stackEnd = malloc(STACK_SIZE);
    if( stackEnd == NULL){
        print("Could not allocate stackEnd\n");
        return -1;
    }

    if( (nameCopy = malloc(strlen(info->name)+1)) == NULL){
        print("Could not allocate for name\n");
        return -1;
    }
    if(info->argc != 0 && ((argvCopy = malloc(sizeof(char *) * info->argc)) == NULL)){
        print("Could not malloc for argv\n");
        free(stackEnd);
        free(nameCopy);
        return -1;
    }
    // copy arguments 
    for (int i = 0; i < info->argc; ++i) {
        size_t length = strlen(info->argv[i]) + 1;

        if ((argvCopy[i] = malloc(length)) == NULL) {
            free(stackEnd);
            free(nameCopy);
            while (i > 0) {
                i--;
                free(argvCopy[i]);
            }
            free(argvCopy);
            print("Argv could not malloc");
            return -1;
        }

        memcpy(argvCopy[i], info->argv[i], length);
    }

    strcpy(nameCopy,info->name);
    if(nameCopy == NULL){
        print("NAME COPY IS NULL\n");
    }

    ProcessS * process = &processArr[pid];

    memset(process, 0, sizeof(ProcessS));

    process->stackEnd = stackEnd;

    process->stackStart = stackEnd + STACK_SIZE;
    process->fg_flag = info->fg_flag;
    process->name = nameCopy;
    process->argv = argvCopy;
    process->argc = info->argc;

    // call scheduler so that it adds the process to its queue
    processWasCreated(pid,info->argc,info->argv,info->priority, info->start,process->stackStart);
    if(process->name == NULL){
        print("NAME POINTER IS NULL\n");
    }

    lastPID++;
        

    return pid;
}

static int nameValidation(const char * name){
    if(name == NULL){
        print("Name is null\n");
        return 0;
    }
    int i = 0;
    for(; i<= MAX_NAME_LENGTH ; i++){
        char c = name[i];
        if(c == '\0'){
            return 1;
        }
    }
    return 0;
}


int kill(pid pid){
    ProcessS * process;
    if(!findPID(pid, &process)){
        print("Validation error\n");
        return 1;
    }
    
     // free all process memory -
    for (int i = 0; i < process->memoryCount; i++){
        // this prints "pointer is null" -> its correct , process hasnt assigned memory lel
        free(process->memory[i]);
    } 
    free(process->memory); 
    //print("Something went wrong\n");

    //call scheduler to take it out of queue
    processWasKilled(pid);

    for (int i = 0; i < process->argc; i++) {
        free(process->argv[i]);
        
    }
    //  I dont think its necessary to free, but idk, IF MEMORY LEAKS LOOK HERE
    //free(process->argv); 

    free(process->stackEnd);
    free(process->name);
    memset(process, 0, sizeof(ProcessS));
    return 0;

}

int sendToBackground(pid pid){
    ProcessS * p;
    if(!findPID(pid, &p)){
        return -1;
    }
    return !(p->fg_flag = BACKGROUND);
}

int bringToForeground(pid pid){
    ProcessS * p;
    if(!findPID(pid, &p)){
        return -1;
    }
    return (p->fg_flag = FOREGROUND);
}


int isForeground(pid pid){
    ProcessS * p;
    if(!findPID(pid, &p)){
        return -1;
    }
    return p->fg_flag;
}

int listProcessesInfo(ProcessInfo * processes, int max_processes){
    // not implemented yet
    return 0;
}


static int findPID(pid pid, ProcessS **pr) {
    if (pid < 0 || pid >= MAX_PROCESSES || processArr[pid].stackEnd == NULL)
        return 0;

    *pr = &processArr[pid];
    return 1;
}