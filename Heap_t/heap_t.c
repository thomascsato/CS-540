#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct heap_struct {
    size_t ele_size;  /* size of element */
    size_t num_eles;  /* number of elements */
    size_t capacity;  /* current capacity of the heap */
    bool (*gt)(void *, void *);  /* comparison function for "greater than" */
    void *eles;  /* pointer to heap elements */
} heap_t;

/* Helper function to swap two elements in the heap */
static void swap(void *a, void *b, size_t ele_size) {
    void *temp = malloc(ele_size);
    if (!temp) {
        return;  /* Handle memory allocation failure */
    }
    memcpy(temp, a, ele_size);
    memcpy(a, b, ele_size);
    memcpy(b, temp, ele_size);
    free(temp);
}

/* Helper function to heapify (sift-down) from a given index */
static void sift_down(heap_t *heap, size_t index) {
    size_t left_child = 2 * index + 1;
    size_t right_child = 2 * index + 2;
    size_t largest = index;

    if (left_child < heap->num_eles && heap->gt((char *)heap->eles + left_child * heap->ele_size, (char *)heap->eles + largest * heap->ele_size)) {
        largest = left_child;
    }
    if (right_child < heap->num_eles && heap->gt((char *)heap->eles + right_child * heap->ele_size, (char *)heap->eles + largest * heap->ele_size)) {
        largest = right_child;
    }

    if (largest != index) {
        swap((char *)heap->eles + index * heap->ele_size, (char *)heap->eles + largest * heap->ele_size, heap->ele_size);
        sift_down(heap, largest);
    }
}

/* Initialize a heap */
heap_t heap(size_t ele_size, bool (*gt)(void *, void *)) {
    heap_t h;
    h.ele_size = ele_size;
    h.num_eles = 0;
    h.capacity = 10;  /* Start with an arbitrary small capacity */
    h.gt = gt;
    h.eles = malloc(h.capacity * ele_size);
    if (!h.eles) {
        exit(1);  /* Handle memory allocation failure */
    }
    return h;
}

/* Insert an element into the heap */
void insert(heap_t *heap, void *ele_ptr) {
    if (heap->num_eles == heap->capacity) {
        /* Double the capacity if needed */
        heap->capacity *= 2;
        heap->eles = realloc(heap->eles, heap->capacity * heap->ele_size);
        if (!heap->eles) {
            exit(1);  /* Handle memory allocation failure */
        }
    }

    /* Copy the new element into the heap */
    memcpy((char *)heap->eles + heap->num_eles * heap->ele_size, ele_ptr, heap->ele_size);
    heap->num_eles++;

    /* Heapify (bubble up) to maintain the heap property */
    size_t index = heap->num_eles - 1;
    while (index > 0) {
        size_t parent = (index - 1) / 2;
        if (heap->gt((char *)heap->eles + index * heap->ele_size, (char *)heap->eles + parent * heap->ele_size)) {
            swap((char *)heap->eles + index * heap->ele_size, (char *)heap->eles + parent * heap->ele_size, heap->ele_size);
            index = parent;
        } else {
            break;
        }
    }
}

/* Pop the maximum element from the heap */
void *maxpop(heap_t *heap) {
    if (heap->num_eles == 0) {
        return NULL;  /* Heap is empty */
    }

    /* Allocate memory for the element to return */
    void *max_element = malloc(heap->ele_size);
    if (!max_element) {
        exit(1);  /* Handle memory allocation failure */
    }

    /* Copy the maximum element (the root) to return */
    memcpy(max_element, heap->eles, heap->ele_size);

    /* Replace the root with the last element in the heap */
    heap->num_eles--;
    memcpy(heap->eles, (char *)heap->eles + heap->num_eles * heap->ele_size, heap->ele_size);

    /* Sift down from the root to maintain the heap property */
    sift_down(heap, 0);

    return max_element;
}

