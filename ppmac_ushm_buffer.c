//#define DEBUG

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

size_t get_frame_len(char *types) {
    return strlen(types);
}

bool is_aligned(void* addr, size_t alignment) {
    // return if remainder zero for addr divided by alignment
    return ((uintptr_t)addr % alignment) == 0;
}

void* align_addr(void *addr, size_t alignment) {
    uintptr_t uint_addr = (uintptr_t) addr;
    return (void *)((uint_addr + alignment-1) & ~(alignment -1));   // uint_addr + alignment - 1
}

void set_aligned_pointer(Point *ptr_arr[], int idx, void **next_free_memory, size_t size) {
    if (!is_aligned(*next_free_memory, size)) {
        *next_free_memory = align_addr(*next_free_memory, size);
    }
    #ifdef DEBUG
    void *pushm = (void *)malloc(sizeof(void));
    #endif
    ptr_arr[idx] = (Point*)((uintptr_t)pushm + (uintptr_t)*next_free_memory);
    //printf("ptr_arr[%d]: 0x%08x\n", idx, (uintptr_t)ptr_arr[idx]);
    *next_free_memory += size;
    return;
}

int init_buffer(char *types, Point *ptr_arr[], size_t *frame_bytesize) {
    // TODO: add optional arg to get offset

    // As it is, it is equivalent to what will be if receiving 1 as argument for offset // TODO: Review this statement
    // That is necessary if using interleaved frames - buffer size is also necessary in that case:
    // Probably it needs a flag whether the frames are interleaved or not - or use it when arg[0] exists

    // Initialize buffer on USHM
    size_t frame_len = 0;
    unsigned int idx; // loop iterator
    #ifdef DEBUG
    void *pushm = (void *)malloc(sizeof(void));
    #endif
    void* base_memory = USHM_BASE_ADDR;
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
    *frame_bytesize = (size_t)(next_free_memory-base_memory);
    return 0;
}


void test_print_buffer(char *frame_types, Point *ptr_arr[]) {
    unsigned int idx;
    for (idx = 0; idx < get_frame_len(frame_types); idx++) {
        switch (frame_types[idx]) {
            case 'd':
                ptr_arr[idx]->d = 12.6;
                printf("*ptr_arr[%d]: %f\n", idx, *((double*)ptr_arr[idx]));
                break;
            case 'i':
                ptr_arr[idx]->i = 777;
                printf("*ptr_arr[%d]: %d\n", idx, *((int*)ptr_arr[idx]));

                break;
            // Extend to other types if needed
            default:
                printf("Unknown type at ptr_arr[%d]\n", idx);
        }
    }
    return;
}

int main(void)
{
    Point *ptr_arr[MAX_FRAME_NUMEL];
    unsigned int idx;    // Loop iterator
    // TODO: Make it into arg[] - Test with different settings
    // Usage example: for a scan each frame refers to a Point in the trajectory
    // They could be arranged subsequently or interleaved(not yet supported)
    // Each frame can contain MAX_FRAME_SIZE values - Currently supports: int and double
    char *frame_types = "iidd";

    size_t frame_bytesize = 0x00000000;
    //printf("%d\n",get_frame_len(frame_types));

    #ifndef DEBUG
        InitLibrary();  // Required for accessing Power PMAC library
    #endif
    init_buffer(frame_types, ptr_arr, &frame_bytesize); // TODO: return to status variable to check errors
    //printf("0x%08x\n",frame_bytesize);
    // TEST: Print the values stored at the addresses
    test_print_buffer(frame_types, ptr_arr);
	#ifndef DEBUG
        CloseLibrary();
    #endif
    return 0;
}

