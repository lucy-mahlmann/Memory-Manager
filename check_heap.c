
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
 *      - Check if any memory_blocks (free) overlap with each other. Hint: Run through the heap sequentially and check that
 *        for some memory_block n, memory_block n+1 has a sensible block_size and is within the valid heap addresses.
 *      - Ensure that each memory_block is aligned. 
 * 
 * Should return 0 if the heap is still consistent, otherwise return a non-zero
 * return code. Asserts are also a useful tool here.
 */
int check_heap() {
    memory_block_t* current = free_head;
    while(current != NULL) {
        // Check if there are any allocated blocks in the free list.
        if (is_allocated(current)) {
            return -1;
        }
        // check that free blocks are within the valid heap address
        if (check_malloc_output(current, get_size(current) + 16) == -1) {   
            return -1;
        }
        // check if block is 16 bit aligned
        if ((long) current % 16 != 0) {
            return -1;
        }
        // check if current block is overlapping with the next block
        if (!((long) current + get_size(current) + 16 < (long) get_next(current))) {
            return -1;
        }
        current = get_next(current);
    }

    return 0;
}