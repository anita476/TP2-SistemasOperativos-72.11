// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "phylo.h"
#include "utils.h"
#include <commands.h>
#include <libSysCalls.h>  // getChar
#include <stdint.h>
#include <test_util.h>  // for satoi

/*
    conditions:
    1. no deadlocks
    2. no starvation (using even-odd method, there is possibility of starvation
    for philosopher 0 if num_philosophers = ODD
    since the last philosopher would have an even index
    therefore they would need the same fork at the same time)
    3. no race conditions: can be avoided by using semaphores ->
    when adding a philosopher, we have to lock/block the philosophers that are right and left of the new philosopher
*/

typedef int pid;
typedef int sem_t;

#define MAX_PHILOSOPHERS 10
#define MIN_PHILOSOPHERS 3
#define THINK_TIME       1000
#define EAT_TIME         800

#define MUTEX "phylo_mutex"

#define LEFT(id)  (((id) + num_philosophers - 1) % num_philosophers)
#define RIGHT(id) (((id) + 1) % num_philosophers)

#define LEFT_PHIL(id, num)  (((id) + num - 1) % num)
#define RIGHT_PHIL(id, num) (((id) + 1) % num)

typedef enum { NONE = 0, THINKING, HUNGRY, EATING } State;

typedef struct philosopher {
  pid pid;
  State state;
  State prev;
  sem_t sem;
} philosopher_t;

static char *PHILOSOPHER_NAMES[] = {"Plato",     "Aristotle", "Socrates",     "Kant",       "Nietzsche",
                                    "Confucius", "Buddha",    "Descartes",    "Hume",       "Locke",
                                    "Marx",      "Sartre",    "Wittgenstein", "Russell",    "Hegel",
                                    "Spinoza",   "Voltaire",  "Epicurus",     "Pythagoras", "Seneca"};

static char *HELLO_MESSAGES[] = {"arrives at the table!", "joins the feast!", "takes a seat!", "appears!", "joins in!"};

static char *GOODBYE_MESSAGES[] = {
    "has reached enlightenment...",          "went to find the meaning of life...",
    "left to question reality elsewhere...", "went to think, therefore to be...",
    "departed to contemplate existence...",
};

static philosopher_t philosophers[MAX_PHILOSOPHERS];
static sem_t mutex;               // For protecting shared state
static int num_philosophers = 0;  // should this int be protected as well?
static int running = 0;

static void print_welcome(int id);
static void print_goodbye(int id);
static void display_table();
static void try_to_eat(int phil_id);
static int add_semaphore(int phil_id);
static int philosopher_action(int argc, char *argv[]);
static int add_philosopher(int id);
static int remove_philosopher(int running_check);

uint64_t phylo(uint64_t argc, char *argv[]) {
  if (argc != 1) {
    fprintf(STDERR, "ERROR: Usage: phylo <num_philosophers>\n");
    return -1;
  }

  running = 1;
  num_philosophers = 0;

  int initial_philosophers = satoi(argv[0]);
  if (initial_philosophers < MIN_PHILOSOPHERS || initial_philosophers > MAX_PHILOSOPHERS) {
    fprintf(STDERR, "ERROR: Invalid number of philosophers (must be between 3 and 20 inclusive)\n");
    return -1;
  }

  if ((mutex = sem_open(MUTEX, 1)) < 0) {
    fprintf(STDERR, "ERROR: ERROR: Failed to create mutex\n");
    return -1;
  }

  fprintf(STDOUT, "Initial number of philosophers: ");
  fprintf(STDOUT, argv[0]);
  fprintf(STDOUT, "\n");

  fprintf(STDOUT, "\n*****************************************\n");
  fprintf(STDOUT, "* Welcome to the Great Philosophical Feast! *\n");
  fprintf(STDOUT, "*     Where great minds eat and think     *\n");
  fprintf(STDOUT, "*****************************************\n\n");
  fprintf(STDOUT, "Available Actions:\n");
  fprintf(STDOUT, "  'a' - Invite another philosopher to dine\n");
  fprintf(STDOUT, "  'r' - Excuse a philosopher from the table\n");
  fprintf(STDOUT, "  's' - Check on our distinguished guests\n");
  fprintf(STDOUT, "  'c' - View the dinner table state\n");
  fprintf(STDOUT, "  'q' - End the philosophical feast\n\n");

  for (int i = 0; i < MAX_PHILOSOPHERS; i++) {
    philosophers[i].pid = -1;
    philosophers[i].state = NONE;
    philosophers[i].prev = NONE;
  }

  for (int i = 0; i < initial_philosophers; i++) {
    if (add_philosopher(i) < 0) {
      fprintf(STDERR, "ERROR: Failed to create initial philosophers\n");

      while (--i >= 0) {
        remove_philosopher(0);
      }
      sem_close(mutex);
      return -1;
    }
  }

  char cmd = 0;
  while (running && ((cmd = getChar()) != 'q')) {
    if (cmd == 'q') {
      running = 0;
      break;
    }
    switch (cmd) {
    case 'a':
      if (add_philosopher(num_philosophers) < 0)
        fprintf(STDERR, "ERROR: Failed to add philosopher\n");
      break;
    case 'r':
      if (remove_philosopher(1) < 0) {
        fprintf(STDERR, "ERROR: Failed to remove philosopher\n");
      }
      break;
    case 's':
      sem_wait(mutex);
      ps();
      fprintf(STDOUT, "\n");
      sem_post(mutex);
      break;
    case 'c':
      sem_wait(mutex);
      display_table();
      sem_post(mutex);
      break;
    }
  }
  // running = 0;

  // cleanup
  fprintf(STDOUT, "Cleaning up philosophers\n");
  while (num_philosophers > 0) {
    remove_philosopher(0);
  }
  sem_close(mutex);
  fprintf(STDOUT, "Done\n");

  return 0;
}

