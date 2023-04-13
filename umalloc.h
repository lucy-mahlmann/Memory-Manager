#include <stdlib.h>
#include <stdbool.h>

#define ALIGNMENT 16 /* The alignment of all payloads returned by umalloc */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))

/*
 * memory_block_t - Represents a block of memory managed by the heap. 
 * bit0 - 3 are used for the header
 * and the remaining 60 bit represent the size.
 */
typedef struct memory_block_struct {
    size_t block_size_alloc; // This is the size of the block minus the header (16 bits).
    struct memory_block_struct *next;
} memory_block_t;


// Makes sure that the block is not NULL and then returns true if the bit0 
// of block equals 1 (is a allocated) otherwise returns false if the bit0 
// of block equals 0 (is a free block).
bool is_allocated(memory_block_t *block);
// Checks if the block is not NULL and then changes the bit0 of 
// block to a 1. This sets it as an allocated block.
void allocate(memory_block_t *block);
// Checks if the block is not NULL and then changes the bit0 of 
// block to a 0. This sets the block as a free block.
void deallocate(memory_block_t *block);
// Checks if the block is not NULL and returns the value that is stored in 
// the memory_block_t->block_size_alloc variable. This is the value that 
// represents the size of the block minues the metadata that holds the 
// header (which is 16bits). 
size_t get_size(memory_block_t *block);
// Checks if the block is not NULL and then dereferences the block pointer 
// to access the next field and returns the memory_block_t pointer to the 
// next block in the free list or NULL if it is the last free block in the free list.
memory_block_t *get_next(memory_block_t *block);
// Puts a memory_block_t struct into the memory address that is specified 
// by the pointer block and sets the block_size_alloc variable as the size 
// and sets the bit0 of the header to 1 (if allocated) or 0 (if free).
void put_block(memory_block_t *block, size_t size, bool alloc);
// Checks if block is not NULL and returns a void pointer to the start of 
// the payload in the given memory_block_t block. This is the part of the 
// block that does not include the header and is given to the user.
void *get_payload(memory_block_t *block);
// Checks if the payload is not NULL and returns a memory_block_t pointer
// of the block that has the corresponding payload given from the void 
// pointer payload.
memory_block_t *get_block(void *payload);


// Find the first block that will fit the size that the user has requested 
// in umalloc, if none is found then call extend to extend the 
// heap by size. Then if necessary remove the found block from free list, 
// split the found block, and return the block.
memory_block_t *find(size_t size);
// Use csbrk to get a block of memory of size and immediately return that 
// block to be given to the user in umalloc.
memory_block_t *extend(size_t size);
// Always split the given block, using the high end of the block as the 
// part that will be allocated and the low end will stay a free block.
memory_block_t *split(memory_block_t *block, size_t size);
// Join free blocks that are right next to each other in memory. 
// Coalesce if possible right after you free an allocated block in memory.
memory_block_t *coalesce(memory_block_t *block);
// Checks if the target block can be coalesced with the blocks previous 
// to it and after it in the free list.
void ufree_check_coalescing(memory_block_t* prev, memory_block_t* target); 

int uinit();
void *umalloc(size_t size);
void ufree(void *ptr);