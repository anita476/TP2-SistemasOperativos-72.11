// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <lib.h>
#include <processes.h>
#include <scheduler.h>
#include <videoDriver.h>

static int nameValidation(const char *name);
static int findPID(pid pid, ProcessS **pr);

static ProcessS processArr[MAX_PROCESSES];  // We store all of our proccesses info here
static familyUnit families[MAX_PROCESSES];  // we store children here
int lastPID = 0;

static void addChild(pid parent, pid child) {
  families[parent].childrenArr[child] = 1;
  families[parent].numberOfChildren++;
}

static void removeChild(pid child) {
  ProcessS *proc = &processArr[child];
  pid parent = proc->parent;
  char buffer[10];
  intToStr(proc->parent, buffer, 10);
  if (parent != (NO_PROC)) {
    families[parent].childrenArr[child] = 0;
    families[parent].numberOfChildren--;
    if (families[parent].numberOfChildren == 0) {  // if it isnt blocked it has no effect
      unblock(parent);
    }
  }
}

static void shellAdoption(pid child, pid lastParent) {
  /* remove from home */
  families[lastParent].childrenArr[child] = 0;
  families[lastParent].numberOfChildren--;
  /* go through with adoption */
  processArr[child].parent = 0;
  families[0].childrenArr[child] = 1;
  families[0].numberOfChildren++;
}

pid createProcess(createProcessInfo *info) {
  pid parent = getpid();
  pid pid = 0;
  // Find first empty slot
  for (; pid < MAX_PROCESSES && processArr[pid].stackEnd != NULL; pid++)
    ;

  if (pid >= MAX_PROCESSES || info->argc < 0 || !nameValidation(info->name)) {
    if (pid >= MAX_PROCESSES) {}
    if (info->argc < 0) {}
    if (!nameValidation(info->name)) {}
    return -1;
  }

  void *stackEnd = NULL;
  char *nameCopy = NULL;
  char **argvCopy = NULL;

  // Allocate space for each field
  stackEnd = malloc(STACK_SIZE);
  if (stackEnd == NULL) {
    print(STDERR, "Could not allocate stackEnd\n");
    return -1;
  }

  if ((nameCopy = malloc(strlen(info->name) + 1)) == NULL) {
    print(STDERR, "Could not allocate for name\n");
    return -1;
  }

  if (info->argc != 0 && ((argvCopy = malloc(sizeof(char *) * info->argc)) == NULL)) {
    print(STDERR, "Could not malloc for argv\n");
    free(stackEnd);
    free(nameCopy);
    return -1;
  }

  // Copy arguments
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
      print(STDERR, "Argv could not malloc");
      return -1;
    }

    memcpy(argvCopy[i], info->argv[i], length);
  }

  strcpy(nameCopy, info->name);
  if (nameCopy == NULL) {
    print(STDERR, "NAME COPY IS NULL\n");
  }

  ProcessS *process = &processArr[pid];

  memset(process, 0, sizeof(ProcessS));

  process->stackEnd = stackEnd;
  process->stackStart = stackEnd + STACK_SIZE;
  process->fg_flag = info->fg_flag;
  process->name = nameCopy;
  process->argv = argvCopy;
  process->argc = info->argc;

  process->input = info->input;
  process->output = info->output;

  if (pid != (PID_KERNEL)) { /* if im in kernel im creating  shell -> if its shell then the process it no ones child*/
    addChild(parent, pid);
    process->parent = parent;
  } else {
    process->parent = (NO_PROC);
  }

  // Call scheduler so that it adds the process to its queue and blocks parent process
  processWasCreated(pid, process->argc, (const char *const *) process->argv, info->priority, info->start,
                    process->stackStart);
  if (process->name == NULL) {
    print(STDERR, "NAME POINTER IS NULL\n");
  }
  lastPID++;

  return pid;
}

