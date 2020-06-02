```C
//get_pte - get pte and return the kernel virtual address of this pte for la
//        - if the PT contians this pte didn't exist, alloc a page for PT
// parameter:
//  pgdir:  the kernel virtual base address of PDT
//  la:     the linear address need to map
//  create: a logical value to decide if alloc a page for PT
// return vaule: the kernel virtual address of this pte
pte_t *
get_pte(pde_t *pgdir, uintptr_t la, bool create) {
    /* LAB2 EXERCISE 2: YOUR CODE
     *
     * If you need to visit a physical address, please use KADDR()
     * please read pmm.h for useful macros
     *
     * Maybe you want help comment, BELOW comments can help you finish the code
     *
     * Some Useful MACROs and DEFINEs, you can use them in below implementation.
     * MACROs or Functions:
     *   PDX(la) = the index of page directory entry of VIRTUAL ADDRESS la.
     *   KADDR(pa) : takes a physical address and returns the corresponding kernel virtual address.
     *   set_page_ref(page,1) : means the page be referenced by one time
     *   page2pa(page): get the physical address of memory which this (struct Page *) page  manages
     *   struct Page * alloc_page() : allocation a page
     *   memset(void *s, char c, size_t n) : sets the first n bytes of the memory area pointed by s
     *                                       to the specified value c.
     * DEFINEs:
     *   PTE_P           0x001                   // page table/directory entry flags bit : Present
     *   PTE_W           0x002                   // page table/directory entry flags bit : Writeable
     *   PTE_U           0x004                   // page table/directory entry flags bit : User can access
     */
    //typedef uintptr_t pde_t;
    pde_t *pdep = &pgdir[PDX(la)];  // (1)获取页表
    if (!(*pdep & PTE_P))             // (2)假设页目录项不存在
    {      
        struct Page *page;
        if (!create || (page = alloc_page()) == NULL) // (3) check if creating is needed, then alloc page for page table
        {    //假如不需要分配或是分配失败
            return NULL;
        }
        set_page_ref(page, 1);                      // (4)设置被引用1次
        uintptr_t pa = page2pa(page);                  // (5)得到该页物理地址
        memset(KADDR(pa), 0, PGSIZE);                  // (6)物理地址转虚拟地址，并初始化
        *pdep = pa | PTE_U | PTE_W | PTE_P;            // (7)设置可读，可写，存在位
    }
    return &((pte_t *)KADDR(PDE_ADDR(*pdep)))[PTX(la)];     // (8) return page table entry
    //KADDR(PDE_ADDR(*pdep)):这部分是由页目录项地址得到关联的页表物理地址，再转成虚拟地址
    //PTX(la)：返回虚拟地址la的页表项索引
    //最后返回的是虚拟地址la对应的页表项入口地址
}
```