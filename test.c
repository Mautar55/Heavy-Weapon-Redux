#include <assert.h>
#include <stddef.h>
#include "wutils.h"

static void *list_item_ptr(const WList *list, size_t index) {
    const size_t full_item_size = list->item_size + sizeof(size_t) * 2; // item + prev + next
    return (char *) WMemRefFromOffset(list->items.items_ref)->ptr + index * full_item_size;
}

static size_t list_prev(const WList *list, size_t index) {
    return *(size_t *) ((char *) list_item_ptr(list, index) + list->item_size);
}

static size_t list_next(const WList *list, size_t index) {
    return *(size_t *) ((char *) list_item_ptr(list, index) + list->item_size + sizeof(size_t));
}

static int list_item_int(const WList *list, size_t index) {
    return *(int *) list_item_ptr(list, index);
}

static void assert_list_integrity_and_order_int(const WList *list, const int *expected, size_t expected_count) {
    assert(list != NULL);
    assert(list->item_size == sizeof(int));
    assert(list->list_size == expected_count);

    if (expected_count == 0) {
        assert(list->starting_index == NULL_OFFSET);
        assert(list->last_index == NULL_OFFSET);
        return;
    }

    assert(list->starting_index != NULL_OFFSET);
    assert(list->last_index != NULL_OFFSET);

    size_t idx = list->starting_index;
    size_t prev = NULL_OFFSET;

    for (size_t i = 0; i < expected_count; i++) {
        assert(idx != NULL_OFFSET);
        assert(list_prev(list, idx) == prev);
        assert(list_item_int(list, idx) == expected[i]);

        prev = idx;
        idx = list_next(list, idx);
    }

    assert(idx == NULL_OFFSET);
    assert(prev == list->last_index);
}

int main(void) {
    WMemClear();

    list_new(my_list, int);
    /*{
        int exp0[] = {};
        assert_list_integrity_and_order_int(&my_list, exp0, 0);
    }*/

    int val1 = 10;
    list_insert_at(&my_list, &val1, 0);
    {
        const int exp1[] = {10};
        assert(my_list.starting_index == my_list.last_index);
        assert_list_integrity_and_order_int(&my_list, exp1, 1);
    }

    int val2 = 20;
    list_insert_at(&my_list, &val2, 1);
    {
        const int exp2[] = {10, 20};
        assert_list_integrity_and_order_int(&my_list, exp2, 2);
        assert(list_item_int(&my_list, my_list.last_index) == 20);
    }

    int val3 = 30;
    list_insert_at(&my_list, &val3, 1);
    {
        const int exp3[] = {10, 30, 20};
        assert_list_integrity_and_order_int(&my_list, exp3, 3);
        assert(list_item_int(&my_list, my_list.last_index) == 20);
    }

    int val4 = 40;
    list_insert_at(&my_list, &val4, 0);
    {
        const int exp4[] = {40, 10, 30, 20};
        assert_list_integrity_and_order_int(&my_list, exp4, 4);
        assert(list_item_int(&my_list, my_list.last_index) == 20);
    }

    list_remove_at(&my_list, 3);
    {
        const int exp5[] = {40, 10, 30};
        assert_list_integrity_and_order_int(&my_list, exp5, 3);
        assert(list_item_int(&my_list, my_list.last_index) == 30);
    }

    list_remove_at(&my_list, 0);
    {
        const int exp6[] = {10, 30};
        assert_list_integrity_and_order_int(&my_list, exp6, 2);
    }

    list_remove_at(&my_list, 0);
    {
        const int exp7[] = {30};
        assert_list_integrity_and_order_int(&my_list, exp7, 1);
        assert(my_list.starting_index == my_list.last_index);
    }

    list_remove_at(&my_list, 0);
    /*{
        const int exp8[0] = {};
        assert_list_integrity_and_order_int(&my_list, exp8, 0);
    }*/

    WMemClear();
    return 0;
}
