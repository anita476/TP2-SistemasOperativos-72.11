// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <_loader.h>
#include <libSysCalls.h>
#include <stdlib.h>
#include <test_util.h>

#define MAX_BLOCKS 128

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

uint64_t test_mm(uint64_t argc, char *argv[]) {

  if (argc != 1) {
    return -1;
  }

  fprintf(STDOUT,"Starting test...\n");

  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

  if (argc != 1) {
    return -1;
  }

  if ((max_memory = satoi(argv[0])) <= 0) {
    return -1;
  }

  while (1) {
    rq = 0;
    total = 0;

    // Request as many blocks as we can
    fprintf(STDOUT,"Requesting blocks...\n");
    while (rq < MAX_BLOCKS && total < max_memory) {
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      mm_rqs[rq].address = malloc(mm_rqs[rq].size);

      if (mm_rqs[rq].address) {
        fprintf(STDOUT,"Succesful malloc\n");
        total += mm_rqs[rq].size;
        rq++;
      }
    }

    // Set
    uint32_t i;
    for (i = 0; i < rq; i++) {
      if (mm_rqs[i].address) {
        fprintf(STDOUT,"Memset passed\n");
        memset(mm_rqs[i].address, i, mm_rqs[i].size);
      }
    }

    // Check
    for (i = 0; i < rq; i++) {
      if (mm_rqs[i].address) {
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
          fprintf(STDERR,"test_mm ERROR\n");
          return -1;
        } else {
          fprintf(STDOUT,"Memcheck passed\n");
        }
      }
    }

    // Free
    for (i = 0; i < rq; i++) {
      if (mm_rqs[i].address) {
        fprintf(STDOUT,"Freeing block\n");
        free(mm_rqs[i].address);
      }
    }
    fprintf(STDOUT,"Iteration done\n");
  }
}