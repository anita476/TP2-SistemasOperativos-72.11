#include <memoryManagement.h>
#include <unistd.h>

static Header base; 			// Base of memory list
static Header * freep = NULL; 	// Empty list start
int memorySize;
int totalMemory;
int usedMemory;
int numberOfBlocks;

void memoryInit(void * p, int s) {
	void * memoryStart = p;

	base.s.ptr = p;
	s -= (memoryStart -  p);
	totalMemory = s;
	usedMemory = sizeof(Header);
	numberOfBlocks = 1;

	

}

void * malloc(uint64_t bytes) {
	Header * p, * prevp;
	
}

void free(void * p) {
}