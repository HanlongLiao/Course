```c
/**
* list_add -add a new free_list
* @listelm: list head to add after
* @elm: new entry to be added
* Instert the new element @elm *after* the element @listelm which
* Is already in the list
*/
static inline void 
list_add(list_entry_t * listelm, list_entry_t *elm){
    list_add_after(listelm, elm);
}

// ...
static inline void
set_page_ref(struct Page *page, int val){
    page->ref = val;
}