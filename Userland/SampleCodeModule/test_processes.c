// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "test_util.h"
#include <defs.h>
#include <commands.h>
#include <libSysCalls.h>   

typedef struct P_rq {
  int32_t pid;
  status state;
} p_rq;

int64_t test_processes(uint64_t argc, char *argv[]) {
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
  uint64_t max_processes;
  char *argvAux[] = {0};

  if (argc != 1)
    return -1;

  if ((max_processes = satoi(argv[0])) <= 0){
    return -1;
  }
  
  char buffer2[10];
  itoa(max_processes, buffer2, 10);
  print("Max processes: ");
  print(buffer2);
  print("\n");
  p_rq p_rqs[max_processes];

  while (1) {
    print("\n Starting loop\n");
    createProcessInfo loopInfo = {.name = "endless_loop",
                                  .fg_flag = 0,
                                  .priority = DEFAULT_PRIORITY,
                                  .start = (ProcessStart) endless_loop_print, // change if you dont want to see the processes running
                                  .argc = 0,
                                  .argv = (const char *const *) argvAux};

              
    // Create max_processes processes
    for (rq = 0; rq < max_processes; rq++) {
      p_rqs[rq].pid = createProcess(&loopInfo);
      if (p_rqs[rq].pid == -1) {
        print("test_processes: ERROR creating process\n");
        return -1;
      } else {
        p_rqs[rq].state = RUNNING;
        alive++;
      }
    }
    

    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0) {

      for (rq = 0; rq < max_processes; rq++) {
        action = GetUniform(100) % 2;

        switch (action) {
          case 0:
            if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) {
              if (kill(p_rqs[rq].pid) != 0) {
                print("test_processes: ERROR killing process\n");
                return -1;
              }
              p_rqs[rq].state = KILLED;
              alive--;

            }
            break;

          case 1:
            if (p_rqs[rq].state == RUNNING) {
              if (block(p_rqs[rq].pid) != 0) {
                print("test_processes: ERROR blocking process\n");

                return -1;
              }
              p_rqs[rq].state = BLOCKED;
            }
            break;
        }
      }
      // Randomly unblocks processes
      for (rq = 0; rq < max_processes; rq++){
        if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) {
          if (unblock(p_rqs[rq].pid) != 0) {
            print("test_processes: ERROR unblocking process\n");
            return -1;
          }
          p_rqs[rq].state = RUNNING;
        }
       
      }
    }
    print("Loop done\n");
    
  }

  
}
 