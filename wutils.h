#include <stddef.h>
#include <stdlib.h>

typedef struct {
    Vector3 position;
} CircleData;

typedef struct {
    CircleData *items;
    size_t size;
    size_t capacity;
} CircleList;

#define da_append(xs, x)\
    do {\
        if ((xs).size >= (xs).capacity) {\
            if ((xs).capacity == 0) (xs).capacity = 256;\
            else (xs).capacity *= 2;\
            (xs).items = realloc((xs).items, (xs).capacity * sizeof(*(xs).items));\
        }\
        (xs).items[(xs).size++] = x;\
    } while(0)