#include <memoryManagement.h>
#include <unistd.h>

static Header * base; 			// Base of memory list
static Header * freep = NULL; 	// Empty list start


void memoryInit(void * p, int s) {
	base = p;
	base->s.ptr = freep;
	freep->s.size = s - BLOCKSIZE;
	freep->s.ptr = NULL;
}

void * malloc(uint64_t bytes) {
	Header * p, * prevp;
	unsigned nunits; 
	
}

void free(void * p) {
}