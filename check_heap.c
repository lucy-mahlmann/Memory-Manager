
#include "umalloc.h"
#include "csbrk.h"

//Place any variables needed here from umalloc.c or csbrk.c as an extern.
extern memory_block_t *free_head;
extern memory_block_t* lowest_heap;
extern memory_block_t* highest_heap;
extern sbrk_block *sbrk_blocks;

/*
 * check_heap -  used to check that the heap is still in a consistent state.
 *  - Check that pointers in the free list point to valid free blocks. 
      Blocks should be within the valid heap addresses
 *  - Check if any memory_blocks (free) overlap with each other. 
 *  - Ensure that each memory_block is 16 bit aligned. 
 * Return 0 if the heap is still consistent, otherwise return -1.
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