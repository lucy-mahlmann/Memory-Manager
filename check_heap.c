
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
   
    memory_block_t* current = free_head;
    while(current != NULL) {
        // Check if there are any allocated blocks in the free list.
        if (is_allocated(current)) {
            return -1;
        }
        // if current.size + (the ptr address of current) > heap_size, then return -1
        // out of the valid heap address
        if (check_malloc_output(current, get_size(current) + 16) == -1) {   
            return -1;
        }
        // chech if block is 16 bit aligned
        if ((long) current % 16 != 0) {
            return -1;
        }
        // check if overlapping
        if (!((long) current + get_size(current) + 16 <= (long) get_next(current))) {
            return -1;
        }
        current = get_next(current);
    }

    // check that no memory_blocks (free or allocated) overlap
    // Run through the heap sequentially and check that for some memory_block n, memory_block n+1 
    // has a sensible block_size and is within the valid heap addresses.
    // while (current <= heap_size) {
    //     size_t curr_size = get_size(current);
    //     memory_block_t* next_block = current + 1; // get memory_block n+1
    //     size_t next_size = get_size(next_block);
    //     if (current + curr_size > next) {
    //         return -1;
    //     }
    //     current = next_block;
    // }



    //curr + size <= start of next block //to check overlapping
    // curr % 16 != 0 return -1

    // not overlapping check if the ending address of current is less than the starting address of next
    // checking within the heap, see what the minimum value that csbreak returns and then see what highest value 
    // csbreak returns and then make sure current is within these two values 
    // keep two global variables to what csbrk min and max values are, update when you extend/unit 
   
    // to get to the end of a block is curr + pagesize -16 + 16


    return 0;
}