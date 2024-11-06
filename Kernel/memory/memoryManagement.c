// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <memoryManagement.h>
#include <unistd.h>

#ifdef LIST

#define BLOCK_SIZE sizeof(struct block)

struct block {
  size_t size;         // Size of the block
  struct block *next;  // Pointer to the next free block
};

static struct block *freeList;

void init_memory_manager(void *startHeapAddress, size_t totalSize) {
  print(STDOUT, "Initialized with memory manager = FREE LIST \n\n");
  freeList = (struct block *) startHeapAddress;
  // Set the size of the first block
  freeList->size = totalSize - BLOCK_SIZE;  // Subtract size of the block header
  freeList->next = NULL;                    // No other blocks yet
}

void *malloc(size_t bytes) {
  struct block *current, *previous;
  bytes += BLOCK_SIZE;  // Account for block header

  // Search for block
  previous = NULL;
  for (current = freeList; current != NULL; current = current->next) {
    if (current->size >= bytes) {  // Found a big enough block
      if (current->size > bytes + BLOCK_SIZE) {
        // Split the block
        struct block *newBlock = (struct block *) ((char *) current + bytes);
        newBlock->size = current->size - bytes;
        newBlock->next = current->next;
        current->size = bytes;
        current->next = newBlock;
      }
      // Remove from free list
      if (previous == NULL) {
        freeList = current->next;
      } else {
        previous->next = current->next;
      }
      return (void *) ((char *) current + BLOCK_SIZE);  // Return memory address
    }
    previous = current;
  }
  return NULL;  // No suitable block found
}

// Free memory
void free(void *ptr) {
  if (ptr == NULL) {
    return;
  }
  struct block *blockToFree = (struct block *) ((char *) ptr - BLOCK_SIZE);
  blockToFree->next = freeList;
  freeList = blockToFree;  // Add to the front of the free list
}


void memory_manager_state() {
    char buffer[32];
    print(STDOUT, "\n=== Memory Manager State ===\n");

    // Calculate total allocated by traversing the free list
    size_t total_free = 0;
    struct block *current = freeList;
    while (current != NULL) {
        total_free += current->size;
        current = current->next;
    }

    // The total is what we got in init
    size_t total = freeList->size + BLOCK_SIZE;  // Add back the first block header

    print(STDOUT, "Total memory: ");
    intToStr(total, buffer, 10);
    print(STDOUT, buffer);
    print(STDOUT, " bytes\n");

    print(STDOUT, "Free memory: ");
    intToStr(total_free, buffer, 10);
    print(STDOUT, buffer);
    print(STDOUT, " bytes\n");

    print(STDOUT, "Allocated memory: ");
    intToStr(total - total_free, buffer, 10);
    print(STDOUT, buffer);
    print(STDOUT, " bytes\n");
}
#endif