#include <stddef.h>
#include <stdlib.h>
#include "raylib.h"


#define NULL_OFFSET ((size_t)-1)
#define NULLWREF (wref){NULL_OFFSET}
#define IS_NULLWREF .offset==NULL_OFFSET

#define random_position\
    {\
    (float)((GetRandomValue(0,1000)/1000.0f*8)-4),\
    (float)((GetRandomValue(0,1000)/1000.0f*1)-0),\
    (float)((GetRandomValue(0,1000)/1000.0f*8)-4)\
    }

#define random_floats\
    (float)((GetRandomValue(0,1000)/1000.0f*8)-4),(float)((GetRandomValue(0,1000)/1000.0f*1)-0),(float)((GetRandomValue(0,1000)/1000.0f*8)-4)

typedef struct {
    size_t offset;
}wref;

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
        WMemRef *ref = WMemRefFromOffset((array).items_ref);\
        size_t dest_i = (array).size++;\
        size_t elem_size = sizeof new_item;\
        memcpy((char*)ref->ptr + dest_i * elem_size, &new_item, elem_size);\
    } while(0)

#define arr_get(type_cast, array, index)\
    ((type_cast*)WMemRefFromOffset((array).items_ref)->ptr)[index]\

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

#define arr_clear(array)\
    (array).size = 0;\
    (array).capacity=0;\
    WMemClear(array.items_ref);

#define arr_append_str(array, append_str)\
do {\
const char *_src = (const char*)(append_str);\
if (_src == NULL) break;\
size_t _src_len = strlen(_src);\
size_t _dst_start = (array).size;\
if (_dst_start > 0) {\
WMemRef *_ref0 = WMemRefFromOffset((array).items_ref);\
char *_buf0 = (char*)_ref0->ptr;\
if (_buf0[_dst_start - 1] == '\0') _dst_start--; /* overwrite existing terminator */\
}\
size_t _needed = _dst_start + _src_len + 1; /* +1 for '\0' */\
if (_needed > (array).capacity) {\
if ((array).capacity == 0) (array).capacity = 256;\
while (_needed > (array).capacity) (array).capacity *= 2;\
(array).items_ref = WMemRealloc((array).items_ref, (array).capacity * sizeof(char));\
}\
WMemRef *_ref = WMemRefFromOffset((array).items_ref);\
char *_buf = (char*)_ref->ptr;\
memcpy(_buf + _dst_start, _src, _src_len);\
_buf[_dst_start + _src_len] = '\0';\
(array).size = _dst_start + _src_len + 1;\
} while(0)

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
    size_t last_index;
    size_t item_size;
    WArray items;
} WList;

#define declare_list_item(type)\
    typedef struct {\
        type item;\
        size_t prev;\
        size_t next;\
    } type##InList;

#define list_new(label, type)\
    WList label = {0};\
    do {\
        arr_new( inner_arr_##label )\
        label.items = inner_arr_##label;\
        label.item_size = sizeof(type);\
        label.starting_index = NULL_OFFSET;\
        label.last_index = NULL_OFFSET;\
        label.list_size = 0;\
    } while(0)

#define foreach_list(item_type, item_label, target_list, operations)\
size_t idx = (target_list).starting_index;\
for (size_t n = 0; n < (target_list).list_size && idx != NULL_OFFSET; n++) {\
    item_type##InList node = arr_get(item_type##InList, (target_list).items, idx);\
    item_type item_label = node.item;\
    \
    operations\
    \
    idx = node.next;\
};

#define foreach_list_inverted(item_type, item_label, target_list, operations)\
{\
size_t idx = (target_list).last_index;\
for (size_t n = 0; n < (target_list).list_size && idx != NULL_OFFSET; n++) {\
    item_type##InList node = arr_get(item_type##InList, (target_list).items, idx);\
    item_type item_label = node.item;\
    \
    operations\
    \
    idx = node.prev;\
    };\
}

void list_insert_at(WList* target_list, void* item_to_add, size_t target_pos);
void list_insert_first(WList* target_list, void* item_to_add);
void list_insert_last(WList* target_list, void* item_to_add);
void list_remove_at(WList* target_list, size_t in_target_pos);
void list_remove_first(WList* target_list);
void list_remove_last(WList* target_list);

// insert at position
// delete at position