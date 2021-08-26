# Foreword

The difficulty of this lab lies in the various details. In the remaining chapters, I will step through the key points to consider in this experiment :

1. page table tracking
2. copy timing
3. copy operation
4. other details

# Page Table Tracking

Similar to lazy lab, the goal of copy-on-write (COW) fork() is to defer allocating and copying physical memory pages for the child until the copies are actually needed, if ever.

For the first question, we need to track some information about the page table, they may include:
1. virtual address
2. physical address
3. PTE flag
4. ...

## Physical address usage tracking

In the previous lessons, we have recognized that page tables can be used to create mappings from virtual to physical addresses, which may be one-to-one, many-to-one (in the same address space), or many-to-one. (in different address spaces)

For COW fork, there may be multiple processes referencing the same physical address. So one idea is to record the number of references to a piece of physical memory.

In the previous lessons, we learned that physical memory can be divided into blocks of page tables of size `PGSIZE`.

```cpp
uint64 memory_page = physical_address / PGSIZE;
```

We need to record the usage of `memory_page`. There are many kinds of data structures to choose, such as list, arrays, etc.

For simplicity, an array is used here to store all usage:

```cpp
#define NUM_MEMORY_PAGE MAXVA / PGSIZE
int mem_ref_cnt[NUM_MEMORY_PAGE] = { 0 };
```

At the beginning, `mem_ref_cnt` is `0`, when the corresponding page is allocated, the value of the reference count should be `1`; when the page table is shared, the value of the reference count will increase; when the page table is released, the value of the reference count will decrease; when the value of the reference count is `0`, it means that the page table is no longer used by any object, and this is the time to free up this memory space.

The following functions correspond to the individual timings as described above:
- `kalloc`: set `mem_ref_cnt` to 1
- `uvmcopy`: invoked in `fork`. Need to increase the `mem_re_cnt` of the parent process page table.
- `kfree`: decrease the `mem_re_cnt` and the original function will be called when the count value is 0.

## PTE flag setting

The first flag we want to focus on is `PTE_W`, which indicates the corresponding write permission for the page table. For ordinary page table items, we don't care about their `PTE_W` flags. In the COW solution, multiple processes can share the same physical address, which is only used for reading, thus postponing the timing of fork allocation of space. **Therefore, for all COW page tables, the `PTE_W` flag bit needs to be cleared.**

Here we mention the so-called COW page tables, how can we distinguish them from ordinary page tables? Perhaps we could take a similar approach to reference counting and track with new variables, but RISC-V provides a convenient mechanism which allows some data to be set in flags for kernel use only:

```cpp
// PTE[9:8] is reserved for supervisor software
#define PTE_RSW_BIT8 (1L << 8)
#define PTE_RSW_BIT9 (1L << 9)
```

We can set the RSW bit, and thus distinguish between ordinary page tables and COW page tables.


# Copy Timing

Similar to lazy lab, COW fork defers the allocation time of the children process memory space. After the fork is executed, all processes will share the same physical address space. When this memory space is not modified, then everything is fine, which means that the process can only perform read operations on this space. If a write operation is taken, it indicates that the corresponding process can no longer share the address space, but needs a separate address space, which is the time for copying.

Similar to lazy lab, the **write** operation may occur in the following two situations:

1. User Space: when the user tries to perform a write operation, which will trigger a page fault.
2. Kernel Space: for `copyout`, copy from kernel to user

In both cases, you can take the same operation to copy, which is what we will mention in the next section.


# Copy Operation

Although the timing of the copy-on-write may be numerous, the process of copy-on-write is largely consistent. We need to make some modifications to `uvmcopy` and use special uvmcopy when we really need to perform a copy operation.

For the original uvmcopy, it performs the following operations:

1. Find each PTE of the parent process
2. Use `kalloc` to allocate the physical address of the child process
3. Copies memory data from the parent process to the child process
4. Create a mapping from the virtual to the physical address of the child process and set the appropriate flags

For the modified version of uvmcopy, you need to pay attention to the following points:

1. The operations in steps 2 and 3 are deferred
2. Modify the page table flags of the parent and child processes, remove their `PTE_W` flags, and mark them as COW pages

For postponed uvmcopy, the following operations need to be performed:

1. Find the PTE corresponding to the virtual address
2. Allocate a free block of memory using `kalloc`
3. Copy old data to new mem block
4. If possible, free the old page table.
5. Establish a virtual address to a physical address mapping, the current page is no longer a COW page, and has the write permission.

# Other Details...

## lock

Thread safety of reference counting needs to be considered. One way to do this is to use locks.

In my solution, the locks in the `kmem` structure are reused, and you can also create a new lock directly, or use lock-free programming techniques to accomplish this.

## trap

When a user tries to write to memory in a COW page, a trap will be triggered because there is no write permission. The corresponding Exception Code can be found in the RISC-V manual, here the Store/AMO page fault corresponds to the Exception code `15`.