static void display_table() {
  if (num_philosophers < MIN_PHILOSOPHERS)
    return;

  int state_changed = 0;

  for (int i = 0; i < num_philosophers && !state_changed; i++) {
    // if (philosophers[i].state == NONE) return;
    if ((philosophers[i].state == EATING && philosophers[i].prev != EATING) ||
        (philosophers[i].state != EATING && philosophers[i].prev == EATING)) {
      state_changed = 1;
      break;
    }
  }

  if (state_changed) {
    // fprintf(STDOUT, "\n=== The Great Philosophical Dinner ===\n");
    char count_str[50];
    sprintf(count_str, "Philosophers: %d: ", num_philosophers);
    fprintf(STDOUT, count_str);
    for (int i = 0; i < num_philosophers; i++) {
      philosophers[i].prev = philosophers[i].state;
      fprintf(STDOUT, philosophers[i].state == EATING ? "E  " : ".  ");
    }
    fprintf(STDOUT, "\n");
  }
}

static void print_welcome(int id) {
  fprintf(STDOUT, "\n*** Philosopher ");
  fprintf(STDOUT, PHILOSOPHER_NAMES[id]);
  fprintf(STDOUT, " ");
  fprintf(STDOUT, HELLO_MESSAGES[id % 5]);
  fprintf(STDOUT, " *\n");
}

static void print_goodbye(int id) {
  fprintf(STDOUT, "\n>>> Philosopher ");
  fprintf(STDOUT, PHILOSOPHER_NAMES[id]);
  fprintf(STDOUT, " ");
  fprintf(STDOUT, GOODBYE_MESSAGES[id % 5]);
  fprintf(STDOUT, " <\n");
}

static void try_to_eat(int phil_id) {
  if (philosophers[phil_id].state == HUNGRY && philosophers[LEFT(phil_id)].state != EATING &&
      philosophers[RIGHT(phil_id)].state != EATING) {
    philosophers[phil_id].state = EATING;
    display_table();
    // sem_post(philosophers[phil_id].sem);
    if (sem_post(philosophers[phil_id].sem) < 0) {
      char buffer[50];
      sprintf(buffer, "ERROR!!! Cannot post semaphore for philosopher %d\n", phil_id);
      fprintf(STDERR, buffer);
      sprintf(buffer, "Current pid is: %d\n", getpid());
      fprintf(STDERR, buffer);
    }
  }
}

static int add_semaphore(int phil_id) {
  char sem_name[8];
  sprintf(sem_name, "phy_%d", phil_id);
  if ((philosophers[phil_id].sem = sem_open(sem_name, 0)) < 0) {
    fprintf(STDERR, "ERROR: Cannot create new philosopher semaphore\n");
    return -1;
  }
  return 0;
}

