```c
/**
* list_add_before - add a new entry
* @listelm: list head to add list_add_before
* @elm: new entry to be added
*
* Insert the new element &elm *before* the element @listelm which is already in the list.
*/

static inline void 
list_add_before(list_entry_t *listelm, list_entry_t *elm){
    __list_add(elm, listelm->prev, listelm);
}

/**
* Insert a new entry between two known consecutive entries.
* 
* This is only for internal list manipulation where we know the prev/next entries already!
*/

static inline void 
    list_add(list_entry_t * elm, list_entry_t *prev, list_entry *next){
        prev->next = next->prev = elm;
        elm->next = next;
        elm->prev = prev;
    }

/**
* list_prev -get the previous entry
* @listelm: the list head
*/

static inline list_entry_t * list_prev(list_entry_t * listelm){
    return listelm->prev;
}
