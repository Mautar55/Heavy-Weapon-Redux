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