static int philosopher_action(int argc, char *argv[]) {
  if (argc != 1) {
    fprintf(STDERR, "ERROR: Cannot create philosopher action because ID was not given\n");
    return -1;
  }
  int phil_id = satoi(argv[0]);

  char sem_name[8];
  sprintf(sem_name, "phy_%d", phil_id);
  if ((philosophers[phil_id].sem = sem_open(sem_name, 0)) < 0) {
    fprintf(STDERR, "ERROR: Cannot create new philosopher semaphore\n");
    return -1;
  }

  if (sem_open(MUTEX, 1) != mutex) {
    fprintf(STDERR, "ERROR: Cannot open mutex semaphore\n");
    return -1;
  }

  if (phil_id == 0) {
    for (int i = 0; i < MAX_PHILOSOPHERS; i++) {
      add_semaphore(i);
    }
  } else {
    add_semaphore(0);
    add_semaphore(LEFT_PHIL(phil_id, MAX_PHILOSOPHERS));
    add_semaphore(RIGHT_PHIL(phil_id, MAX_PHILOSOPHERS));
  }

  while (running) {

    wait(THINK_TIME);

    // get hungry --> take forks
    sem_wait(mutex);
    philosophers[phil_id].state = HUNGRY;
    display_table();
    try_to_eat(phil_id);
    sem_post(mutex);

    // get permission to eat
    sem_wait(philosophers[phil_id].sem);
    // permission granted, therefore can eat yum yum :p

    wait(EAT_TIME);

    // done eating, back to thinking
    sem_wait(mutex);
    philosophers[phil_id].state = THINKING;
    display_table();
    try_to_eat(LEFT(phil_id));
    try_to_eat(RIGHT(phil_id));
    sem_post(mutex);
  }
  return 0;
}

static int add_philosopher(int id) {
  if (num_philosophers >= MAX_PHILOSOPHERS) {
    fprintf(STDERR, "Maximum number of philosophers reached\n");
    return -1;
  }
  sem_wait(mutex);

  philosophers[id].state = NONE;
  philosophers[id].prev = NONE;

  // if ((philosophers[id].sem = add_semaphore(id)) < 0) {
  //     return -1;
  // }

  char sem_name[8];
  sprintf(sem_name, "phy_%d", id);
  if ((philosophers[id].sem = sem_open(sem_name, 0)) < 0) {
    fprintf(STDERR, "ERROR: Cannot create new philosopher semaphore\n");
    return -1;
  }

  char name[8];
  sprintf(name, "phylo_%d", id);
  char id_str[10];
  itoa(id, id_str, 10);
  char *args[] = {id_str, NULL};
  // Create philosopher process

  int fg_flag = isForeground(getpid());

  createProcessInfo info = {.name = name,
                            .fg_flag = fg_flag,
                            .priority = DEFAULT_PRIORITY,
                            .start = (ProcessStart) philosopher_action,
                            .argc = 1,
                            .argv = (const char *const *) args,
                            .input = STDIN,
                            .output = STDOUT};

  print_welcome(id);

  if ((philosophers[id].pid = createProcess(&info)) < 0) {
    // sem_close(philosophers[id].sem);
    fprintf(STDERR, "ERROR: Unable to create process\n");
    sem_post(mutex);
    return -1;
  }
  num_philosophers++;
  wait(THINK_TIME);
  display_table();
  sem_post(mutex);
  return 0;
}

static int remove_philosopher(int running_check) {
  if (num_philosophers <= MIN_PHILOSOPHERS && running_check) {
    fprintf(STDERR, "Minimum number of philosophers reached\n");
    return -1;
  }

  sem_wait(mutex);
  int id = num_philosophers - 1;

  // Add debug prints
  // char buffer[50];

  // sprintf(buffer, "Attempting to remove philosopher %d\n", id);
  // fprintf(STDOUT, buffer);
  // sprintf(buffer, "PID: %d\n", philosophers[id].pid);
  // fprintf(STDOUT, buffer);
  // sprintf(buffer, "State: %d\n", philosophers[id].state);
  // fprintf(STDOUT, buffer);

  if (philosophers[id].pid <= 0) {
    sem_post(mutex);
    fprintf(STDERR, "ERROR: Invalid philosopher PID\n");
    return -1;
  }

  pid pid_to_kill = philosophers[id].pid;

  while (philosophers[LEFT(id)].state == EATING && philosophers[RIGHT(id)].state == EATING) {
    sem_post(mutex);
    sem_wait(philosophers[id].sem);
    sem_wait(mutex);
  }

  if (kill(pid_to_kill) < 0) {
    fprintf(STDERR, "ERROR: Failed to kill philosopher\n");
    return -1;
  }

  if (sem_close(philosophers[id].sem) < 0) {
    fprintf(STDERR, "ERROR: Failed to close semaphore\n");
    return -1;
  }  // im curious whether any process will be able to sem_close a sempahore that it didnt create

  philosophers[id].state = NONE;
  philosophers[id].prev = NONE;
  philosophers[id].pid = -1;
  philosophers[id].sem = -1;
  num_philosophers--;

  print_goodbye(id);

  display_table();

  sem_post(mutex);

  return 0;
}