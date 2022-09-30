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
// A block pointer to the lowest address in the heap
memory_block_t* lowest_heap;
// A block pointer to the highest addres in the heap
memory_block_t* highest_heap;

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
        
        -Finds first block in the list that is large enough to fit the allocated data amount but never gives
        the free head to be allocated so always go to the next block after the free head. If there is not a free 
        block big enough for the size the user is asking for then extend the heap by size and give that block to 
        be allocated.
 */

/*
 * find - finds a free block that can satisfy the umalloc request.
 */
memory_block_t *find(size_t size) {
    // Size does not include the header bits.
    memory_block_t* curr_block = free_head;
    curr_block = free_head->next; // never allocate free_head
    memory_block_t* prev_block = free_head;
    while (curr_block != NULL) { 
        // This keeps the free block right after the free head from being coalesced together.
        if (prev_block != free_head) {
            // Check if blocks can be coalesced (is curr free block right next to prev free block in memory).
            memory_block_t* prev_end_address = (memory_block_t*) ((long) prev_block + 16 + get_size(prev_block));
            // prev and curr are right next to one another in memory.
            if (prev_end_address == curr_block) {
                curr_block = coalesce(prev_block);
            }
        }
        size_t curr_size = get_size(curr_block);
        // Finds the first block that is big enough to satisfy the umalloc request.
        if (size <= curr_size) {
            if (size < curr_size) {
                return split(curr_block, size);
            }
            prev_block->next = get_next(curr_block); // removes the allocated block from the free list
            allocate(curr_block);
            return curr_block;
        }
        prev_block = curr_block;
        curr_block = get_next(curr_block);
    }
    // Could not find a spot large enough for the allocated size.
    // Try coalescing free blocks and search through list again for a large enough block. 
    return extend(size); 
}

/*
 * extend - extends the heap if more memory is required.
 */
memory_block_t *extend(size_t size) {
    // Extends the heap if possible by size.
    memory_block_t* new_block = (memory_block_t*) csbrk(size + 16);
    // Immediately sends this block to user to allocate therefore it doesn't have to be added to the free list.
    put_block(new_block, size, true);

    // update lowest and highest addresses in heap.
    if (new_block < lowest_heap) {
        lowest_heap = new_block;
    } 
    memory_block_t* new_block_end_address = (memory_block_t*)((long) new_block + size + 16);
    if (new_block_end_address > highest_heap) {
        highest_heap = new_block_end_address;
    }
    return new_block;
}

/*
 *  STUDENT TODO:
 *      Describe how you chose to split allocated blocks. Always? Sometimes? Never? Which end?
       
       - Always split the given block using the high end of the block as the part that will be 
        allocated and the low end will stay a free block.
*/

/*
 * split - splits a given block in parts, one allocated, one free.
 */
memory_block_t *split(memory_block_t *block, size_t size) { // size does not include the header
    // Update size in original free block.
    block->block_size_alloc = get_size(block) - size - 16;
    // Make new memory block for the allocated block.
    memory_block_t* allocated_block = (memory_block_t*)((long) block + get_size(block) + 16);
    put_block(allocated_block, size, true);
    return allocated_block;
}

/*
 * coalesce - coalesces a free memory block with neighbors.
 */
memory_block_t *coalesce(memory_block_t *block) {
    //printf("coalescing...\n");
    // Update the size of the block that is being joined together.
    block->block_size_alloc = get_size(block) + get_size(get_next(block)) + 16;
    block->next = get_next(get_next(block));
    return block;
}



/*
 * uinit - Used initialize metadata required to manage the heap
 * along with allocating initial memory.
 */
int uinit() {
    // Creates a header block that is never allocated to the user. Is the start of the free list.
    memory_block_t* free_head_block = (memory_block_t*) csbrk(PAGESIZE);
    free_head = free_head_block;
    put_block(free_head_block, 16, false);
    // Creates a block for the rest of the heap given from csbrk that can be allocated.
    memory_block_t* usable_memory = free_head_block + 2;
    put_block(usable_memory, PAGESIZE - 48, false);
    free_head_block->next = usable_memory;
    usable_memory->next = NULL;

    // Assign pointers to the lowest and highest address in the heap.
    lowest_heap = free_head_block;
    highest_heap = (memory_block_t*)((long) free_head_block + PAGESIZE);

    return 0;
}

/*
 * umalloc -  allocates size bytes and returns a pointer to the allocated memory.
 */
void *umalloc(size_t size) {
    size_t payload_size = ALIGN(size);
    // Find a block within the free list that will satisfy the umalloc request or extend the heap.
    memory_block_t* curr_block = find(payload_size);
    allocate(curr_block);
    return get_payload(curr_block); // return the ptr that is just the payload (doesn't include the header)
}

/*
 *  STUDENT TODO:
 *      Describe your free block insertion policy.
        -The free list is address sorted so insert the block given behind the first free block in the free list
        that has an address greater than it.
*/

/*
 * ufree -  frees the memory space pointed to by ptr, which must have been called
 * by a previous call to malloc.
 */
void ufree(void *ptr) {
    // Get the block address that is associated with the ptr to its payload.
    memory_block_t* target = get_block(ptr);
    deallocate(target);
    memory_block_t* curr = free_head->next;
    memory_block_t* prev = free_head;
    // Search through free list to determine where to add target.
    while (curr != NULL) {
        // Check if address of curr is greater than the addres of target.
        if (curr > target) {
            // Set next of target to curr.
            target->next = curr;
            // Set next of prev to target.
            prev->next = target;
        }
        prev = curr;
        curr = get_next(curr);
        // Reached end of free list, add target to end of list.
        if (curr == NULL) {
            prev->next = target;
            target->next = NULL;
        }
    }
}