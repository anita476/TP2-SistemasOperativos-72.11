// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "test_util.h"
#include <defs.h>
#include <libSysCalls.h>
#include <stdint.h>
#include <test_prio.h>

#define MINOR_WAIT      1000000
#define WAIT            100000000
#define TOTAL_PROCESSES 3
#define LOWEST          0
#define MEDIUM          5
#define HIGHEST         10

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void test_prio() {
  int64_t pids[TOTAL_PROCESSES];
  char *argv[] = {0};
  uint64_t i;

  int fg_flag = sys_is_foreground(sys_get_pid());

  createProcessInfo endlessInfo = {.name = "endless",
                                   .fg_flag = fg_flag,
                                   .priority = DEFAULT_PRIORITY,
                                   .start = (ProcessStart) endless_loop_print,
                                   .argc = 0,
                                   .argv = (const char *const *) argv,
                                   .input = STDIN,
                                   .output = STDOUT};
  fprintf(STDOUT, "\n");
  fprintf(STDOUT, "CREATING PROCESSES...\n");
  for (i = 0; i < TOTAL_PROCESSES; i++) {
    pids[i] = sys_create_process(&endlessInfo);
  }

  bussy_wait(WAIT);
  fprintf(STDOUT, "\nCHANGING PRIORITIES...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++) {
    sys_set_priority(pids[i], prio[i]);
  }

  bussy_wait(WAIT);
  fprintf(STDOUT, "\nBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++) {
    sys_block(pids[i]);
  }

  fprintf(STDOUT, "CHANGING PRIORITIES WHILE BLOCKED...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++) {
    sys_set_priority(pids[i], MEDIUM);
  }

  fprintf(STDOUT, "UNBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++) {
    sys_unblock(pids[i]);
  }

  bussy_wait(WAIT);
  fprintf(STDOUT, "\nKILLING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++) {
    sys_kill(pids[i]);
  }
}