#ifndef MEMORY_MANAGEMENT_H
#define MEMORY_MANAGEMENT_H

/* We'll be using Kernighans implementation of a memory manager using a simple linked list  */

// #include <stdint.h>
// #include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
// #include "videoDriver.h"


// typedef long ALIGN;


struct block {
    size_t size;          // Size of the block
    struct block * next;  // Pointer to the next free block
    struct block * prev; 
    bool isFree;
};

#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))
#define BLOCK_SIZE ALIGN(sizeof(struct block))
#define HEADER_SIZE sizeof(UHeader) 

struct memStats {
    size_t totalMem;
    size_t freeMem;
    size_t allocatedMem;
    size_t totalBlocks;
    size_t freeBlocks;
};

/* Kernighan section 8.7*/



/*
    @brief Initializes totalSize bytes, starting in startHeapAdress
    @param startHeapAddress Pointer to the start of the asigned memory
    @param totalSize Size of the asigned memory
*/
void init_memory_manager(void * startHeapAddress, size_t totalSize);

/*
    @brief Allocates the specified number of bytes in heap memory
    @param bytes Number of bytes to be asigned
    @returns Pointer to the assigned bytes, or NULL if memory could not
    be allocated
*/
void *malloc(size_t bytes);
    
/*
    @brief Frees the specified memory block
    @param ptr Pointer to block to be freed
*/
void free(void *ptr);

struct memStats getMemStats();




#endif 