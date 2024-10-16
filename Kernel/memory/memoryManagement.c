// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "memoryManagement.h"

// #ifndef LIST


static void *splitBlocks(struct block *block, size_t size);
static void mergeBlocks();


static struct block *freeList; 
static struct memStats stats;


void init_memory_manager(void * startHeapAddress, size_t totalSize) {
	freeList = (struct block *)startHeapAddress;
    // Set the size of the first block
    freeList->size = totalSize - BLOCK_SIZE; // Subtract size of the block header
    freeList->isFree = true;
    freeList->next = NULL; // No other blocks yet
    freeList->prev = NULL;

    stats.totalMem = totalSize;
    stats.freeMem = freeList->size;
    stats.allocatedMem = 0;
    stats.totalBlocks = 1;
    stats.freeBlocks = 1;

}

void *malloc(size_t bytes) {
    if (bytes == 0) return NULL; // nothing to allocate duh

    struct block * current = freeList;
    size_t alignedBytes = ALIGN(bytes);
    bytes += BLOCK_SIZE; // Account for block header

    // Search for block
    while (current) {
        if (current->isFree && current->size >= alignedBytes) {
            void * result = splitBlocks(current, alignedBytes);
            if (result) {
                stats.freeMem -= (alignedBytes + BLOCK_SIZE);
                stats.allocatedMem += (alignedBytes + BLOCK_SIZE);
                stats.freeBlocks--;
                stats.totalBlocks++;
            }
            return result; // Return the memory address
        }
        current = current->next; 
    }
    return NULL; // No suitbale block found
}

// Free memory
void free(void * ptr) {
    if (ptr == NULL) return; 

    struct block * blockToFree = (struct block *) ((char *) ptr - BLOCK_SIZE);

    if (blockToFree < freeList || blockToFree >= (struct block*)((char *)freeList + stats.totalMem)) {
        // pointer outside of range :: error 
        // print("Error freeing memory: pointer outside of range\n");
        return;
    }

    blockToFree->isFree = true;
    stats.freeMem += blockToFree->size + BLOCK_SIZE;
    stats.allocatedMem -= (blockToFree->size + BLOCK_SIZE);
    stats.freeBlocks++;
    // blockToFree->next = freeList;
    // freeList = blockToFree; // Add to the front of the free list
    mergeBlocks();
}

struct memStats getMemStats() {
    return stats; 
}

static void * splitBlocks(struct block *block, size_t size) {
    if (block->size >= size + BLOCK_SIZE + ALIGNMENT) { 
        struct block * newBlock = (struct block*)((char*)block + BLOCK_SIZE + size);
        newBlock->size = block->size - size - BLOCK_SIZE; 
        newBlock->isFree = true; 
        newBlock->next = block->next; 
        newBlock->prev = block;

        if (block->next == NULL) {
            block->next->prev = newBlock; 
        }

        block->size = size; 
        block->next = newBlock;
    }

    block->isFree = false;
    return (void *)((char *)block + BLOCK_SIZE);
}

static void mergeBlocks() {
    struct block *current = freeList;

    while (current && current->next) {
        if (current->isFree && current->next->isFree) {
            current->size += (current->next->size + BLOCK_SIZE);
            current->next = current->next->next;
            if (current->next) {
                current->next->prev = current;
            }
            stats.totalBlocks--;
            stats.freeBlocks--;
        } else {
            current = current->next;
        }
    }
}
// #endif