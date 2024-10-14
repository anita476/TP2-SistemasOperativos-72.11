#include <test_prio.h>
#include <defs.h>
#include <stdint.h>
#include "test_util.h"
#include <libSysCalls.h>


#define MINOR_WAIT 1000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 100000000      // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 0  // TODO: Change as required
#define MEDIUM 2  // TODO: Change as required
#define HIGHEST 5 // TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void test_prio() {
  int64_t pids[TOTAL_PROCESSES];
  char *argv[] = {"9000000"};
  uint64_t i;
  createProcessInfo endlessInfo = {.name = "endless",
                                     .fg_flag = 1,
                                     .priority = DEFAULT_PRIORITY,
                                     .start = (ProcessStart) endless_loop_print,
                                     .argc = 0,
                                     .argv = (const char *const *) argv};
  print("CREATING PROCESSES...\n");
  for (i = 0; i < TOTAL_PROCESSES; i++){
    pids[i] = createProcess(&endlessInfo);
  }

  
  bussy_wait(WAIT);
  print("\nCHANGING PRIORITIES...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++){
    int n = setPriority(pids[i], prio[i]);
    if(n == 0){
      print("Changed priority\n");
    }
    else{
      print("Couldnt change prio\n");
    }
  }



  bussy_wait(WAIT);
  print("\nBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    block(pids[i]);

  print("CHANGING PRIORITIES WHILE BLOCKED...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++){
    int j = setPriority(pids[i], MEDIUM);
    if(j == 0){
      print("Priority changed\n");
    }
    else{
      print("Couldnt change prio\n");
    }
  }


  print("UNBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    unblock(pids[i]); 

  bussy_wait(WAIT);
  print("\nKILLING...\n"); 

  for (i = 0; i < TOTAL_PROCESSES; i++)
    kill(pids[i]); 
}