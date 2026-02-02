#include "wutils.h"
#include <string.h>

static struct WMemState state = {0};

static void* WMemGetReferences() {
    return state.start;
}

static WMemRef* WMemGetRefArray() {
    return (WMemRef*)state.start;
}

static void* WMemGetPoolStart() {
    if (state.start == NULL) return NULL;
    return (char*)state.start + (state.references_capacity * sizeof(WMemRef));
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

void list_insert_at(WList* target_list, ShapeData* item_to_add, size_t in_target_pos){

    size_t target_pos = (in_target_pos >= target_list->list_size)
    ? target_list->list_size : in_target_pos;

    // Look for an available index in the inner array
    size_t found_av = NULL_OFFSET;
    for (size_t i = 0; i < target_list->items.size; i++) {
        ShapeDataInList current = arr_get(ShapeDataInList,target_list->items,i);
        if (current.next == NULL_OFFSET
            && current.prev == NULL_OFFSET
            && i != target_list->starting_index) {
            found_av = i;
            break;
        }
    }
    // find the item of the list that will be replaced and its previous
    ShapeDataInList* current = NULL;
    size_t new_next = NULL_OFFSET;
    size_t new_prev = NULL_OFFSET;
    size_t current_index = target_list->starting_index;
    if (current_index != NULL_OFFSET) {
        current = &arr_get(ShapeDataInList,target_list->items,current_index);
        for (size_t i = 0; i < target_pos; i++) {
            current_index = current->next;
            if (current_index != NULL_OFFSET) {
                current = &arr_get(ShapeDataInList,target_list->items,current_index);
            }
        }
        new_next = current_index;
        if (current_index != NULL_OFFSET) {
            if (current->prev != NULL_OFFSET) {
                new_prev = current->prev;
            }
        }
    }

    ShapeDataInList new_cube = {0};
    new_cube.item = *item_to_add;
    new_cube.prev = new_prev;
    new_cube.next = new_next;
    if (found_av != NULL_OFFSET) {
        arr_set(target_list->items, found_av, new_cube);
    } else {
        arr_append(target_list->items, new_cube);
        found_av = target_list->items.size - 1;
    }
    if (new_prev != NULL_OFFSET) arr_get(ShapeDataInList,target_list->items,new_prev).next = found_av;
    if (new_next != NULL_OFFSET) arr_get(ShapeDataInList,target_list->items,new_next).prev = found_av;
    if (new_prev == NULL_OFFSET) target_list->starting_index = found_av;
    target_list->list_size++;
}