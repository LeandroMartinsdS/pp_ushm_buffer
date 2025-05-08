#define DEBUG

#ifndef DEBUG
    #include <gplib.h>
    #define _PPScriptMode_		// for enum mode, replace this with #define _EnumMode_
    #include "../../Include/pp_proj.h"
#else
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdint.h>
    #include <stdbool.h>
    #include <string.h>
    #include <stddef.h>
    // #define pushm 0xd0d91000
#endif

#include "ppmac_ushm_buffer.h"

#ifdef DEBUG
    void *pushm = NULL;
#endif

int init_buffer(char *types, Point *ptr_arr[], size_t *frame_bytesize, void* base_memory) {
    // TODO: add optional arg to get offset
    // Add option whether the frames are interleaved or not

    // Initialize buffer on USHM
    size_t frame_len = 0;
    unsigned int idx; // loop iterator
    void* next_free_memory = base_memory;
    frame_len = get_frame_len(types);
    if (frame_len > MAX_FRAME_NUMEL) {
        return -1;
    }

    for (idx = 0; idx < frame_len; idx++) {
        switch (types[idx]) {
            case 'd':
                set_aligned_pointer(ptr_arr, idx, &next_free_memory, sizeof(double));
                break;
            case 'i':
                set_aligned_pointer(ptr_arr, idx, &next_free_memory, sizeof(int));
                break;
            default:
                return -1;
        }
    }
    *frame_bytesize = ((char *)next_free_memory-(char *)base_memory);
    return 0;
}

size_t get_frame_len(char *types) {
    return strlen(types);
}

void set_aligned_pointer(Point *ptr_arr[], int idx, void **next_free_memory, size_t type_size) {
    if (!is_aligned(*next_free_memory, type_size)) {
        *next_free_memory = align_addr(*next_free_memory, type_size);
    }
    ptr_arr[idx] = (Point*)((uintptr_t)pushm + (uintptr_t)*next_free_memory);
    //printf("ptr_arr[%d]: 0x%08x\n", idx, (uintptr_t)ptr_arr[idx]);
    *next_free_memory = (char *)*next_free_memory + type_size;
    return;
}

bool is_aligned(void* addr, size_t alignment) {
    return ((uintptr_t)addr % alignment) == 0;
}

void* align_addr(void *addr, size_t alignment) {
    uintptr_t uint_addr = (uintptr_t) addr;
    return (void *)((uint_addr + alignment-1) & ~(alignment -1));   // uint_addr + alignment - 1
}

int update_buffer(char *types, Point *ptr_arr[], size_t size) {
    unsigned int idx; // loop iterator
    size_t frame_len = get_frame_len(types);
    if (frame_len > MAX_FRAME_NUMEL) {
        return -1;
    }

    for (idx = 0; idx < frame_len; idx++) {
        update_addr(ptr_arr, idx, size);
    }
    return 0;
}

void update_addr(Point *ptr_arr[], int idx, size_t size) {
    ptr_arr[idx] = (Point *)((uintptr_t)ptr_arr[idx] + size);
    return;
}

void write_double(Point *ptr_arr[], int idx , size_t size, double value) {
    ptr_arr[idx]->d = value;
    return;
}

void write_int(Point *ptr_arr[], int idx, size_t size, int value) {
    ptr_arr[idx]->i = value;
    return;
}

void write_frame(char *frame_types, Point *ptr_arr[], size_t size, Point values[]) {
    unsigned int idx;
    for (idx = 0; idx < get_frame_len(frame_types); idx++) {
        switch (frame_types[idx]) {
            case 'd':
//                write_double(ptr_arr, idx, size, values[idx]);
                ptr_arr[idx]->d = values[idx].d;
                break;
            case 'i':
//                write_int(ptr_arr, idx, size, value[idx]);
                ptr_arr[idx]->i = values[idx].i;
                break;
            // Extend to other types if needed
            default:
                printf("Unknown type at ptr_arr[%d]\n", idx);
        }
    }
    update_buffer(frame_types, ptr_arr, size);

    return;
}

void test_write_buffer(char *frame_types, Point *ptr_arr[], size_t size) {
    unsigned int idx;
    for (idx = 0; idx < get_frame_len(frame_types); idx++) {
        switch (frame_types[idx]) {
            case 'd':
                write_double(ptr_arr, idx, size, idx+idx/10);
                break;
            case 'i':
                write_int(ptr_arr, idx, size, idx);
                break;
            // Extend to other types if needed
            default:
                printf("Unknown type at ptr_arr[%d]\n", idx);
        }
    }
    update_buffer(frame_types, ptr_arr, size);

    return;
}

void test_print_buffer(char *frame_types, Point *ptr_arr[], size_t size) {
    unsigned int idx;
    for (idx = 0; idx < get_frame_len(frame_types); idx++) {
        switch (frame_types[idx]) {
            case 'd':
                printf("*ptr_arr[%d]: %f\n", idx, *((double*)ptr_arr[idx]));
                break;
            case 'i':
                printf("*ptr_arr[%d]: %d\n", idx, *((int*)ptr_arr[idx]));

                break;
            // Extend to other types if needed
            default:
                printf("Unknown type at ptr_arr[%d]\n", idx);
        }
    }
    update_buffer(frame_types, ptr_arr, size);

    return;
}


// int main(void)
// {
//     Point *ptr_arr[MAX_FRAME_NUMEL];
//     // TODO: Make it into arg[] - Test with different settings
//     // Usage example: for a scan each frame refers to a Point in the trajectory
//     // They could be arranged subsequently or interleaved(not yet supported)
//     // Each frame can contain MAX_FRAME_SIZE values - Currently supports: int and double
//     char *frame_types = "iidd";

//     size_t frame_bytesize = 0x00000000;
//     //printf("%d\n",get_frame_len(frame_types));

//     #ifdef DEBUG
//     pushm = (void *)malloc(sizeof(pushm)); // HACK
//     // printf("sizeof(void): %zu\n", sizeof(char)); // Should fail in standard C
//     #else
//     InitLibrary();
//     #endif

//     init_buffer(frame_types, ptr_arr, &frame_bytesize); // TODO: return to status variable to check errors
//     //printf("0x%08x\n",frame_bytesize);
//     // TEST: Print the values stored at the addresses
//     Point *values = (Point *)malloc(4 * sizeof(Point));

//     values[0].i = 7.78;
//     values[1].i = 2;
//     values[2].d = 3.3;
//     values[3].d = 4.4;
//     write_frame(frame_types, ptr_arr, frame_bytesize, values);
//     values[0].i = 5;
//     values[1].i = 6;
//     values[2].d = 7.7;
//     values[3].d = 8.8;
//     write_frame(frame_types, ptr_arr, frame_bytesize, values);

//     init_buffer(frame_types, ptr_arr, &frame_bytesize);
//     test_print_buffer(frame_types, ptr_arr, frame_bytesize);
//     test_print_buffer(frame_types, ptr_arr, frame_bytesize);

//     #ifdef DEBUG
//         free(pushm);
//     #else
//         CloseLibrary();
//     #endif
//     return 0;
// }

