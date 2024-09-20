/* We'll be using Kernighans implementation of a memory manager using a simple linked list -> put in report */

#include <stdint.h>

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
void memoryInit(void * p, int s);

/*
    @brief Allocates the specified number of bytes in heap memory
    @param bytes Number of bytes to be asigned
    @returns Pointer to the assigned bytes, or NULL if memory could not
    be allocated
*/
void * malloc(uint64_t bytes);
    
/*
    @brief Frees the specified memory block
    @param p Pointer to block to be freed
*/
void free(void * p);