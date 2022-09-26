
#include "umalloc.h"
#include "csbrk.h"

//Place any variables needed here from umalloc.c or csbrk.c as an extern.
extern memory_block_t *free_head;
extern sbrk_block *sbrk_blocks;

/*
 * check_heap -  used to check that the heap is still in a consistent state.
 
 * STUDENT TODO:  
 * Required to be completed for checkpoint 1:
 *      - Check that pointers in the free list point to valid free blocks. Blocks should be within the valid heap addresses: look at csbrk.h for some clues.
 *        They should also be allocated as free.
 *      - Check if any memory_blocks (free and allocated) overlap with each other. Hint: Run through the heap sequentially and check that
 *        for some memory_block n, memory_block n+1 has a sensible block_size and is within the valid heap addresses.
 *      - Ensure that each memory_block is aligned. 
 * 
 * Should return 0 if the heap is still consistent, otherwise return a non-zero
 * return code. Asserts are also a useful tool here.
 */
int check_heap() {
    // Example heap check:
    // Check that all blocks in the free list are marked free.
    // If a block is marked allocated, return -1.
    /*
        memory_block_t *cur = free_head;
        while (cur) {
            if (is_allocated(cur)) {
                return -1;
            }
        }
    */
    // check that there are no allocated blocks in the free list.
    // - this means that in the header all blocks should point to the next block in list or be NULL (reached end of list)
    // all blocks in the list should be within the valid heap address (given from the sbrk_block)
    uint64_t heap_size = (*sbrk_blocks).sbrk_end;
    memory_block_t* current = *free_header;
    while((*current).next != NULL) {
        // Check if there are any allocated blocks in the free list.
        if (is_allocated(current)) {
            return -1;
        }
        // if current.size + (the ptr address of current) > heap_size, then return -1
        // out of the valid heap address
        if (&(*current) + 16 + get_size(current) > heap_size) {
            return -1;
        }
        current = get_next(current);
    }

    return 0;
}