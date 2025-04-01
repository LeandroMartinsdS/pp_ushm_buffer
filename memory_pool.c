#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

typedef union {
    double d;
    int i;
} Point;

typedef struct {
    void *base;
    void *current;
    size_t size;
} MemoryPool;

void init_memory_pool(MemoryPool *pool, void *base) {
    pool->base = base;
    pool->current = base;
    pool->size = 0; // Set size to 0 once
}

bool is_aligned(void* addr, size_t alignment) {
    return ((uintptr_t)addr % alignment) == 0;
}

void* align_addr(void *addr, size_t alignment) {
    uintptr_t uint_addr = (uintptr_t) addr;
    return (void *)((uint_addr + alignment - 1) & ~(alignment - 1));
}

void* allocate_memory(MemoryPool *pool, size_t size) {
    uintptr_t current_addr = (uintptr_t)pool->current;
    uintptr_t aligned_addr = align_addr((void *)current_addr, size);
    pool->current = (void *)(aligned_addr + size);
    return (void *)aligned_addr;
}

void set_aligned_pointer(Point *ptr_arr[], int idx, MemoryPool *pool, size_t size) {
    void *aligned_memory = allocate_memory(pool, size);
    if (aligned_memory != NULL) {
        ptr_arr[idx] = (Point *)aligned_memory;
    }
}

int init_buffer(const char *types, Point *ptr_arr[], size_t *frame_bytesize, MemoryPool *pool) {
    size_t frame_len = strlen(types);
    if (frame_len > 8) { // Maximum of 8 variables
        return -1;
    }

    for (unsigned int idx = 0; idx < frame_len; idx++) {
        switch (types[idx]) {
            case 'd':
                set_aligned_pointer(ptr_arr, idx, pool, sizeof(double));
                break;
            case 'i':
                set_aligned_pointer(ptr_arr, idx, pool, sizeof(int));
                break;
            default:
                return -1;
        }
    }
    *frame_bytesize = (size_t)((uintptr_t)pool->current - (uintptr_t)pool->base);
    return 0;
}

void test_print_buffer(const char *frame_types, Point *ptr_arr[]) {
    for (unsigned int idx = 0; idx < strlen(frame_types); idx++) {
        switch (frame_types[idx]) {
            case 'd':
                ptr_arr[idx]->d = 12.6;
                printf("*ptr_arr[%d]->d: %f\n", idx, ptr_arr[idx]->d);
                break;
            case 'i':
                ptr_arr[idx]->i = 777;
                printf("*ptr_arr[%d]->i: %d\n", idx, ptr_arr[idx]->i);
                break;
            default:
                printf("Unknown type at ptr_arr[%d]\n", idx);
        }
    }
}

int main(void) {
    Point *ptr_arr[8]; // Maximum of 8 variables
    char *frame_types = "iidd";
    size_t frame_bytesize = 0;

    void *pushm = malloc(1024); // Example base address, replace with actual base address in your system
    MemoryPool pool;
    init_memory_pool(&pool, pushm); // Initialize without size

    if (init_buffer(frame_types, ptr_arr, &frame_bytesize, &pool) != 0) {
        fprintf(stderr, "Failed to initialize buffer\n");
        free(pushm);
        return -1;
    }

    test_print_buffer(frame_types, ptr_arr);

    free(pushm);
    return 0;
}