static int nameValidation(const char *name) {
  if (name == NULL) {
    print(STDERR, "Name is null\n");
    return 0;
  }
  int i = 0;
  for (; i <= MAX_NAME_LENGTH; i++) {
    char c = name[i];
    if (c == '\0') {
      return 1;
    }
  }
  return 0;
}

int kill(pid pid) {  // if it had children, shell adopts them
                     //  if it had a parent, remove it from family
  if (pid == 0) {
    return -1;  // cant kill shell
  }
  ProcessS *process;
  if (!findPID(pid, &process)) {
    print(STDERR, "Validation error\n");
    return -1;
  }
  /* remove from parent list*/
  removeChild(pid);

  /* check if it has children and make shell adopt them*/
  if (families[pid].numberOfChildren != 0) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
      if (families[pid].childrenArr[i]) {
        shellAdoption(i, pid);
        if (!families[pid].numberOfChildren) {
          break;
        }
      }
    }
  }

  // Free all process memory
  for (int i = 0; i < process->memoryCount; i++) {
    // This prints "pointer is null" -> its correct, process hasnt assigned memory lel
    free(process->memory[i]);
  }
  free(process->memory);

  // Call scheduler to take it out of queue
  processWasKilled(pid);

  for (int i = 0; i < process->argc; i++) {
    free(process->argv[i]);
  }
  //  I dont think its necessary to free, but idk, IF MEMORY LEAKS LOOK HERE
  // free(process->argv);

  free(process->stackEnd);
  free(process->name);
  memset(process, 0, sizeof(ProcessS));
  lastPID--;
  return 0;
}

int killWithChildren(pid pid) {
  if (pid == 0) {
    return -1;
  }

  ProcessS *process;
  if (!findPID(pid, &process)) {
    print(STDERR, "Validation error\n");
    return -1;
  }

  removeChild(pid);

  if (families[pid].numberOfChildren != 0) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
      if (families[pid].childrenArr[i]) {
        kill(i);
      }
    }
  }

  for (int i = 0; i < process->memoryCount; i++) {
    free(process->memory[i]);
  }
  free(process->memory);

  processWasKilled(pid);

  for (int i = 0; i < process->argc; i++) {
    free(process->argv[i]);
  }

  free(process->stackEnd);
  free(process->name);
  memset(process, 0, sizeof(ProcessS));
  lastPID--;
  return 0;
}

int sendToBackground(pid pid) {
  ProcessS *p;
  if (!findPID(pid, &p)) {
    return -1;
  }
  return !(p->fg_flag = BACKGROUND);
}

int bringToForeground(pid pid) {
  ProcessS *p;
  if (!findPID(pid, &p)) {
    return -1;
  }
  return (p->fg_flag = FOREGROUND);
}

int isForeground(pid pid) {
  ProcessS *p;
  if (!findPID(pid, &p)) {
    return -1;
  }
  return p->fg_flag;
}

int listProcessesInfo(ProcessInfo *processes, int maxProcesses) {
  int processCounter = 0;
  for (int i = 0; i < MAX_PROCESSES && processCounter < maxProcesses; ++i) {
    ProcessS *process = &processArr[i];
    if (process->stackEnd != NULL) {
      ProcessInfo *info = &processes[processCounter++];
      info->pid = i;
      strcpy(info->name, process->name);
      info->stackEnd = process->stackEnd;
      info->stackStart = process->stackStart;
      info->fg_flag = process->fg_flag;
      info->parent = process->parent;
      getProcessInfo(i, info);
    }
  }
  return processCounter;
}

static int findPID(pid pid, ProcessS **pr) {
  if (pid < 0 || pid >= MAX_PROCESSES || processArr[pid].stackEnd == NULL) {
    return 0;
  }
  *pr = &processArr[pid];
  return 1;
}


int get_process_input(pid pid){
  ProcessS *p;
  if (!findPID(pid, &p)) {
    return -1;
  }
  return p->input;
}

int get_process_output(pid pid){
  ProcessS *p;
  if (!findPID(pid, &p)) {
    return -1;
  }
  return p->output;
}
