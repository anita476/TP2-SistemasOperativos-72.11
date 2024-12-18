// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <keyboardDriver.h>
#include <lib.h>
#include <processes.h>
#include <scheduler.h>
#include <videoDriver.h>

static int name_validation(const char *name);
static int find_pid(pid pid, ProcessS **pr);

static ProcessS processArr[MAX_PROCESSES];  // We store all of our proccesses info here
static familyUnit families[MAX_PROCESSES];  // We store children here
int lastPID = 0;

static void add_child(pid parent, pid child) {
  families[parent].childrenArr[child] = 1;
  families[parent].numberOfChildren++;
}

static void remove_child(pid child) {
  ProcessS *proc = &processArr[child];
  pid parent = proc->parent;
  char buffer[10];
  int_to_str(proc->parent, buffer, 10);
  if (parent != (NO_PROC)) {
    families[parent].childrenArr[child] = 0;
    families[parent].numberOfChildren--;

    int should_unblock = 1; 
    if (families[parent].numberOfChildren > 0) {
      for (int i = 0; i < MAX_PROCESSES && families[parent].numberOfChildren > 0; i++) {
        if (families[parent].childrenArr[i] && is_foreground(i)) {
          should_unblock = 0;
          break;
        }
      }
    }
    if (should_unblock) unblock(parent);
  }
}

static void shell_adoption(pid child, pid lastParent) {
  families[lastParent].childrenArr[child] = 0;
  families[lastParent].numberOfChildren--;
  processArr[child].parent = 0;
  families[0].childrenArr[child] = 1;
  families[0].numberOfChildren++;
}

pid create_process(createProcessInfo *info) {
  pid parent = get_pid();
  pid pid = 0;
  for (; pid < MAX_PROCESSES && processArr[pid].stackEnd != NULL; pid++)
    ;

  if (pid >= MAX_PROCESSES || info->argc < 0 || !name_validation(info->name)) {
    if (pid >= MAX_PROCESSES) {
      print(STDERR, "No more processes can be created\n");
    }
    if (info->argc < 0) {
      print(STDERR, "Argc is negative\n");
    }
    if (!name_validation(info->name)) {
      print(STDERR, "Name is invalid\n");
    }
    return -1;
  }

  void *stackEnd = NULL;
  char *nameCopy = NULL;
  char **argvCopy = NULL;

  stackEnd = malloc(STACK_SIZE);
  if (stackEnd == NULL) {
    print(STDERR, "Could not allocate stackEnd\n");
    return -1;
  }

  if ((nameCopy = malloc(strlen(info->name) + 1)) == NULL) {
    print(STDERR, "Could not allocate for name\n");
    free(stackEnd);
    return -1;
  }

  if (info->argc != 0) {
    if ((argvCopy = malloc(sizeof(char *) * info->argc)) == NULL) {
      print(STDERR, "Could not malloc for argv\n");
      free(stackEnd);
      free(nameCopy);
      return -1;
    }

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
  }

  strcpy(nameCopy, info->name);

  ProcessS *process = &processArr[pid];

  memset(process, 0, sizeof(ProcessS));

  process->stackEnd = stackEnd;
  process->stackStart = stackEnd + STACK_SIZE;
  process->fg_flag = info->fg_flag;
  process->name = nameCopy;
  process->argv = argvCopy;
  process->argc = info->argc;

  add_child(parent, pid);
  process->parent = parent;

  int parentInput = get_process_input(process->parent);
  if (parentInput != STDIN && info->input == STDIN) {
    process->input = parentInput;
  } else {
    process->input = info->input;
  }
  int parentOutput = get_process_output(process->parent);
  if (parentOutput != STDOUT && info->output == STDOUT) {
    process->output = parentOutput;
  } else {
    process->output = info->output;
  }

  process_was_created(pid, process->argc, (const char *const *) process->argv, info->priority, info->start,
                      process->stackStart);
  lastPID++;
  return pid;
}

static int name_validation(const char *name) {
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

int kill(pid pid) {
  if (pid == 0) {
    return -1;  // Can't kill shell
  }
  ProcessS *process;
  if (!find_pid(pid, &process)) {
    return -1;
  }
  remove_child(pid);

  if (families[pid].numberOfChildren != 0) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
      if (families[pid].childrenArr[i]) {
        shell_adoption(i, pid);
        if (!families[pid].numberOfChildren) {
          break;
        }
      }
    }
  }

  for (int i = 0; i < process->memoryCount; i++) {
    free(process->memory[i]);
  }
  free(process->memory);

  process_was_killed(pid);

  for (int i = 0; i < process->argc; i++) {
    free(process->argv[i]);
  }

  if (process->output != STDOUT && (get_process_output(process->parent) == STDOUT)) {
    signal_eof(process->output);
  }
  free(process->stackEnd);
  free(process->name);
  memset(process, 0, sizeof(ProcessS));
  lastPID--;

  return 0;
}

int kill_with_children(pid pid) {
  if (pid == 0) {
    return -1;
  }

  ProcessS *process;
  if (!find_pid(pid, &process)) {
    print(STDERR, "Validation error\n");
    return -1;
  }

  remove_child(pid);

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

  process_was_killed(pid);

  for (int i = 0; i < process->argc; i++) {
    free(process->argv[i]);
  }
  free(process->stackEnd);
  free(process->name);
  memset(process, 0, sizeof(ProcessS));
  lastPID--;
  return 0;
}

int send_to_background(pid pid) {
  ProcessS *p;
  if (!find_pid(pid, &p)) {
    return -1;
  }
  p->fg_flag = BACKGROUND;
  return 0;
}

int bring_to_foreground(pid pid) {
  ProcessS *p;
  if (!find_pid(pid, &p)) {
    return -1;
  }
  return (p->fg_flag = FOREGROUND);
}

int is_foreground(pid pid) {
  ProcessS *p;
  if (!find_pid(pid, &p)) {
    return -1;
  }
  return p->fg_flag;
}

int list_processes_info(ProcessInfo *processes, int maxProcesses) {
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
      info->input = process->input;
      info->output = process->output;
      get_process_info(i, info);
    }
  }
  return processCounter;
}

static int find_pid(pid pid, ProcessS **pr) {
  if (pid < 0 || pid >= MAX_PROCESSES || processArr[pid].stackEnd == NULL) {
    return 0;
  }
  *pr = &processArr[pid];
  return 1;
}

int get_process_input(pid pid) {
  if (pid == NO_PROC || pid == PID_KERNEL || pid == 0) {
    return STDIN;
  }
  ProcessS *p;
  if (!find_pid(pid, &p)) {
    return -1;
  }
  return p->input;
}

int get_process_output(pid pid) {
  if (pid == NO_PROC || pid == PID_KERNEL || pid == 0) {
    return STDOUT;
  }
  ProcessS *p;
  if (!find_pid(pid, &p)) {
    return -1;
  }
  return p->output;
}
