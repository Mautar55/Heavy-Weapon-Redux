#include "wutils.h"
#include <string.h>

static struct WMemState state = {0};

static void* WMemGetReferences() {
    return state.start;
}

static WMemRef* WMemGetRefArray() {
    return (WMemRef*)state.start;
}

static void* WMemGetPoolStart() {return (char*)state.start + (state.references_capacity * sizeof(WMemRef));
}

void* WMemGetStart() {
    return state.start;
}

wref WMemAlloc(size_t size) {
    if (state.references_capacity == 0) {
        state.references_capacity = 256;
        state.references_capacity_jump = 256;
        state.pool_capacity = 1024 * 1024; // 1MB initial pool
        state.pool_capacity_jump = 1024 * 1024;
        
        size_t total_size = (state.references_capacity * sizeof(WMemRef)) + state.pool_capacity;
        state.start = MemAlloc(total_size);
        memset(state.start, 0, total_size);
    }

    // Check if we need more references
    // Search for a free slot (ptr == NULL)
    WMemRef* refs = WMemGetRefArray();
    int found_slot = -1;
    for (size_t i = 0; i < state.references_capacity; i++) {
        if (refs[i].ptr == NULL) {
            found_slot = (int)i;
            break;
        }
    }

    if (found_slot == -1) {
        size_t old_ref_cap = state.references_capacity;
        state.references_capacity += state.references_capacity_jump;
        
        size_t total_size = (state.references_capacity * sizeof(WMemRef)) + state.pool_capacity;
        
        void* new_start = MemAlloc(total_size);
        memset(new_start, 0, total_size);
        
        // Copy old references
        memcpy(new_start, state.start, old_ref_cap * sizeof(WMemRef));
        
        // Copy pool data
        void* old_pool = (char*)state.start + (old_ref_cap * sizeof(WMemRef));
        void* new_pool = (char*)new_start + (state.references_capacity * sizeof(WMemRef));
        memcpy(new_pool, old_pool, state.pool_size);
        
        // Update pointers in references because the pool moved relative to state.start
        WMemRef* new_refs = (WMemRef*)new_start;
        for (size_t i = 0; i < old_ref_cap; i++) {
            if (new_refs[i].ptr != NULL) {
                size_t offset = (char*)new_refs[i].ptr - (char*)old_pool;
                new_refs[i].ptr = (char*)new_pool + offset;
            }
        }
        
        MemFree(state.start);
        state.start = new_start;
        
        // Use the first newly created slot
        found_slot = (int)old_ref_cap;
    }

    // Check if we need more pool space
    if (state.pool_size + size > state.pool_capacity) {
        void* old_pool = WMemGetPoolStart();
        void* old_start = state.start;

        while (state.pool_size + size > state.pool_capacity) {
            state.pool_capacity += state.pool_capacity_jump;
        }

        size_t total_size = (state.references_capacity * sizeof(WMemRef)) + state.pool_capacity;
        state.start = MemRealloc(state.start, total_size);

        if (state.start != old_start) {
            void* new_pool = WMemGetPoolStart();
            WMemRef* refs_after_realloc = WMemGetRefArray();
            for (size_t i = 0; i < state.references_capacity; i++) {
                if (refs_after_realloc[i].ptr != NULL) {
                    size_t offset = (char*)refs_after_realloc[i].ptr - (char*)old_pool;
                    refs_after_realloc[i].ptr = (char*)new_pool + offset;
                }
            }
        }
    }

    // Use the found slot
    WMemRef* current_refs = WMemGetRefArray();
    void* pool_ptr = (char*)WMemGetPoolStart() + state.pool_size;
    current_refs[found_slot].ptr = pool_ptr;
    current_refs[found_slot].size = size;
    
    state.pool_size += size;
    
    return (wref){found_slot * sizeof(WMemRef)};
}

wref WMemRealloc(wref ref, size_t size) {
    if (ref IS_NULLWREF) return WMemAlloc(size);
    if (size == 0) {
        WMemFree(ref);
        return NULLWREF;
    }

    WMemRef* refs = WMemGetRefArray();
    size_t index = ref.offset / sizeof(WMemRef);
    
    if (index < state.references_capacity && refs[index].ptr != NULL) {
        void* old_ptr = refs[index].ptr;
        size_t old_size = refs[index].size;

        wref new_ref = WMemAlloc(size);
        WMemRef* new_refs = WMemGetRefArray();
        size_t new_index = new_ref.offset / sizeof(WMemRef);
        void* new_ptr = new_refs[new_index].ptr;

        size_t copy_size = (size < old_size) ? size : old_size;
        memcpy(new_ptr, old_ptr, copy_size);
        
        // Mark old slot as free
        WMemFree(ref);
        return new_ref;
    }
    
    return NULLWREF;
}

void WMemFree(wref ref) {
    if (ref IS_NULLWREF) return;
    WMemRef* refs = WMemGetRefArray();
    size_t index = ref.offset / sizeof(WMemRef);
    
    if (index < state.references_capacity) {
        // Mark as free by setting ptr to NULL
        refs[index].ptr = NULL;
        refs[index].size = 0;
    }
}

void WMemClear() {
    MemFree(WMemGetRefArray());
}

///////////////////////////////////////////////////////
/// LIST IMPLEMENTATION
///////////////////////////////////////////////////////

