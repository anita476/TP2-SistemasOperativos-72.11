
#include <memoryManagement.h>

#ifdef BUDDY

#define BLOCK_SIZE sizeof(struct block)
#define DEBUG_MEMORY 1

struct block {
    size_t size;         // Size of the block
    struct block *next;  // Pointer to the next free block
    #if DEBUG_MEMORY
    int allocated;       // Flag to track if block is allocated
    size_t requested_size; // Original requested size
    #endif
};

static struct block *freeList;
static size_t total_allocations = 0;
static size_t total_frees = 0;
static size_t current_allocated_memory = 0;

void init_memory_manager(void *startHeapAddress, size_t totalSize) {
    freeList = (struct block *)startHeapAddress;
    freeList->size = totalSize - BLOCK_SIZE;
    freeList->next = NULL;
    #if DEBUG_MEMORY
    freeList->allocated = 0;
    freeList->requested_size = 0;
    #endif
    
    current_allocated_memory = 0;
    total_allocations = 0;
    total_frees = 0;
}

void *malloc(size_t bytes) {
    if (bytes == 0) return NULL;
    
    struct block *current, *previous;
    size_t total_required = bytes + BLOCK_SIZE;
    
    #if DEBUG_MEMORY
    print("Attempting to allocate ");
    char buffer[20];
    intToStr(bytes, buffer, 10);
    print(buffer);
    print(" bytes\n");
    #endif

    previous = NULL;
    for (current = freeList; current != NULL; current = current->next) {
        if (current->size >= total_required) {
            // Found a block big enough
            if (current->size >= total_required + BLOCK_SIZE + sizeof(struct block)) {
                // Split the block if there's enough space for a new header
                struct block *newBlock = (struct block *)((char *)current + total_required);
                newBlock->size = current->size - total_required;
                newBlock->next = current->next;
                #if DEBUG_MEMORY
                newBlock->allocated = 0;
                newBlock->requested_size = 0;
                #endif
                
                current->size = total_required;
                current->next = newBlock;
            }

            // Remove block from free list
            if (previous == NULL) {
                freeList = current->next;
            } else {
                previous->next = current->next;
            }

            #if DEBUG_MEMORY
            current->allocated = 1;
            current->requested_size = bytes;
            total_allocations++;
            current_allocated_memory += current->size;
            
            print("Successfully allocated memory at address ");
            intToStr((size_t)((char *)current + BLOCK_SIZE), buffer, 16);
            print(buffer);
            print("\n");
            #endif

            return (void *)((char *)current + BLOCK_SIZE);
        }
        previous = current;
    }

    #if DEBUG_MEMORY
    print("Failed to allocate memory of size ");
    intToStr(bytes, buffer, 10);
    print(buffer);
    print("\n");
    #endif

    return NULL;
}

void free(void *ptr) {
    if (ptr == NULL) return;

    struct block *blockToFree = (struct block *)((char *)ptr - BLOCK_SIZE);
    
    #if DEBUG_MEMORY
    if (!blockToFree->allocated) {
        print("WARNING: Attempting to free already freed memory!\n");
        return;
    }
    blockToFree->allocated = 0;
    total_frees++;
    current_allocated_memory -= blockToFree->size;
    #endif

    // Add to front of free list
    blockToFree->next = freeList;
    freeList = blockToFree;
}

void memory_manager_state() {
    size_t free_memory = 0;
    size_t largest_block = 0;
    int free_blocks = 0;
    struct block *current = freeList;
    
    while (current != NULL) {
        free_memory += current->size;
        if (current->size > largest_block) {
            largest_block = current->size;
        }
        free_blocks++;
        current = current->next;
    }

    char buffer[32];
    
    print("\n=== Memory Manager State ===\n");
    
    print("Free memory: ");
    intToStr(free_memory, buffer, 10);
    print(buffer);
    print(" bytes\n");
    
    print("Largest free block: ");
    intToStr(largest_block, buffer, 10);
    print(buffer);
    print(" bytes\n");
    
    print("Number of free blocks: ");
    intToStr(free_blocks, buffer, 10);
    print(buffer);
    print("\n");
    
    #if DEBUG_MEMORY
    print("Total allocations: ");
    intToStr(total_allocations, buffer, 10);
    print(buffer);
    print("\n");
    
    print("Total frees: ");
    intToStr(total_frees, buffer, 10);
    print(buffer);
    print("\n");
    
    print("Currently allocated: ");
    intToStr(current_allocated_memory, buffer, 10);
    print(buffer);
    print(" bytes\n");
    #endif
    
    print("========================\n");
}

#endif