#include "umalloc.h"
#include "csbrk.h"
#include "ansicolors.h"
#include <stdio.h>
#include <assert.h>

const char author[] = ANSI_BOLD ANSI_COLOR_RED "Lucy Mahlmann lam6744" ANSI_RESET;


// A pointer to the start of the free list.
memory_block_t *free_head;
// A block pointer to the lowest address in the heap
memory_block_t* lowest_heap;
// A block pointer to the highest address in the heap
memory_block_t* highest_heap;

/*
 * is_allocated - returns true if a block is marked as allocated.
 */
bool is_allocated(memory_block_t *block) {
    assert(block != NULL); 
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
 * find - finds a free block that can satisfy the umalloc request.
 * allocation strategy - finds first block that is equal or larger than 
 * the size that is given. The free head block is never allocated, 
 * always splits blocks that are larger than the size. Extends heap
 * by size if not enough space. Returns allocated block.
 */
memory_block_t *find(size_t size) {
    // Size does not include the header bits.
    memory_block_t* curr = free_head;
    curr = get_next(free_head); // never allocate free_head
    memory_block_t* prev = free_head;
    // Go through the free list.
    while (curr != NULL) { 
        size_t curr_size = get_size(curr);
        // Finds the first block that is big enough to satisfy the umalloc request.
        if (size <= curr_size) {
            // Current block is big enough to satify user request. 
            // Always split if it is bigger.
            if (size < curr_size) {
                return split(curr, size);
            }
            // removes the allocated block from the free list
            prev->next = get_next(curr); 
            allocate(curr);
            return curr;
        }
        prev = curr;
        curr = get_next(curr);
    }
    // Could not find a block large enough for the users 
    // request therefore extend the heap.
    return extend(size); 
}

/*
 * extend - extends the heap if more memory is required.
 */
memory_block_t *extend(size_t size) {
    // Extends the heap if possible by size + 16.
    memory_block_t* new_block = (memory_block_t*) csbrk(size + 16);
    // Immediately sends this block to user to allocate therefore it 
    // doesn't have to be added to the free list.
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
 * split - splits a given block in parts, one allocated, one free.
 * The high end of the given block will be the allocated block,
 * the low end will remain a free block. Returns allocated block.
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
    // Update the size of the block that is being joined together.
    block->block_size_alloc = get_size(block) + get_size(get_next(block)) + 16;
    // Update the next of the joined block to its next block 
    // in the free list's next block.
    block->next = get_next(get_next(block));
    return block;
}



/*
 * uinit - Used initialize metadata required to manage the heap
 * along with allocating initial memory.
 */
int uinit() {
    // Creates a header block that is never allocated to the user 
    // has a size of 0. Is the start of the free list.
    memory_block_t* free_head_block = (memory_block_t*) csbrk(PAGESIZE);
    free_head = free_head_block;
    put_block(free_head_block, 0, false);
    // Creates a block for the rest of the heap given from 
    // csbrk that can be allocated.
    memory_block_t* usable_memory = free_head_block + 1;
    put_block(usable_memory, PAGESIZE - 32, false);
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
    // Find a block within the free list that will satisfy the 
    // umalloc request or extend the heap.
    memory_block_t* curr = find(payload_size);
    // return the ptr that is just the payload (doesn't include the header).
    return get_payload(curr); 
}

/*
    ufree_check_coalescing - checks if the target block is right next 
    to its previous block within the free 
    list and/or its next block within the free list in memory. 
    If they are adjacent to one another than coalesce blocks together.
*/
void ufree_check_coalescing(memory_block_t* prev, memory_block_t* target) {
    memory_block_t* next = get_next(target);
    memory_block_t* prev_end_address = (memory_block_t*) ((long) prev + 16 + get_size(prev));
   // Check if target block is the first free block after the free 
   // head so that you do not coalesce the target block with the free 
   // head since it is never allocated.
    if (prev != free_head) {
         // If the previous free block and this block are right next 
         // to each other in memory coalesce them.
        if (prev_end_address == target) {
            target = coalesce(prev);
        }
    }
    // Check if target block is the last free block in the free list.
    if (next != NULL) {
        memory_block_t* tar_end_address =  (memory_block_t*) ((long) target + 16 + get_size(target));
        // If this block and the next free block in the free list are 
        // right next to each other in memory then coalesce them.
        if (tar_end_address == next) {
            coalesce(target);
        }
    }
}


/*
 * ufree -  frees the memory space pointed to by ptr, which must have been called
 * by a previous call to malloc.
 * Inserts the newly freed block in an address ordered free list. Coaeleces
 * immediately if applicable.
 */
void ufree(void *ptr) {
    // Get the block address that is associated with the ptr to its payload.
    memory_block_t* target = get_block(ptr);
    deallocate(target);
    memory_block_t* curr = get_next(free_head);
    memory_block_t* prev = free_head;
    // Search through free list to add target in front of the first
    // free block found with a greater address
    while (curr != NULL) {
        // Check if address of curr is greater than the addres of target.
        if (curr > target) {
            target->next = curr;
            prev->next = target;
            // Coalesce blocks in free list if neccessary.
            ufree_check_coalescing(prev, target);
            return;
        }
        prev = curr;
        curr = get_next(curr);
        // Reached end of free list, add target to end of list.
        if (curr == NULL) {
            prev->next = target;
            target->next = NULL;
            ufree_check_coalescing(prev, target);
        }
    }
}