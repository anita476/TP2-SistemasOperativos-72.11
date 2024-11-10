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
  freeList->size = totalSize - BLOCK_SIZE;
  freeList->next = NULL;

  memInfo.totalSize = totalSize;
  memInfo.freeSize = freeList->size;
  memInfo.allocatedSize = 0;
}

void *malloc(size_t bytes) {
  if (bytes == 0) {
    return NULL;
  }

  struct block *current, *previous;
  bytes += BLOCK_SIZE;

  previous = NULL;
  for (current = freeList; current != NULL; current = current->next) {
    if (current->size >= bytes) {
      if (current->size > bytes + BLOCK_SIZE) {
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
      if (previous == NULL) {
        freeList = current->next;
      } else {
        previous->next = current->next;
      }
      return (void *) ((char *) current + BLOCK_SIZE);
    }
    previous = current;
  }
  return NULL;
}

static void coalesce_free_blocks() {
  struct block *current = freeList;
  while (current != NULL && current->next != NULL) {
    if ((char *) current + current->size == (char *) current->next) {
      current->size += current->next->size;
      current->next = current->next->next;
    } else {
      current = current->next;
    }
  }
}

// Free memory
void free(void *ptr) {
  if (ptr == NULL) {
    return;
  }
  struct block *blockToFree = (struct block *) ((char *) ptr - BLOCK_SIZE);

  if (blockToFree->size == 0 || blockToFree->size > memInfo.totalSize) {
    return;
  }

  memInfo.freeSize += blockToFree->size - BLOCK_SIZE;
  memInfo.allocatedSize -= (blockToFree->size - BLOCK_SIZE);

  blockToFree->next = freeList;
  freeList = blockToFree;

  coalesce_free_blocks();
}

memoryInfo *get_memory_info() { return &memInfo; }

#endif