void list_insert_at(WList* target_list, void* item_to_add, size_t in_target_pos){

    size_t full_item_size = target_list->item_size + sizeof(size_t) * 2; // item + prev + next

    // Helper to get pointer to element in items array
    #define LIST_ITEM_PTR(list, index) ((char*)WMemRefFromOffset((list)->items.items_ref)->ptr + (index) * full_item_size)
    #define GET_PREV(ptr) (*(size_t*)((char*)(ptr) + target_list->item_size))
    #define GET_NEXT(ptr) (*(size_t*)((char*)(ptr) + target_list->item_size + sizeof(size_t)))

    // Look for an available index in the inner array
    size_t found_av = NULL_OFFSET;
    for (size_t i = 0; i < target_list->items.size; i++) {
        void* current_ptr = LIST_ITEM_PTR(target_list, i);
        if (GET_NEXT(current_ptr) == NULL_OFFSET
            && GET_PREV(current_ptr) == NULL_OFFSET
            && i != target_list->starting_index) {
            found_av = i;
            break;
            }
    }

    // find the item of the list that will be replaced and its previous
    size_t new_next = NULL_OFFSET;
    size_t new_prev = NULL_OFFSET;

    if (in_target_pos < target_list->list_size && target_list->starting_index != NULL_OFFSET) {
        size_t current_index = target_list->starting_index;
        if (current_index != NULL_OFFSET) {
            void* current_ptr = LIST_ITEM_PTR(target_list, current_index);
            for (size_t i = 0; i < in_target_pos; i++) {
                current_index = GET_NEXT(current_ptr);
                if (current_index != NULL_OFFSET) {
                    current_ptr = LIST_ITEM_PTR(target_list, current_index);
                }
            }
            new_next = current_index;
            new_prev = GET_PREV(current_ptr);
        }
    } else if (in_target_pos == target_list->list_size && target_list->starting_index != NULL_OFFSET) {
        new_prev = target_list->last_index;
    }

    if (found_av == NULL_OFFSET) {
        // Grow array if needed
        if (target_list->items.size >= target_list->items.capacity) {
            if (target_list->items.capacity == 0) target_list->items.capacity = 256;
            else target_list->items.capacity *= 2;
            target_list->items.items_ref = WMemRealloc(target_list->items.items_ref, target_list->items.capacity * full_item_size);
        }
        found_av = target_list->items.size++;
    }

    void* target_ptr = LIST_ITEM_PTR(target_list, found_av);
    memcpy(target_ptr, item_to_add, target_list->item_size);
    GET_PREV(target_ptr) = new_prev;
    GET_NEXT(target_ptr) = new_next;

    if (new_prev != NULL_OFFSET) {
        GET_NEXT(LIST_ITEM_PTR(target_list, new_prev)) = found_av;
    } else {
        target_list->starting_index = found_av;
    }

    if (new_next != NULL_OFFSET) {
        GET_PREV(LIST_ITEM_PTR(target_list, new_next)) = found_av;
    } else {
        target_list->last_index = found_av;
    }

    target_list->list_size++;

    #undef LIST_ITEM_PTR
    #undef GET_PREV
    #undef GET_NEXT
}

void list_remove_at(WList* target_list, size_t in_target_pos)
{
    if (!target_list) return;
    if (target_list->list_size == 0) return;
    if (in_target_pos >= target_list->list_size) return;
    if (target_list->starting_index == NULL_OFFSET) return;

    size_t full_item_size = target_list->item_size + sizeof(size_t) * 2; // item + prev + next

    #define LIST_ITEM_PTR(list, index) ((char*)WMemRefFromOffset((list)->items.items_ref)->ptr + (index) * full_item_size)
    #define GET_PREV(ptr) (*(size_t*)((char*)(ptr) + target_list->item_size))
    #define GET_NEXT(ptr) (*(size_t*)((char*)(ptr) + target_list->item_size + sizeof(size_t)))

    // Walk to the node at logical position in_target_pos
    size_t current_index = target_list->starting_index;
    void* current_ptr = LIST_ITEM_PTR(target_list, current_index);

    for (size_t i = 0; i < in_target_pos; i++) {
        size_t next_index = GET_NEXT(current_ptr);
        if (next_index == NULL_OFFSET) {
            // List structure is inconsistent with list_size; bail safely.
            return;
        }
        current_index = next_index;
        current_ptr = LIST_ITEM_PTR(target_list, current_index);
    }

    size_t prev_index = GET_PREV(current_ptr);
    size_t next_index = GET_NEXT(current_ptr);

    // Bridge neighbors around current
    if (prev_index != NULL_OFFSET) {
        void* prev_ptr = LIST_ITEM_PTR(target_list, prev_index);
        GET_NEXT(prev_ptr) = next_index;
    } else {
        // Removing head
        target_list->starting_index = next_index;
    }

    if (next_index != NULL_OFFSET) {
        void* next_ptr = LIST_ITEM_PTR(target_list, next_index);
        GET_PREV(next_ptr) = prev_index;
    } else {
        // Removing tail
        target_list->last_index = prev_index;
    }

    // Mark this slot as available (matches insert_at free-slot check)
    GET_PREV(current_ptr) = NULL_OFFSET;
    GET_NEXT(current_ptr) = NULL_OFFSET;

    // Optional: wipe item payload (helps debugging; not required)
    // memset(current_ptr, 0, target_list->item_size);

    target_list->list_size--;

    #undef LIST_ITEM_PTR
    #undef GET_PREV
    #undef GET_NEXT
}

void list_insert_first(WList* target_list, void* item_to_add) {
    list_insert_at(target_list, item_to_add, 0);
}

void list_insert_last(WList *target_list, void *item_to_add) {
    list_insert_at(target_list, item_to_add, target_list->list_size);
}

void list_remove_first(WList* target_list) {
    list_remove_at(target_list, 0);
};

void list_remove_last(WList* target_list) {
    list_remove_at(target_list, target_list->list_size - 1);
};
