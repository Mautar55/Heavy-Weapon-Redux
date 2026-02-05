#include <stdio.h>
#include <assert.h>
#include "wutils.h"

void print_list_state(WList* list, const char* msg) {
    printf("%s: size=%zu, start=%zu, last=%zu\n", msg, list->list_size, list->starting_index, list->last_index);
}

int main() {
    WMemClear();
    list_new(my_list, int);
    print_list_state(&my_list, "Initial");

    int val1 = 10;
    list_insert_at(&my_list, &val1, 0);
    print_list_state(&my_list, "After insert 10 at 0");
    // Expect: last_index == starting_index

    int val2 = 20;
    list_insert_at(&my_list, &val2, 1);
    print_list_state(&my_list, "After insert 20 at 1");
    // Expect: last_index points to 20

    int val3 = 30;
    list_insert_at(&my_list, &val3, 1);
    print_list_state(&my_list, "After insert 30 at 1");
    // Expect: last_index still points to 20

    int val4 = 40;
    list_insert_at(&my_list, &val4, 0);
    print_list_state(&my_list, "After insert 40 at 0");
    // Expect: last_index still points to 20

    printf("Removing at 3 (last element)...\n");
    list_remove_at(&my_list, 3);
    print_list_state(&my_list, "After remove at 3");
    
    printf("Removing everything...\n");
    while(my_list.list_size > 0) {
        list_remove_at(&my_list, 0);
        print_list_state(&my_list, "After remove at 0");
    }

    return 0;
}
