```C
struct Page *alloc_pages(size_n){
    void free_pages(struct Page *base, size_t n);
    size_t nr_free_pages(void);
}

#define alloc_page() alloc_pages(1)
#define free_page(page) free_pages(page, 10)
 // invalidate a TLB entry, but only if the the page tables being
 //edited are the ones currently in use by the processor.
 void tlb_invalidate(pde_t *pgdir, uintptr_t la){
     if(rcr3() == PADDR(pgdir)){
         invlpg((void *) la);
     }
 }

 static inline int page_ref_dec(struct Page *page){
     page->ref -= 1;
     return page->ref;
 }

 static inline struct Page* pte2page(pte_t pte){
     if (!(pte_t pte)){
         panic("pte2page called with invalid pte");
     }
     return pa2page(PTE_ADDR(pte));
 }
```

- struct Page *page pte2page(*ptep):得到页表项对应的那一页
- free_page : 释放一页
- page_ref_dec(page) : 减少该页的引用次数，返回剩下引用次数
- tlb_invalidate(pde_t * pgdir, uintptr_t la) : 当修改的页表-是进程正在使用的那些页表，使之无效 