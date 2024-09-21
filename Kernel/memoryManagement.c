#include <memoryManagement.h>
#include <unistd.h>
#include <videoDriver.h>

#include <stdint.h> // for uintptr_t

#define BLOCK_SIZE sizeof(struct block)

// Define the block structure
struct block {
    size_t size;          // Size of the block
    struct block *next;   // Pointer to the next free block
};

// Initialize the free list at the specified start address
static struct block *freeList = (struct block *)0x00F00000;

// Function to initialize the memory manager
void init_memory_manager(size_t totalSize) {
    // Set the size of the first block
    freeList->size = totalSize - BLOCK_SIZE; // Subtract size of the block header
    freeList->next = NULL; // No other blocks yet
}

// Allocate memory
void *malloc(size_t size) {
    struct block *current, *previous;
    size += BLOCK_SIZE; // Account for block header

    // Search for a suitable block
    previous = NULL;
    for (current = freeList; current != NULL; current = current->next) {
        if (current->size >= size) { // Found a big enough block
            if (current->size > size + BLOCK_SIZE) {
                // Split the block
                struct block *newBlock = (struct block *)((char *)current + size);
                newBlock->size = current->size - size;
                newBlock->next = current->next;
                current->size = size;
                current->next = newBlock;
            }

            // Remove from free list
            if (previous == NULL) {
                freeList = current->next;
            } else {
                previous->next = current->next;
            }
            return (void *)((char *)current + BLOCK_SIZE); // Return memory address
        }
        previous = current;
    }

    return NULL; // No suitable block found
}

// Free memory
void free(void *ptr) {
    struct block *blockToFree = (struct block *)((char *)ptr - BLOCK_SIZE);
    blockToFree->next = freeList;
    freeList = blockToFree; // Add to the front of the free list
}












/* 
static Header * base = NULL; 				// Base of memory list
static Header * freeList = NULL; 	// List of free pointers

void memoryInit(void * p, size_t s) {
	base = p;
	base->s.ptr = freeList;
	freeList->s.size = s - BLOCKSIZE;
	freeList->s.ptr = NULL;
}
	
void * malloc(uint64_t bytes) {
	Header * currentPointer, * prevPointer;
	unsigned numberOfBlocks;
	// Starts at base
	prevPointer = base;
	numberOfBlocks = (bytes + BLOCKSIZE - 1) / BLOCKSIZE + 1;
	for (currentPointer = prevPointer->s.ptr; ; (prevPointer = currentPointer), currentPointer = currentPointer->s.ptr) {
		// If there is enough space for the malloc
		if (currentPointer->s.size >= numberOfBlocks) {
			// Exact space
			if (currentPointer->s.size == numberOfBlocks) {
				print("Exact space\n");
				prevPointer->s.ptr = currentPointer->s.ptr;
			}
			// More space than necessary
			else {
				print("More space than necessary\n");
				currentPointer->s.size -= numberOfBlocks;
				currentPointer += currentPointer->s.size;
				currentPointer->s.size = numberOfBlocks;
			}
			freeList = prevPointer;	
			print("Returning pointer\n");
			return(void *) currentPointer + 1; // Returns what is next to the header (where user will put info)
		}
		// There was no space
		if (currentPointer == freeList) {
			print("No space left\n");
			return NULL; // For now...
		}
	}
	return NULL;
}

void free(void * p) {
	Header * basePointer, * currentPointer;
	basePointer = (Header *) p - 1;		// After the header
	for (currentPointer = freeList; !(basePointer > currentPointer && basePointer < currentPointer->s.ptr); currentPointer = currentPointer->s.ptr) {
		print("In for\n");
		if (currentPointer >= currentPointer->s.ptr && (basePointer > currentPointer || basePointer < currentPointer->s.ptr)) {
			print("Freeing memory\n");
			break;
		}
	}
	// Checks if freed block can be merged with next block in memory
	if (basePointer + basePointer->s.size == currentPointer->s.ptr) {
		print("Merging with next block\n");
		basePointer->s.size += currentPointer->s.ptr->s.size;
		basePointer->s.ptr = currentPointer->s.ptr->s.ptr;
	}
	else {
		basePointer->s.ptr = currentPointer->s.ptr;
		print("Not merging with next block\n");
	}
	// Checks if freed block can be merged with previous block in memory
	if (currentPointer + currentPointer->s.size == basePointer) {
		print("Merging with previous block\n");
		currentPointer->s.size += basePointer->s.size;
		currentPointer->s.ptr = basePointer->s.ptr;
	}
	else {
		currentPointer->s.ptr = basePointer;
	}
	freeList = currentPointer;
	print("Free list updated\n");
}	 */