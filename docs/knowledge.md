# Knowledge

## System Call Routine

In operating system, the system call is one type of **trap**, which means the normal routine in the user space is interruped, and then switching to kernel space.

The system call has a specific routine depending on the hardware platform. In XV6, the routine is

1. Invoke the syscall from its user space stub. i.e. the system calls interface in `xv6-labs-20xx/user/user.h`. The assembly code for these user space stub is defined in `xv6-labs-20xx/user/usys.pl` (using Perl), and the real assembly code is generated after building, in `xv6-labs-20xx/user/usys.S`.

    ```s
    # user.usys.S
    li a7, <syscall_number>
    ecall
    ret
    ```

   `ecall` instruction is used to **make a request for a service from the execution environment**. It triggers a trap to the operating system.

2. Execute `uservec` in Trampoline (`kernel/trampoline.S`)

   This happens in **supervisor mode**. The steps are:
   * The register sets of the user space are saved into **trapframe** page.
     > See `struct trapframe` in `kernel/proc.h`.
   * The kernel stackpointer is also initialized.
   * Install the kernal pagetable by `csrw satp, t1`.
   * Jump to `usertrap` function. [C1].

3. Execute `void usertrap(void)` function.

   The following are done:
   * Set the trap handler to the kernel trap handler by `w_stvec((uint64)kernelvec);`.
   * ...
   * Determine the cause of why `usertrap` is invoked by `if(r_scause() == 8)`. `8` is the code for system call.
   * ...
   * Go into `void syscall(void)`: ...
   * Call the last line `usertrapret();`

4. Execute `void usertrapret(void)`.

   * The restoration of user saved registers.
   * ...
   * Get the kernel vaddr of the user pagetable of this process.
   * Call `userret` (defined in `trampoline.S`) by `((void (*)(uint64))trampoline_userret)(satp);`
     > NOTE: The trampoline's user va and kernel va is same!

5. Execute `userret`.

   * Switch to user pagetable
   * Restore all registers from the user trapframe.
   * Call `sret` instruction to switch back to user mode!!!

<!-- 
```sh
break usertrapret
break userinit
break allocproc
break forkret
break kernel/trap.c:101
``` 
-->

## Startup Routine

TBD

## Pagetable

### Pagetable Data Structure

xv6 uses three-level (L2, L1 and L0) pagetable. The entire pagetable is a sparse-tree structure.

* L2 is the root pagetable (has 512 entries). Each entry of L2 has 512 L1 pagetables. And Each entry of L0 has 512 L0 pages.
* The entire pagetable is a sparse-tree structure, which saves space for the unused virtual pages mapping.
* Each physical page is 4096 bytes.
* One physical page is the smallest unit for address mapping.
* Each single-level pagetable itself takes one physical page.
* PPN stores the physical page number (representing a physical address) of the next level page or leaf.

The virtual address shall be interrpeted as indices of 3-dim array. E.g.
`Pagetable[L2 index][L1 index][L0 index]`

```text
# Virtual Address Meaning

| Reserved 25 bits | L2 index (9 bits) | L1 index (9bits) | L0 index (9 bits) | offset (12 bits) |
| ---------------- | ----------------- | -----------------| ----------------- | ---------------- |

# Single-Level Pagetable
 ----------------------------------------------------------------------------------
| Index | Reserved 10 bits | PPN (Physical Page Number, 44 bits) | Flags (10 bits) |
| ----- | ---------------- | ----------------------------------- | --------------- |
|  511  |                  | Addr to sub-level pagetable or leaf |                 |
|   …   |                  | Addr to sub-level pagetable or leaf |                 |
|   1   |                  | Addr to sub-level pagetable or leaf |                 |
|   0   |                  | Addr to sub-level pagetable or leaf |                 |
 ----------------------------------------------------------------------------------

# Flags
 ----------------------------------------------------------------------------------
| Reserved 5 bits |          Bit 4          | Bit 3 | Bit 2 | Bit 1 |     Bit 0    |
|---------------- | ----------------------- | ----- | ----- | ----- | ------------ |
|                 | PTE_U (user can access) | PTE_X | PTE_W | PTE_R | PTE_V (valid)|
 ----------------------------------------------------------------------------------

```

Some formulas:

```c++
physical_addr = (PPN >> 10) << 12;

```

### Pagetable Allocation

General principles are

* **Allocation on demand**, i.e. the unused virtual address will not allocate pagetables for it. The sparsity is the key!!!
* `kalloc()` selects an empty physical page from the linked list starting at `kmem.freelist`, and `void kfree(void *pa)` frees the physical page and update `kmem.freelist with the page just been released.
* The not allocated physical page first 8 bytes stores a pointer to the next not-allocated physical page.
* The release order of physical pages will impact the topology of the linked list.

The kernel virtual address space is created by `kernel/main.c`:

* The sections of kernel are loaded to physical address from `0x80000000` to `end`(the first address after kernel) according to `kernel.ld`. Not pagetables are enabled at the begining.
* The unused physical memory from `end` until `PHYSTOP` are linked with linkist by `freerange(end, (void*)PHYSTOP)`. `kmem.freelist` is assigned.
* Creating kernel pagetable with mapping by `kernel_pagetable = kvmmake()`, invoked by `kvminit()` in main.
  > Note the trampoline page is mapped to the highest va. The physical address from `0x80000000` to `end` has identical mapping.
* Load the kernel pagetable and enable paging by `w_satp(MAKE_SATP(kernel_pagetable))`. From now on, kernel virtual addr is used.

The user process virtual address space is created by `pagetable_t uvmcreate()` in `pagetable_t proc_pagetable(struct proc *p)`.

## Function Explain

```cpp
// kernel/proc.c
// Only called by `void userinit(void)` and `int fork(void)`.
static struct proc* allocproc(void);
```

## Comments

### C1: How is `usertrap` cached in proc?

Note `p->trapframe->kernel_trap = (uint64)usertrap;` in function `void usertrapret(void);`
in `kernel/trap.c:107`.

This line is called for the first time at the OS startup (`kernel/main.c`). The calling footprint is:

```txt
main() at kernel/main.c:31

userinit() at kernel/proc.c:234

allocproc() at kernel/proc.c:111
    p->context.ra = (uint64)forkret;

forkret() at kernel/proc.c:527

usertrapret() at kernel/trap.c:91, called by `scheduler()` in kernel main
    p->trapframe->kernel_trap = (uint64)usertrap; // kernel/trap.c:107
```
