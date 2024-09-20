#include <stdio.h>
#include <lib.h>
#include <videoDriver.h>
#include <stdlib.h>
#include <string.h>
#include "syscall.h"
#include "test_util.h"

#define MAX_BLOCKS 128

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

uint64_t test_mm(uint64_t argc, char *argv[]) {
  print("Beginning test...\n");

  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

  if (argc != 1) {
    print("wrong arg\n");
    return -1;
  }

  if ((max_memory = satoi(argv[0])) <= 0) {
    print("wrong argv[0]\n");
    return -1;
  }

  int i = 0;

  while (i != 3) {
    print("while\n");
    rq = 0;
    total = 0;

    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory) {
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      mm_rqs[rq].address = malloc(mm_rqs[rq].size);

      if (mm_rqs[rq].address) {
        print("Succesful malloc\n");
        total += mm_rqs[rq].size;
        rq++;
      }
      else{
        print("malloc not succesful?");
      }
    }

    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        memset(mm_rqs[i].address, i, mm_rqs[i].size);

    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
          print("test_mm ERROR\n");
          return -1;
        }

    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        free(mm_rqs[i].address);
    // wait(500);
    print("Finished iteration\n");
    i++;
  }
  return 0;

}