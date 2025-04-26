#include <stdbool.h>
#include <stdlib.h>

typedef struct heap_struct {
    /* These may be changed, but probably are okay */
    size_t ele_size; /* size of element */
    size_t num_eles; /* number of elements */
    size_t capacity; /* current capacity of the heap */
    bool (*gt)(void *, void *); /* for "greater than" */
    /* You must store all data elements s.t. reachable from 1 pointer */
    void *eles;
} heap_t;

/* 
 * Initialize a _max_ heap that 
 * takes elemenets of size "ele_size" and
 * compares them via comparator "gt" 
 */
heap_t heap(size_t ele_size, bool (*gt)(void *, void *));

/* 
 * Insert the element of size "heap.ele_size"
 * Use e.g. memcpy to make a local copy
 * At location "ele_ptr" 
 * Into heap_t "heap" 
 * _Be sure to maintain the heap property (heapify)_
 */
void insert(heap_t *heap, void *ele_ptr);


/*
 * Remove and return the maximum element from heap_t "heap"
 * Return a point to a malloc'ed copy of the element
 * _Be sure to maintain the heap property (siftdown)_
 */
void *maxpop(heap_t *heap);
