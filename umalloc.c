#include "umalloc.h"
#include "csbrk.h"
#include "ansicolors.h"
#include <stdio.h>
#include <assert.h>

const char author[] = ANSI_BOLD ANSI_COLOR_RED "Lucy Mahlmann lam6744" ANSI_RESET;

/*
 * The following helpers can be used to interact with the memory_block_t
 * struct, they can be adjusted as necessary.
 */

// A sample pointer to the start of the free list.
memory_block_t *free_head;

/*
 * is_allocated - returns true if a block is marked as allocated.
 */
bool is_allocated(memory_block_t *block) {
    assert(block != NULL); // assert() if value evaulates to false then an error message is printed out
    return block->block_size_alloc & 0x1;
}

/*
 * allocate - marks a block as allocated.
 */
void allocate(memory_block_t *block) {
    assert(block != NULL);
    block->block_size_alloc |= 0x1;
}


/*
 * deallocate - marks a block as unallocated.
 */
void deallocate(memory_block_t *block) {
    assert(block != NULL);
    block->block_size_alloc &= ~0x1;
}

/*
 * get_size - gets the size of the block.
 */
size_t get_size(memory_block_t *block) {
    assert(block != NULL);
    return block->block_size_alloc & ~(ALIGNMENT-1);
}

/*
 * get_next - gets the next block.
 */
memory_block_t *get_next(memory_block_t *block) {
    assert(block != NULL);
    return block->next;
}

/*
 * put_block - puts a block struct into memory at the specified address.
 * Initializes the size and allocated fields, along with NUlling out the next 
 * field.
 */
void put_block(memory_block_t *block, size_t size, bool alloc) {
    assert(block != NULL);
    assert(size % ALIGNMENT == 0);
    assert(alloc >> 1 == 0);
    block->block_size_alloc = size | alloc; 
    block->next = NULL;
}

/*
 * get_payload - gets the payload of the block.
 */
void *get_payload(memory_block_t *block) {
    assert(block != NULL);
    return (void*)(block + 1);
}

/*
 * get_block - given a payload, returns the block.
 */
memory_block_t *get_block(void *payload) {
    assert(payload != NULL);
    return ((memory_block_t *)payload) - 1;
}

/*
 *  STUDENT TODO:
 *      Describe how you select which free block to allocate. What placement strategy are you using?
        Finds first block in the list that is large enough to fit the allocated data amount
 */

/*
 * find - finds a free block that can satisfy the umalloc request.
 */
memory_block_t *find(size_t size) {
    //? STUDENT TODO
    memory_block_t* curr_block = free_head;
    curr_block = free_head->next; // never allocate free_head
    while (curr_block != NULL) {
        size_t curr_size = get_size(curr_block) + 16; // should be 16??
        if (size <= curr_size) {
            return curr_block;
        }
        curr_block = get_next(curr_block);
    }
    // could not find a spot large enough for the allocated size 
    // call coalence to see if you can merge free blocks together and research 
    return extend(size);
    //return NULL;
}

/*
 * extend - extends the heap if more memory is required.
 */
memory_block_t *extend(size_t size) {
    //? STUDENT TODO
    // extend heap if possible by size
    // make new block for this new memory, setting a header
    // immediately sends this block to user to allocate so you don't have to add to free list
    // mark block as allocated , returns get_payload(block) so they get the memory address
    memory_block_t* new_block = csbrk(size); // should be plus 16??
    put_block(new_block, size, true);
    return new_block;
}

/*
 *  STUDENT TODO:
 *      Describe how you chose to split allocated blocks. Always? Sometimes? Never? Which end?
        Always split the allocated blocks using the high end of the block as the part that will be 
        allocated and the low end will stay a free block.
*/

/*
 * split - splits a given block in parts, one allocated, one free.
 */
memory_block_t *split(memory_block_t *block, size_t size) {
    //? STUDENT TODO
    // check if we even need to split
    if (get_size(block) == size - 16) {
        return block;
    }
    // update size in original block to be get_size(block) - size
    block->block_size_alloc = get_size(block) - size;
    // make new memory block for the allocated block
    // maybe use put_block ??
    memory_block_t* allocated_block = block + get_size(block) + 16;
    allocated_block->block_size_alloc = size - 16;
    allocate(allocated_block);
    // returns the allocated block, add free block to free list
    return allocated_block;
}

/*
 * coalesce - coalesces a free memory block with neighbors.
 */
memory_block_t *coalesce(memory_block_t *block) {
    //? STUDENT TODO
    return NULL;
}



/*
 * uinit - Used initialize metadata required to manage the heap
 * along with allocating initial memory.
 */
int uinit() {
    //* STUDENT TODO
    // call sbrk to get heap amount
    // write in a header (size of heap - 16 and NULL) to make one large free bloc
    memory_block_t* free_head_block = csbrk(PAGESIZE);
    free_head = free_head_block;
    free_head_block->block_size_alloc = 16; // use put_block instead
    memory_block_t* heap_block = get_payload(free_head_block) + 1;
    free_head_block->next = heap_block;
    heap_block->block_size_alloc = PAGESIZE - 3;
    heap_block->next = NULL;

    // 
    return 0;
}

/*
 * umalloc -  allocates size bytes and returns a pointer to the allocated memory.
 */
void *umalloc(size_t size) {
    //* STUDENT TODO
    // first get 16 Bit alignment
    size_t min_size = ALIGN(size + 16);
    memory_block_t* curr_block = find(min_size);
    curr_block = split(curr_block, min_size);
    curr_block->block_size_alloc = size;
    return get_payload(curr_block); // how to get a pointer that is past the header
}

/*
 *  STUDENT TODO:
 *      Describe your free block insertion policy.
*/

/*
 * ufree -  frees the memory space pointed to by ptr, which must have been called
 * by a previous call to malloc.
 */
void ufree(void *ptr) {
    //* STUDENT TODO
    //go through free list until you find a block that has a address greater than this block and keep
    // track of prev block as well and set as next of prev block to this block and this block next to 
    // the block with the address that is greater

}