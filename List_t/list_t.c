#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "list_t.h"

/* Internal structure to simulate a list */
typedef struct {
    /* Generic list of pointers */
    void **items;
    /* Stores total size allocated */
    size_t capacity;
    /* Stores length of the list */
    size_t length;
} list_internal;

#define INIT_CAPACITY 4

/* Cast public type to internal structure */
/* Not entirely sure what this means to be honest but it works */ 
static list_internal *as_internal(list_t l) {
    return (list_internal *)l;
}

/* Create a new list */
list_t list_new() {
    /* Allocates memory for a list_internal struct */
    list_internal *li = (list_internal *)malloc(sizeof(list_internal));
    /* Checks if memory allocation failed */
    if (li == NULL) exit(1);

    /* Allocates space for the list of items from list_internal */
    li->items = (void **)malloc(sizeof(void *) * INIT_CAPACITY);
    /* Checks if memory allocation failed */
    if (li->items == NULL) exit(1);

    /* Initializes the other fields list capacity and length */
    li->capacity = INIT_CAPACITY;
    li->length = 0;

    /* Casting internal structure as a list_t object and returns it */
    return (list_t)li;
}

/* Free memory used by list */
void list_free(list_t l) {
    /* Converts the list_l back to the internal structure */
    list_internal *li = as_internal(l);

    /* Frees the array of items */
    free(li->items);
    /* Frees the list_internal struct itself */
    free(li);
}

/* Print list contents */
void list_print(list_t l) {
    /* Converts the list_l back to the internal structure */
    list_internal *li = as_internal(l);
    size_t i;

    /* Printing each element (li->items) of the list */
    printf("[");
    for (i = 0; i < li->length; i++) {
	/* uintptr_t is an integer type that holds a pointer */
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

    /* If the list is full, need to resize */
    if (li->length >= li->capacity) {
        new_capacity = li->capacity * 2;
	/* Reallocates existing data into new memory */
        new_items = (void **)realloc(li->items, sizeof(void *) * new_capacity);
	/* Checks if allocation fails */
        if (new_items == NULL) exit(1);
	/* Updates the internal pointer and capacity */
        li->items = new_items;
        li->capacity = new_capacity;
    }
}

/* Append x to list */
void list_append(list_t l, void *x) {
    /* Converts the list_l back to the internal structure */
    list_internal *li = as_internal(l);
    resize_if_needed(li);
    /* Sets the last thing in list to x, then increments the length of list */
    li->items[li->length] = x;
    li->length++;
}

/* Extend l1 with all elements of l2 */
void list_extend(list_t l1, list_t l2) {
    /* Converts the list_ls back to the internal structure */
    list_internal *li1 = as_internal(l1);
    list_internal *li2 = as_internal(l2);
    size_t i;

    /* For each thing in l2, append it to l1 */
    for (i = 0; i < li2->length; i++) {
        list_append(l1, li2->items[i]);
    }
}

/* Insert x at index i */
void list_insert(list_t l, size_t i, void *x) {
    /* Converts the list_l back to the internal structure */
    list_internal *li = as_internal(l);
    size_t j;

    /* If try to insert past length of list, just appends it */
    if (i > li->length) {
        i = li->length;
    }

    /* Resize to make sure there is space for one more thing */
    resize_if_needed(li);

    /* Shifts everything from index i onward to the right by one */
    for (j = li->length; j > i; j--) {
        li->items[j] = li->items[j - 1];
    }

    /* Insert x and update length of the list */
    li->items[i] = x;
    li->length++;
}

/* Remove first instance of x from list */
bool list_remove(list_t l, void *x) {
    /* Converts the list_l back to the internal structure */
    list_internal *li = as_internal(l);
    size_t i, j;

    /* Loop through each element of the list */
    for (i = 0; i < li->length; i++) {
	/* If the item matches the thing (by pointer) */
        if (li->items[i] == x) {
	    /* Then overwrite the item by shifting everything to the left by one */
            for (j = i; j < li->length - 1; j++) {
                li->items[j] = li->items[j + 1];
            }
	    /* Update length of the list */
            li->length--;
	    /* Returns true if item is found */
            return TRUE;
        }
    }
    /* Returns false if item is not found */
    return FALSE;
}

/* Pop element at index i */
void *list_pop(list_t l, size_t i) {
    /* Converts the list_l back to the internal structure */
    list_internal *li = as_internal(l);
    void *value;
    size_t j;

    /* Checks if the index is in range */
    if (i >= li->length) {
        fprintf(stderr, "Index out of range\n");
        exit(1);
    }

    /* Saves temp value for after removal */
    value = li->items[i];

    /* Overwrite the item by shifting everything to the left by one */
    for (j = i; j < li->length - 1; j++) {
        li->items[j] = li->items[j + 1];
    }
    /* Update the length of the list */
    li->length--;
    /* Return the value that was popped */
    return value;
}

/* Clear the list */
void list_clear(list_t l) {
    /* Converts the list_l back to the internal structure */
    list_internal *li = as_internal(l);
    /* Sets the length of the list to zero, thereby clearing it of all items */
    li->length = 0;
}

/* Return index of x or exit if not found */
size_t list_index(list_t l, void *x) {
    /* Converts the list_l back to the internal structure */
    list_internal *li = as_internal(l);
    size_t i;

    /* For every item in the list, return the index when the item to find matches */
    for (i = 0; i < li->length; i++) {
        if (li->items[i] == x) {
            return i;
        }
    }

    /* If not found, then throws an error */
    fprintf(stderr, "Element not found in list\n");
    exit(1);
}

/* Count occurrences of x */
uint64_t list_count(list_t l, void *x) {
    /* Converts the list_l back to the internal structure */
    list_internal *li = as_internal(l);
    size_t i;
    uint64_t count = 0;

    /* For every item in the list, increment counter if the item to find matches */
    for (i = 0; i < li->length; i++) {
        if (li->items[i] == x) {
            count++;
        }
    }
    /* Return the count */
    return count;
}

/* Reverse the list */
void list_reverse(list_t l) {
    /* Converts the list_l back to the internal structure */
    list_internal *li = as_internal(l);
    size_t i, j;
    void *temp;

    /* Swap first and last items
     * Increment i and decrement j
     * When they cross/meet, then stop */
    for (i = 0, j = li->length - 1; i < j; i++, j--) {
        temp = li->items[i];
        li->items[i] = li->items[j];
        li->items[j] = temp;
    }
}

/* Copy the list */
list_t list_copy(list_t l) {
    /* Converts the source list to internal structure, defines destination list */
    list_internal *src = as_internal(l);
    list_internal *dest = (list_internal *)list_new();
    size_t i;

    /* Loops through each item in the original list and appends to new one */
    for (i = 0; i < src->length; i++) {
        list_append((list_t)dest, src->items[i]);
    }

    /* Returns the destination list converted back to list_t */
    return (list_t)dest;
}

