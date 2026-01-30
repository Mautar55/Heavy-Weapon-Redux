#include <stddef.h>
#include <stdlib.h>
#include "raylib.h"

typedef struct {
    Vector3 position;
} CircleData;

#define da_struct(T)\
    typedef struct {\
        size_t items_ref;\
        size_t size;\
        size_t capacity;\
    } T##Array;

// returns what used to be // T* items

#define da_append(array, new_item)\
    do {\
        if ((array).size >= (array).capacity) {\
            if ((array).capacity == 0) (array).capacity = 256;\
            else (array).capacity *= 2;\
            (array).items_ref = WMemRealloc((array).items_ref, (array).capacity * sizeof(new_item));\
        }\
        (WMemRef)(WMemGetStart[(array).items_ref]).ptr[(array).size++] = new_item;\
    } while(0)

da_struct(CircleData)

// Heap memory management

struct WMemState {
    void * start;
    size_t references_capacity;
    size_t references_capacity_jump;
    size_t pool_size;
    size_t pool_capacity;
    size_t pool_capacity_jump;
};

typedef struct {
    void * ptr;
    size_t size;
}WMemRef;

// The array starts with a smaller array of references.
// That array should be used to track allocated memory blocks
// The allocated memory blocks vary in size themselves.
// Whenever new memory is requested, the positions of the references should not change,
// But the pointers to allocated memory should not.

#define WMEM_INVALID_OFFSET ((size_t)-1)

size_t WMemAlloc(size_t size);
size_t WMemRealloc(size_t offset, size_t size);
void WMemFree(size_t offset);

// To access the reference directly:
// struct WMemRef* ref = (struct WMemRef*)((char*)state.start + offset);
// But state is static in wutils.c. Let's add a helper to get the start pointer.
void* WMemGetStart(void);
