```c
/**
* list_next -get the next entry
* @listelm: the list head
*/
static inline list_entry_t *
list_next(list_entry_t *listelm){
    return listelm-> next;
}

/**
* list_del - deletes entry from list
* @listelm: the element to delete from list
* Note: list_empty() on @listelm does not return true after this, the entry is in an undefined state.
*/
static inline void
list_del(list_entry_t *listelm){
    _list_del(listelm->prev, listelm->next);
}
```