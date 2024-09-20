#include <memoryManagement.h>
#include <unistd.h>
#include <videoDriver.h>

static Header * base; 				// Base of memory list
static Header * freeList = NULL; 	// List of free pointers

void memoryInit(void * p, int s) {
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
				prevPointer->s.ptr = currentPointer->s.ptr;
			}
			// More space than necessary
			else {
				currentPointer->s.size -= numberOfBlocks;
				currentPointer += currentPointer->s.size;
				currentPointer->s.size = numberOfBlocks;
			}
			freeList = prevPointer;
			return(void *) currentPointer + 1; // Returns what is next to the header (where user will put info)
		}
		// There was no space
		if (currentPointer == freeList) {
			return NULL; // For now...
		}
	}
	return NULL;
}

void free(void * p) {
	Header * basePointer, * currentPointer;
	basePointer = (Header *) p - 1;		// After the header
	for (currentPointer = freeList; !(basePointer > currentPointer && basePointer < currentPointer->s.ptr); currentPointer = currentPointer->s.ptr) {
		if (currentPointer >= currentPointer->s.ptr && (basePointer > currentPointer || basePointer < currentPointer->s.ptr)) {
			break;
		}
	}
	// Checks if freed block can be merged with next block in memory
	if (basePointer + basePointer->s.size == currentPointer->s.ptr) {
		basePointer->s.size += currentPointer->s.ptr->s.size;
		basePointer->s.ptr = currentPointer->s.ptr->s.ptr;
	}
	else {
		basePointer->s.ptr = currentPointer->s.ptr;
	}
	// Checks if freed block can be merged with previous block in memory
	if (currentPointer + currentPointer->s.size == basePointer) {
		currentPointer->s.size += basePointer->s.size;
		currentPointer->s.ptr = basePointer->s.ptr;
	}
	else {
		currentPointer->s.ptr = basePointer;
	}
	freeList = currentPointer;
}	