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
    #define pushm 0xd0d91000
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

void set_aligned_pointer(void **ptr_arr, int idx, void **next_free_memory, size_t size, unsigned int shift) {
    if (!is_aligned(*next_free_memory, size)) {
        *next_free_memory = align_addr(*next_free_memory, size);
    }
    ptr_arr[idx] = (void *)((uintptr_t)pushm + ((uintptr_t)*next_free_memory >> shift));
    *next_free_memory += size;
    //printf("0x%08x\n",*next_free_memory);
    return;
}

int init_buffer(char *types, void *ptr_arr[], size_t *frame_bytesize) {
    // TODO: add optional arg to get offset

    // As it is, it is equivalent to what will be if receiving 1 as argument for offset // TODO: Review this statement
    // That is necessary if using interleaved frames - buffer size is also necessary in that case:
    // Probably it needs a flag whether the frames are interleaved or not - or use it when arg[0] exists

    // Initialize buffer on USHM
    size_t frame_len = 0;
    unsigned int idx; // loop iterator
    void* base_memory = pushm + (void*)USHM_BASE_ADDR;
    void* next_free_memory = base_memory;
    //printf("next free memory 0x%08x\n", next_free_memory);
    frame_len = get_frame_len(types);
    if (frame_len > MAX_FRAME_NUMEL) {
        return -1;
    }

    for (idx = 0; idx < frame_len; idx++) {
        switch (types[idx]) {
            case 'i':
                set_aligned_pointer(ptr_arr, idx, &next_free_memory, sizeof(int), INT_SHIFT);
                break;
            case 'd':
                set_aligned_pointer(ptr_arr, idx, &next_free_memory, sizeof(double), DOUBLE_SHIFT);
                break;
            default:
                return -1;
        }
    }
    *frame_bytesize = (size_t)(next_free_memory-base_memory);
    return 0;
}


void test_print_buffer(char *frame_types, void *ptr_arr[]) {
    unsigned int idx;
    for (idx = 0; idx < get_frame_len(frame_types); idx++) {
        switch (frame_types[idx]) {
            case 'i':
                printf("%d\n", ((int*)ptr_arr[idx]));
                printf("Value at ptr_arr[%d]: %d\n", idx, *(int *)ptr_arr[idx]);
                printf("Addr at ptr_arr[%d]: 0x%08x\n", idx, ptr_arr[idx]);
                break;
            case 'd':
                printf("Value at ptr_arr[%d]: %f\n", idx, *(double *)ptr_arr[idx]);
                printf("Addr at ptr_arr[%d]: 0x%08x\n", idx, ptr_arr[idx]);

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
    void *ptr_arr[MAX_FRAME_NUMEL];
    unsigned int idx;    // Loop iterator
    // TODO: Make it into arg[] - Test with different settings
    // Usage example: for a scan each frame refers to a point in the trajectory
    // They could be arranged subsequently or interleaved(not yet supported)
    // Each frame can contain MAX_FRAME_SIZE values - Currently supports: int and double
    char *frame_types = "ddddiiii";

    size_t frame_bytesize = 0x00000000;
    //printf("%d\n",get_frame_len(frame_types));

    #ifndef DEBUG
        InitLibrary();  // Required for accessing Power PMAC library
    #endif
    init_buffer(frame_types, ptr_arr, &frame_bytesize); // TODO: return to status variable to check errors
    printf("0x%08x\n",frame_bytesize);
    // TEST: Print the values stored at the addresses
    test_print_buffer(frame_types, ptr_arr);
	#ifndef DEBUG
        CloseLibrary();
    #endif
    return 0;
}

