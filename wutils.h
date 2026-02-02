#include <stddef.h>
#include <stdlib.h>
#include "raylib.h"

#define NULL_OFFSET ((size_t)-1)
#define NULLWREF (wref){NULL_OFFSET}
#define IS_NULLWREF .offset==NULL_OFFSET
typedef struct {
    size_t offset;
}wref;

typedef struct {
    Vector3 position;
} ShapeData;

typedef struct {
    wref items_ref;
    size_t size;
    size_t capacity;
} WArray;

#define arr_new(label)\
    WArray label = {0};\
    label.items_ref = NULLWREF;

// returns what used to be // T* items

#define arr_append(array, new_item)\
    do {\
        if ((array).size >= (array).capacity) {\
            if ((array).capacity == 0) (array).capacity = 256;\
            else (array).capacity *= 2;\
            (array).items_ref = WMemRealloc((array).items_ref, (array).capacity * sizeof(new_item));\
        }\
        WMemRef *ref = WMemRefFromOffset(array.items_ref);\
        size_t dest_i = array.size++;\
        size_t elem_size = sizeof new_item;\
        memcpy((char*)ref->ptr + dest_i * elem_size, &new_item, elem_size);\
    } while(0)

#define arr_get(type_cast, array, index)\
    ((type_cast*)WMemRefFromOffset(array.items_ref)->ptr)[index]\

#define arr_set(array, index, new_item)\
    do {\
        size_t _i = (size_t)(index);\
        size_t _elem_size = sizeof(new_item);\
        if (_i >= (array).capacity) {\
            if ((array).capacity == 0) (array).capacity = 256;\
            while (_i >= (array).capacity) (array).capacity *= 2;\
            (array).items_ref = WMemRealloc((array).items_ref, (array).capacity * _elem_size);\
        }\
        WMemRef *_ref = WMemRefFromOffset((array).items_ref);\
        memcpy((char*)_ref->ptr + _i * _elem_size, &(new_item), _elem_size);\
        if (_i >= (array).size) (array).size = _i + 1;\
    } while (0)

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

wref WMemAlloc(size_t size);
wref WMemRealloc(wref ref, size_t size);
void WMemFree(wref ref);
void WMemClear();

// To access the reference directly:
// struct WMemRef* ref = (struct WMemRef*)((char*)state.start + offset);
// But state is static in wutils.c. Let's add a helper to get the start pointer.
void* WMemGetStart(void);

static inline WMemRef* WMemRefFromOffset(wref ref)
{
    return (WMemRef*)((char*)WMemGetStart() + ref.offset);
}

/////////////////////////////////////////////////
/// LINKED LIST
/////////////////////////////////////////////////

typedef struct {
    size_t list_size;
    size_t starting_index;
    WArray items;
} WList;

#define declare_list_item(type)\
    typedef struct {\
        type item;\
        size_t prev;\
        size_t next;\
    } type##InList;

declare_list_item(ShapeData)

// implementar en main.c
// y despues pasar a macro
#define list_new(label)\
    WList list = {0};\
    arr_new(new_list);\
    list.items = new_list;\
    list.item_size = sizeof(list_item(typeof(type)));\

void list_insert_at(WList* target_list, ShapeData* item_to_add, size_t target_pos);

// insert at position
// delete at position