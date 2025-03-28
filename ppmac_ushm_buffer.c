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
    void *pushm = (void *) 0xd0d91000;
#endif

#include "ppmac_ushm_buffer.h"


// typedef union {
//     double d;
//     int i;
// } point;

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

void set_aligned_pointer(point **ptr_arr, int idx, void **next_free_memory, size_t size, unsigned int shift) {
    if (!is_aligned(*next_free_memory, size)) {
        *next_free_memory = align_addr(*next_free_memory, size);
    }
    // const uintptr_t pushm_tmp = 0xd0d91000;
    // void *pushm = (void *)pushm_tmp;
    ptr_arr[idx] = (void *)((uintptr_t)pushm + ((uintptr_t)*next_free_memory >> shift));
    //ptr_arr[idx] = size;
    *next_free_memory += size;
    //printf("0x%08x\n",*next_free_memory);
    return;
}

void set_aligned_int_pointer(point **ptr_arr, int idx, void **next_free_memory, size_t size) {
    if (!is_aligned(*next_free_memory, size)) {
        *next_free_memory = align_addr(*next_free_memory, size);
    }
    //printf("next_free_memory: 0x%08x\n",(uintptr_t)*next_free_memory);
    printf("SHIFTED next_free_memory: %d\n",(uintptr_t)*next_free_memory >> INT_SHIFT);

    ptr_arr[idx] = (point*)(uintptr_t)pushm + ((uintptr_t)(*next_free_memory) >> INT_SHIFT);
    // *((int*)ptr_arr[idx]) = (int)size;;  // Correctly cast to int* before dereferencing
    ptr_arr[idx]->i = size;
    *next_free_memory += size;


//    printf("*ptr_arr[%d] : %f\n", idx, (*((double *)ptr_arr[idx])));
    printf("*ptr_arr[%d] : 0x%016lx\n", idx, ((unsigned long)ptr_arr[idx]));
    //printf("next_free_memory: 0x%08x\n", *next_free_memory);
    return;
}

void set_aligned_double_pointer(point **ptr_arr, int idx, void **next_free_memory, size_t size) {
    // TODO: Reduce overhead of multiple calls to sizeof
    if (!is_aligned(*next_free_memory, size)) {
        *next_free_memory = align_addr(*next_free_memory, size);
    }
    //printf("next_free_memory: 0x%08x\n",(uintptr_t)*next_free_memory);
    printf("SHIFTED next_free_memory: %d\n",(uintptr_t)((int)(*next_free_memory) >> DOUBLE_SHIFT));

    ptr_arr[idx] = (point*)pushm + ((uintptr_t)(*next_free_memory) >> DOUBLE_SHIFT);
    ptr_arr[idx]->d = 3.14;    // Cast size to double before assignment
    *next_free_memory += size;

//    printf("*ptr_arr[%d] : %f\n", idx, (*((double *)ptr_arr[idx])));
    printf("*ptr_arr[%d] : 0x%016lx\n", idx, ((unsigned long)ptr_arr[idx]));
    //printf("next_free_memory: 0x%08x\n",*next_free_memory);
    return;
}

int init_buffer(char *types, point *ptr_arr[], size_t *frame_bytesize) {
    // TODO: add optional arg to get offset

    // As it is, it is equivalent to what will be if receiving 1 as argument for offset // TODO: Review this statement
    // That is necessary if using interleaved frames - buffer size is also necessary in that case:
    // Probably it needs a flag whether the frames are interleaved or not - or use it when arg[0] exists

    // const uintptr_t pushm_tmp = 0xd0d91000;
    // void *pushm = (void *)pushm_tmp;

    // Initialize buffer on USHM
    size_t frame_len = 0;
    unsigned int idx; // loop iterator
    void* base_memory = USHM_BASE_ADDR;

    void* next_free_memory = base_memory;
    //printf("next free memory 0x%08x\n", next_free_memory);
    frame_len = get_frame_len(types);
    if (frame_len > MAX_FRAME_NUMEL) {
        return -1;
    }
    for (idx = 0; idx < frame_len; idx++) {
        switch (types[idx]) {
            case 'i':
                //set_aligned_pointer(ptr_arr, idx, &next_free_memory, sizeof(int), INT_SHIFT);
                set_aligned_int_pointer(ptr_arr, idx, &next_free_memory, sizeof(int));
                break;
            case 'd':
                //set_aligned_pointer(ptr_arr, idx, &next_free_memory, sizeof(double), DOUBLE_SHIFT);
                set_aligned_double_pointer(ptr_arr, idx, &next_free_memory, sizeof(double));
                break;
            default:
                return -1;
        }
    }
    *frame_bytesize = (size_t)(next_free_memory-base_memory);
    return 0;
}


void test_print_buffer(char *frame_types, point *ptr_arr[]) {
    unsigned int idx;
    for (idx = 0; idx < get_frame_len(frame_types); idx++) {
        switch (frame_types[idx]) {
            case 'i':
                printf("Value at ptr_arr[%d]: %d\n", idx, ptr_arr[idx]->i);
                printf("Addr at ptr_arr[%d]: 0x%08x\n", idx, ptr_arr[idx]);
                break;
            case 'd':
                printf("Value at ptr_arr[%d]: %f\n", idx, ptr_arr[idx]->d);
                printf("Addr at ptr_arr[%d]: 0x%08x\n", idx, ptr_arr[idx]);

                break;
            // Extend to other types if needed
            default:
                printf("Unknown type at ptr_arr[%d]\n", idx);
        }
    }
    return;
}

void assign_double_value(void **ptr_arr, int idx, double value) {
    *(double *)ptr_arr[idx] = value;
}


int main(void)
{
    point *ptr_arr[MAX_FRAME_NUMEL];
    unsigned int idx;    // Loop iterator
    // TODO: Make it into arg[] - Test with different settings
    // Usage example: for a scan each frame refers to a point in the trajectory
    // They could be arranged subsequently or interleaved(not yet supported)
    // Each frame can contain MAX_FRAME_SIZE values - Currently supports: int and double
    char *frame_types = "ddii";

    size_t frame_bytesize = 0x00000000;
    //printf("%d\n",get_frame_len(frame_types));

    #ifndef DEBUG
        InitLibrary();  // Required for accessing Power PMAC library
    #endif
    // init_buffer(frame_types, ptr_arr, &frame_bytesize); // TODO: return to status variable to check errors
    init_buffer(frame_types, ptr_arr, &frame_bytesize); // TODO: return to status variable to check errors

    printf("0x%08x\n",frame_bytesize);
    // TEST: Print the values stored at the addresses
    //test_print_buffer(frame_types, ptr_arr);
	#ifndef DEBUG
        CloseLibrary();
    #endif
    return 0;
}

