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
        
        -Finds first block in the list that is large enough to fit the allocated data amount but never give
        the free head to be allocated so always go to the next block after the free head. If there is not a free 
        block big enough for the size the user is asking for then extend the heap by size and give that block to 
        be allocated.
 */

/*
 * find - finds a free block that can satisfy the umalloc request.
 */
memory_block_t *find(size_t size) {
    // size includes the header
    //? STUDENT TODO
    memory_block_t* curr_block = free_head;
    curr_block = free_head->next; // never allocate free_head
    memory_block_t* prev_block = free_head;
    while (curr_block != NULL) {
        size_t curr_size = get_size(curr_block) + 16; 
        if (size <= curr_size) {
            if (size < curr_size) {
                return split(curr_block, size);
            }
            prev_block->next = get_next(curr_block); // removes the allocated block from the free list
            return curr_block;
        }
        curr_block = get_next(curr_block);
        prev_block = curr_block;
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
    // extends heap if possible by size
    // immediately sends this block to user to allocate therefore don't have to add to free list
    memory_block_t* new_block = (memory_block_t*) csbrk(size);
    put_block(new_block, size - 16, true);
    return new_block;
}

/*
 *  STUDENT TODO:
 *      Describe how you chose to split allocated blocks. Always? Sometimes? Never? Which end?
       
       - Always split the allocated blocks using the high end of the block as the part that will be 
        allocated and the low end will stay a free block.
*/

/*
 * split - splits a given block in parts, one allocated, one free.
 */
memory_block_t *split(memory_block_t *block, size_t size) { // size includes the header
    //? STUDENT TODO
    // update size in original free block
    block->block_size_alloc = get_size(block) - size;
    // make new memory block for the allocated block
    memory_block_t* allocated_block = (memory_block_t*)((long) block + get_size(block) + 16);
    put_block(allocated_block, size - 16, true);
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
    memory_block_t* free_head_block = (memory_block_t*)csbrk(PAGESIZE);
    free_head = free_head_block;
    // put_block(free_head_block, PAGESIZE - 16, false);
    // free_head_block->next = NULL;

    put_block(free_head_block, 32, false);
    memory_block_t* usable_memory = free_head_block + 2;
    put_block(usable_memory, PAGESIZE - 48, false);
    free_head_block->next = usable_memory;
    usable_memory->next = NULL;

    return 0;
}

/*
 * umalloc -  allocates size bytes and returns a pointer to the allocated memory.
 */
void *umalloc(size_t size) {
    //* STUDENT TODO
    size_t payload_size = ALIGN(size);
    memory_block_t* curr_block = find(payload_size + 16);
    allocate(curr_block);
    return get_payload(curr_block); // return the ptr that is just the payload (doesn't include the header)
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