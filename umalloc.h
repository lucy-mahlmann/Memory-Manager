#include <stdlib.h>
#include <stdbool.h>

#define ALIGNMENT 16 /* The alignment of all payloads returned by umalloc */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))

/*
 * memory_block_t - Represents a block of memory managed by the heap. The 
 * struct can be left as is, or modified for your design.
 * In the current design bit0 is the allocated bit
 * bits 1-3 are unused.
 * and the remaining 60 bit represent the size.
 */
typedef struct memory_block_struct {
    size_t block_size_alloc;
    struct memory_block_struct *next;
} memory_block_t;

// Helper Functions, this may be editted if you change the signature in umalloc.c

/*
*  STUDENT TODO:
*      Write 1-2 sentences for each function explaining what it does. Don't just repeat the name of the function back to us.
*/

// Makes sure that the block is not NULL and then returns true if the bit0 of block equals 1 (is a allocated) 
// otherwise returns false if the bit0 of block equals 0 (is a free block).
bool is_allocated(memory_block_t *block);
// Checks if the block is not NULL and then changes the bit0 of block to a 1. This sets it as an allocated block.
void allocate(memory_block_t *block);
// Checks if the block is not NULL and then changes the bit0 of block to a 0. This sets the block as a free block.
void deallocate(memory_block_t *block);
// Checks if the block is not NULL and returns the value that is stored in the memory_block_t->block_size_alloc 
// variable. This is the value that represents the size of the block minues the metadata that holds the header. 
size_t get_size(memory_block_t *block);
// Checks if the block is not NULL and then dereferences the block pointer to access the next field
// and returns the memory_block_t pointer to the next block if it is a free block or NULL if last free block in free list.
memory_block_t *get_next(memory_block_t *block);
// Puts a memory_block_t struct into the memory address that is specified by the pointer block and sets the block_size_alloc
// variable as the size and sets the bit0 of the header to 1 (if allocated) or 0 (if free).
void put_block(memory_block_t *block, size_t size, bool alloc);
// Checks if block is not NULL and returns a void pointer to the start of the payload in the given
// memory_block_t block. This is the part of the block that does not include the header and is given to the user.
void *get_payload(memory_block_t *block);
// Checks if the payload is not NULL and returns a memory_block_t pointer of the block that has the corresponding
// payload given from the void pointer payload.
memory_block_t *get_block(void *payload);

// 
memory_block_t *find(size_t size);
//
memory_block_t *extend(size_t size);
//
memory_block_t *split(memory_block_t *block, size_t size);
//
memory_block_t *coalesce(memory_block_t *block);


// Portion that may not be edited
int uinit();
void *umalloc(size_t size);
void ufree(void *ptr);