// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "test_util.h"
#include <commands.h>
#include <defs.h>
#include <libSysCalls.h>

#define MAX_PROC 7

typedef struct P_rq {
  int32_t pid;
  status state;
} p_rq;

int64_t test_processes(uint64_t argc, char *argv[]) {
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
  char *argvAux[] = {0};
  uint64_t max_processes;

  if (argc != 1)
    return -1;

  if ((max_processes = satoi(argv[0])) <= 0) {
    return -1;
  }

  p_rq p_rqs[MAX_PROC];

  while (1) {
    fprintf(STDOUT, "\n Starting loop\n");

    int fg_flag = sys_is_foreground(sys_get_pid());

    createProcessInfo loopInfo = {.name = "endless_loop",
                                  .fg_flag = fg_flag,
                                  .priority = DEFAULT_PRIORITY,
                                  .start = (ProcessStart) endless_loop,
                                  .argc = 0,
                                  .argv = (const char *const *) argvAux,
                                  .input = STDIN,
                                  .output = STDOUT};

    for (rq = 0; rq < MAX_PROC; rq++) {
      p_rqs[rq].pid = sys_create_process(&loopInfo);
      if (p_rqs[rq].pid == -1) {
        fprintf(STDERR, "test_processes: ERROR creating process\n");
        return -1;
      } else {
        p_rqs[rq].state = RUNNING;
        alive++;
      }
    }

    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0) {
      for (rq = 0; rq < MAX_PROC; rq++) {
        action = get_uniform(100) % 2;

        switch (action) {
        case 0:
          if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) {
            if (sys_kill(p_rqs[rq].pid) != 0) {
              fprintf(STDERR, "test_processes: ERROR killing process\n");
              return -1;
            }
            p_rqs[rq].state = KILLED;
            alive--;
          }
          break;

        case 1:
          if (p_rqs[rq].state == RUNNING) {
            if (sys_block(p_rqs[rq].pid) != 0) {
              fprintf(STDERR, "test_processes: ERROR blocking process\n");

              return -1;
            }
            p_rqs[rq].state = BLOCKED;
          }
          break;
        }
      }
      
      // Randomly unblocks processes
      for (rq = 0; rq < MAX_PROC; rq++) {
        if (p_rqs[rq].state == BLOCKED && get_uniform(100) % 2) {
          if (sys_unblock(p_rqs[rq].pid) != 0) {
            fprintf(STDERR, "test_processes: ERROR unblocking process\n");
            return -1;
          }
          p_rqs[rq].state = RUNNING;
        }
      }
    }
    fprintf(STDOUT, "Loop done\n");
  }
}
