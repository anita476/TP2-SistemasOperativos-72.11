/* We'll be using Kernighans implementation of a memory manager using a simple linked list -> put in report */

#include <stdint.h>
#include <stdlib.h>

typedef long ALIGN;

/* Kernighan section 8.7*/
union header {
    struct {
        union header * ptr;
        uint64_t size;
    } s;
    ALIGN x;
};

typedef union header Header;

#define BLOCKSIZE sizeof(Header)

/*
    @brief Initializes s bytes, starting in p
    @param p Pointer to the start of the asigned memory
    @param s Size of the asigned memory
*/
//void memoryInit(void * p, size_t s);
void init_memory_manager(size_t totalSize);

/*
    @brief Allocates the specified number of bytes in heap memory
    @param bytes Number of bytes to be asigned
    @returns Pointer to the assigned bytes, or NULL if memory could not
    be allocated
*/
//void * malloc(uint64_t bytes);
void *malloc(size_t size);
    
/*
    @brief Frees the specified memory block
    @param p Pointer to block to be freed
*/
//void free(void * p);
void free(void *ptr);