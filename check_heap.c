
#include "umalloc.h"
#include "csbrk.h"

//Place any variables needed here from umalloc.c or csbrk.c as an extern.
extern memory_block_t *free_head;
extern memory_block_t* lowest_heap;
extern memory_block_t* highest_heap;
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
    memory_block_t* curr = free_head;
    while(curr != NULL) {
        size_t curr_size = get_size(curr) + 16;
        // Check if there are any allocated blocks in the free list.
        if (is_allocated(curr)) {
            return -1;
        }
        // check that free blocks are within the valid heap address
        if (curr < lowest_heap && (memory_block_t*)((long) curr + curr_size) > highest_heap) {   
            return -1;
        }
        // check if block is 16 bit aligned
        if ((long) curr % 16 != 0) {
            return -1;
        }
        // check if current block is overlapping with the next block
        if (get_next(curr) != NULL  && ((long) curr + curr_size > (long) get_next(curr))) {
            return -1;
        }
        curr = get_next(curr);
    }

    return 0;
}