mm_struct结构体定义
```C
   struct mm_struct {  
        // linear list link which sorted by start addr of vma  
        list_entry_t mmap_list;  
        // current accessed vma, used for speed purpose  
        struct vma_struct *mmap_cache;  
        pde_t *pgdir; // the PDT of these vma  
        int map_count; // the count of these vma  
        void *sm_priv; // the private data for swap manager  
    };  
```