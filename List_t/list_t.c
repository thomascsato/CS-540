#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "list_t.h"

/* Internal structure to simulate a list */
typedef struct {
    void **items;
    size_t capacity;
    size_t length;
} list_internal;

#define INIT_CAPACITY 4

/* Cast public type to internal structure */
static list_internal *as_internal(list_t l) {
    return (list_internal *)l;
}

/* Create a new list */
list_t list_new() {
    list_internal *li = (list_internal *)malloc(sizeof(list_internal));
    if (li == NULL) exit(1);

    li->items = (void **)malloc(sizeof(void *) * INIT_CAPACITY);
    if (li->items == NULL) exit(1);

    li->capacity = INIT_CAPACITY;
    li->length = 0;
    return (list_t)li;
}

/* Free memory used by list */
void list_free(list_t l) {
    list_internal *li = as_internal(l);
    free(li->items);
    free(li);
}

/* Print list contents */
void list_print(list_t l) {
    list_internal *li = as_internal(l);
    size_t i;

    printf("[");
    for (i = 0; i < li->length; i++) {
        printf("%lu", (uint64_t)(uintptr_t)li->items[i]);
        if (i < li->length - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

/* Ensure capacity for more elements */
static void resize_if_needed(list_internal *li) {
    void **new_items;
    size_t new_capacity;

    if (li->length >= li->capacity) {
        new_capacity = li->capacity * 2;
        new_items = (void **)realloc(li->items, sizeof(void *) * new_capacity);
        if (new_items == NULL) exit(1);
        li->items = new_items;
        li->capacity = new_capacity;
    }
}

/* Append x to list */
void list_append(list_t l, void *x) {
    list_internal *li = as_internal(l);
    resize_if_needed(li);
    li->items[li->length++] = x;
}

/* Extend l1 with all elements of l2 */
void list_extend(list_t l1, list_t l2) {
    list_internal *li1 = as_internal(l1);
    list_internal *li2 = as_internal(l2);
    size_t i;

    for (i = 0; i < li2->length; i++) {
        list_append(l1, li2->items[i]);
    }
}

/* Insert x at index i */
void list_insert(list_t l, size_t i, void *x) {
    list_internal *li = as_internal(l);
    size_t j;

    if (i > li->length) {
        i = li->length;
    }

    resize_if_needed(li);

    for (j = li->length; j > i; j--) {
        li->items[j] = li->items[j - 1];
    }
    li->items[i] = x;
    li->length++;
}

/* Remove first instance of x from list */
bool list_remove(list_t l, void *x) {
    list_internal *li = as_internal(l);
    size_t i, j;

    for (i = 0; i < li->length; i++) {
        if (li->items[i] == x) {
            for (j = i; j < li->length - 1; j++) {
                li->items[j] = li->items[j + 1];
            }
            li->length--;
            return TRUE;
        }
    }
    return FALSE;
}

/* Pop element at index i */
void *list_pop(list_t l, size_t i) {
    list_internal *li = as_internal(l);
    void *value;
    size_t j;

    if (i >= li->length) {
        fprintf(stderr, "Index out of range\n");
        exit(1);
    }

    value = li->items[i];

    for (j = i; j < li->length - 1; j++) {
        li->items[j] = li->items[j + 1];
    }
    li->length--;
    return value;
}

/* Clear the list */
void list_clear(list_t l) {
    list_internal *li = as_internal(l);
    li->length = 0;
}

/* Return index of x or exit if not found */
size_t list_index(list_t l, void *x) {
    list_internal *li = as_internal(l);
    size_t i;

    for (i = 0; i < li->length; i++) {
        if (li->items[i] == x) {
            return i;
        }
    }

    fprintf(stderr, "Element not found in list\n");
    exit(1);
}

/* Count occurrences of x */
uint64_t list_count(list_t l, void *x) {
    list_internal *li = as_internal(l);
    size_t i;
    uint64_t count = 0;

    for (i = 0; i < li->length; i++) {
        if (li->items[i] == x) {
            count++;
        }
    }
    return count;
}

/* Reverse the list */
void list_reverse(list_t l) {
    list_internal *li = as_internal(l);
    size_t i, j;
    void *temp;

    for (i = 0, j = li->length - 1; i < j; i++, j--) {
        temp = li->items[i];
        li->items[i] = li->items[j];
        li->items[j] = temp;
    }
}

/* Copy the list */
list_t list_copy(list_t l) {
    list_internal *src = as_internal(l);
    list_internal *dest = (list_internal *)list_new();
    size_t i;

    for (i = 0; i < src->length; i++) {
        list_append((list_t)dest, src->items[i]);
    }
    return (list_t)dest;
}

