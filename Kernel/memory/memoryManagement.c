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
static memoryInfo memInfo;

void init_memory_manager(void *startHeapAddress, size_t totalSize) {
  freeList = (struct block *) startHeapAddress;
  // Set the size of the first block
  freeList->size = totalSize - BLOCK_SIZE;  // Subtract size of the block header
  freeList->next = NULL;                // No other blocks yet

  memInfo.totalSize = totalSize;
  memInfo.freeSize = freeList->size;
  memInfo.allocatedSize = 0;
}

void *malloc(size_t bytes) {
  if (bytes == 0) return NULL; 

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

        memInfo.freeSize -= bytes;
        memInfo.allocatedSize += bytes - BLOCK_SIZE;
      } else {
        memInfo.freeSize -= current->size;
        memInfo.allocatedSize += bytes - BLOCK_SIZE;
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

static void coalesce_free_blocks() {
    struct block *current = freeList;
    while (current != NULL && current->next != NULL) {
        if ((char *)current + current->size == (char *)current->next) {
            current->size += current->next->size;
            current->next = current->next->next;
            // print(STDERR, "Coalesced\n");
        } else {
            current = current->next;
            // print(STDOUT, "Not Coalesced\n");
        }
    }
}

// Free memory
void free(void *ptr) {
  if (ptr == NULL) {
    return;
  }
  struct block *blockToFree = (struct block *) ((char *) ptr - BLOCK_SIZE);

  if (blockToFree->size == 0 || blockToFree->size > memInfo.totalSize) return;

  memInfo.freeSize += blockToFree->size - BLOCK_SIZE;
  memInfo.allocatedSize -= (blockToFree->size - BLOCK_SIZE);

  blockToFree->next = freeList;
  freeList = blockToFree;  // Add to the front of the free list

  // we should coalesce free blocks for less fragmentation 
  coalesce_free_blocks();
}

memoryInfo* get_memory_info() {
  return &memInfo;
  // char buffer[32];
  // print(STDOUT, "\n=== Memory Manager State ===\n");

  // // Calculate total allocated by traversing the free list
  // size_t total_free = 0;
  // struct block *current = freeList;
  // while (current != NULL) {
  //   total_free += current->size;
  //   current = current->next;
  // }

  // // The total is what we got in init
  // size_t total = freeList->size + BLOCK_SIZE;  // Add back the first block header

  // print(STDOUT, "Total memory: ");
  // int_to_str(total, buffer, 10);
  // print(STDOUT, buffer);
  // print(STDOUT, " bytes\n");

  // print(STDOUT, "Free memory: ");
  // int_to_str(total_free, buffer, 10);
  // print(STDOUT, buffer);
  // print(STDOUT, " bytes\n");

  // print(STDOUT, "Allocated memory: ");
  // int_to_str(total - total_free, buffer, 10);
  // print(STDOUT, buffer);
  // print(STDOUT, " bytes\n");
}
#endif