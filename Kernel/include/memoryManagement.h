/* We'll be using Kernighans implementation of a memory manager using a simple linked list  */

#include <stdint.h>
#include <stdlib.h>

typedef long ALIGN;


/* Kernighan section 8.7*/


#define BLOCKSIZE sizeof(UHeader) 

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