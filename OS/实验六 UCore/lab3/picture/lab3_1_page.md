```C
pte_t *get_pte(pde_t *pgdir, uinitptr_t la, bool create);
struct Page *pgdir_alloc_page(pge_t *pgdir, uintptr_ la, uint32_t perm);

// convert list entry to page
#define le2page(le, member)
    to_struct((le), struct Page, member);
```
