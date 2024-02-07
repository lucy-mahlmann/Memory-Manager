# Memory Manager

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
    </li>
    <li><a href="#structure">Structure</a></li>
    <li><a href="#execution">Execution</a></li>
    <li><a href="#learning-challenges">Learning Challenges</a></li>
    <li><a href="#contact">Contact</a></li>
  </ol>
</details>


 
<!-- ABOUT THE PROJECT -->
## About The Project

Dynamic storage allocator for C programs that is space-effiecient and has high-throughput.

* Mallocs an allocated block for the given size bytes within the heap region
* Implements first fit allocation and splits allocated blocks to reduce internal fragmentation
* Frees allocated blocks within the heap and adds them back to the free list
* Coalesces contiguous free blocks in the free list to reduce external fragmentation

<!-- STRUCTURE -->
## Structure

The blocks in memory that are managed by the heap are represented as a `memory_block_struct`:

```
typedef struct memory_block_struct {
    size_t block_size_alloc; // size of block not including the header
    struct memory_block_struct *next; // points to next block in free list
} memory_block_t;
```
The blocks in memory that are unallocated are stored in a free list in address sorted order. 

Free list
* Singly linked list
* Holds all of the unallocated blocks that are within the heap
* Contains a free_head_block which is always the first element in the free list with a size of 0

Structure of `umalloc()` in `umalloc.c`

<img
  src="umalloc-block-diagram.jpg"
  style="display: inline-block; margin: 0 auto; width: 530px; height: 400px">

Structure of `ufree()` in `umalloc.c`

<img
  src="ufree-block-diagram.jpg"
  style="display: inline-block; margin: 0 auto; width: 458px; height: 466px">

<!-- EXECUTION -->
## Execution

The free list is initialized with `uinit()` in `umalloc.c` which sets up the necessary metadata in the free list and the intial size of the heap to `PAGESIZE` which is 4,096 bytes. Then subsequent calls to `umalloc()` and `ufree()` are made to allocate a block of memory in the heap and to deallocate a block in memory respectively. Calls to `ufree()` are only guaranteed to work if the pointer `ptr` passed in was returned by an earlier call to `umalloc()` and it has not yet been freed. 

Heap Consistency Check:
* Check that all pointers in the free list point to a valid free block (within the valid heap and are set to free)
* Check that no free memory blocks overlap one another
* Check that each memory block is 16-byte aligned


<!-- LEARNING CHALLENGES -->
## Learning Challenges

The major problems that I encountered while programming/designing include:
* what to do if the head of the free list is allocated since the program keeps track of the free list by storing a pointer to the first block
* how to determine when blocks that are in the free list should be coallesced
* what type of allocation strategy to choose that would be both simple and time efficient

How I solved these problems:
* including a free_head_block in the free list which has a size of 0 and can never be allocated (makes accessing free list simpler and only takes up 16 bytes for the block header)
* making the free list sorted by address, therefore, if blocks on the free list are contiguous in memory they would be adjacent in the free list
* chose first fit allocation since my free list is address ordered rather than ordered by size of blocks therefore is O(N)

<!-- CONTACT -->
## Contact

Lucy Mahlmann - lmahlmann@utexas.edu

Project Link: [https://github.com/lucy-mahlmann/Memory-Manager](https://github.com/lucy-mahlmann/Memory-Manager)


