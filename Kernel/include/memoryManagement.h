/* We'll be using Kernighans implementation of a memory manager using a simple linked list  */
#ifndef MEMORY_MANAGEMENT_H
#define MEMORY_MANAGEMENT_H

#include <lib.h>
#include <stdint.h>
#include <stdlib.h>
#include <videoDriver.h>

typedef long ALIGN;

/* Kernighan section 8.7*/

#define BLOCKSIZE sizeof(UHeader)

typedef struct memoryInfo {
  size_t totalSize;
  size_t freeSize;
  size_t allocatedSize;
} memoryInfo;

/*
    @brief Initializes totalSize bytes, starting in startHeapAdress
    @param startHeapAddress Pointer to the start of the asigned memory
    @param totalSize Size of the asigned memory
*/
void init_memory_manager(void *startHeapAddress, size_t totalSize);

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

/*
    @brief Gets the current memory state: total, free, and allocated memory
*/
memoryInfo *get_memory_info();

#endif