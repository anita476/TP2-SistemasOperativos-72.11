#include <memoryManagement.h>
#include <stdbool.h>

#ifdef BUDDY

// Must be power of 2
#define MIN_BLOCK_SIZE 64
#define MAX_ORDER 26 // (~128MB)
#define BLOCK_HEADER_SIZE sizeof(block_t)

typedef struct block {
    size_t size;     // Total size of block including header
    // unsigned char order;       // Level in the buddy system (0 = largest)
    unsigned char is_free;   // 1 if allocated, 0 if free
    struct block *next;  // Next block in free list
} block_t; 

typedef struct {
    void *start;
    block_t *free_lists[MAX_ORDER];
    size_t total_size; 
    size_t free_memory; 
} buddy_t; 

static buddy_t buddy; 

static inline bool is_power_of_2(size_t x) {
    return (x & (x - 1)) == 0;
}

static size_t next_power_of_2(size_t size) {
    size--;
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    size |= size >> 32;
    return size + 1;
}

static int get_order(size_t size) {
    int order = 0;
    size = size - 1;
    while (size >>= 1) order++;
    return order;
}

static block_t* get_buddy(block_t* block) {
    size_t offset = (char*)block - (char*)buddy.start;
    // If offset divisible by block_size * 2, buddy is after, else before
    if ((offset & (block->size * 2 - 1)) == 0) {
        return (block_t*)((char*)block + block->size);
    } else {
        return (block_t*)((char*)block - block->size);
    }
}

void init_memory_manager(void *startHeapAddress, size_t totalSize) {
    char buffer[32];
    
    print("Initializing buddy allocator of size ");
    intToStr(totalSize, buffer, 10);
    print(buffer);
    print(" bytes\n");

    buddy.start = startHeapAddress;
    buddy.total_size = totalSize;
    buddy.free_memory = totalSize;
    for (int i = 0; i < MAX_ORDER; i++) {
        buddy.free_lists[i] = NULL;
    }
    size_t block_size = MIN_BLOCK_SIZE; 
       while ((block_size * 2) <= totalSize) {
        block_size *= 2;
    }

    block_t *initial = (block_t *)startHeapAddress;
    initial->size = block_size;
    initial->is_free = 1;

    int order = get_order(block_size);
    initial->next = NULL;
    buddy.free_lists[order] = initial;

    print("Initial block size: ");
    intToStr(block_size, buffer, 10);
    print(buffer);

    print("     Initialized with order: ");
    intToStr(order, buffer, 10);
    print(buffer);
    print("\n");
}

void *malloc(size_t size) {
    char buffer[32];
    print("\nMalloc request for size: ");
    intToStr(size, buffer, 10);
    print(buffer);
    print("\n");

    size_t required = size + BLOCK_HEADER_SIZE; // let's see if it works without this 
    if (required < MIN_BLOCK_SIZE) required = MIN_BLOCK_SIZE;
    required = next_power_of_2(required);

    // print("Required block size after rounding: ");
    // intToStr(required, buffer, 10);
    // print(buffer);
    // print("\n");

    int order = get_order(required);

    int current_order = order; 
    block_t* block = NULL; 

    while (current_order < MAX_ORDER && block==NULL) {
        if (buddy.free_lists[current_order] != NULL) {
            block = buddy.free_lists[current_order]; 
            buddy.free_lists[current_order] = block->next;
            break; 
        }
        current_order++; 
    }

    if (block == NULL) {
        print("No suitable block found\n");
        return NULL;
    }

    // print("Found block of size: ");
    // intToStr(block->size, buffer, 10);
    // print(buffer);
    // print("\n");

    // split blocks until desired size 
    while (current_order > order) {
        current_order--; 
        size_t new_size = block->size / 2; 

        block_t* buddy_block = (block_t*)((char*)block + new_size);
        buddy_block->size = new_size; 
        buddy_block->is_free = 1;

        buddy_block->next = buddy.free_lists[current_order];
        buddy.free_lists[current_order] = buddy_block;
        block->size = new_size;
    }
    // found the best fit 
    block->is_free = 0;
    buddy.free_memory -= block->size;
    return (void*)(block + 1);  // (block+BLOCK_HEADER_SIZE)
}


void free(void *ptr) {
    if (!ptr) return; 

    block_t* block = ((block_t*)ptr) - 1; // start block from the beginnig of the block header

    // invalid pointer
    if ((void*)block < buddy.start || 
        (void*)block >= (void*)((char*)buddy.start + buddy.total_size)) {
        return;  
    }

    block->is_free = 1; 
    buddy.free_memory += block->size;

    while (1) {
        block_t* buddy_block = get_buddy(block);
        if ((void*)buddy_block >= buddy.start + buddy.total_size || 
            (void*)buddy_block < buddy.start || 
            !buddy_block->is_free || 
            buddy_block->size != block->size) {
            break; 
        }

        // remove buddy from list 
        block_t** current = &buddy.free_lists[get_order(block->size)];
        while (*current != buddy_block) {
            current = &(*current)->next;
        }
        *current = buddy_block->next; 

        if (buddy_block < block) {
            block = buddy_block; 
        }
        block->size *= 2;
    }

    int order = get_order(block->size);
    block->next = buddy.free_lists[order];
    buddy.free_lists[order] = block;
}


void memory_manager_state() {
    char buffer[32];
    print("\n=== Memory Manager State ===\n");
    
    size_t total_free = 0;
    for (int i = 0; i < MAX_ORDER; i++) {
        size_t block_size = (size_t)1 << (i + 6);  
        int count = 0;
        block_t *current = buddy.free_lists[i];
        
        while (current != NULL) {
            count++;
            total_free += current->size;
            current = current->next;
        }
        
        if (count > 0) {
            print("Order ");
            intToStr(i, buffer, 10);
            print(buffer);
            print(" (");
            intToStr(block_size, buffer, 10);
            print(buffer);
            print(" bytes): ");
            intToStr(count, buffer, 10);
            print(buffer);
            print(" free blocks\n");
        }
    }
    
    print("\nTotal memory: ");
    intToStr(buddy.total_size, buffer, 10);
    print(buffer);
    print(" bytes\n");
    
    print("Free memory: ");
    intToStr(buddy.free_memory, buffer, 10);
    print(buffer);
    print(" bytes\n");

    print("Allocated memory: ");
    intToStr(buddy.total_size - buddy.free_memory, buffer, 10);
    print(buffer);
    print(" bytes\n");
    print("========================\n");

}

#endif