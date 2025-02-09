# Lab notes

```c++
kvmmap(pagetable_t kpgtbl, uint64 va, uint64 pa, uint64 sz, int perm)

// va, size and pa must be alligned
int mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, int perm);
```

How can `uvmcopy` in `kernel/vm.c` knows the page to be copied is superpage or not?

TODO:
Change `void uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, int do_free)` to handle the unmap superpage.


0010000111111000000000 0000010111
0010000111111000000001 0000010111