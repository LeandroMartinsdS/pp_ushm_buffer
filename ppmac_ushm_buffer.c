/*For more information see notes.txt in the Documentation folder */
#include <gplib.h>
#include <string.h>

#define _PPScriptMode_		// for enum mode, replace this with #define _EnumMode_
#include "../../Include/pp_proj.h"
#include "ppmac_ushm_buffer.h"

//size_t get_frame_size(char *types) {

//    return strlen(types);
//}

inline size_t get_frame_len(char *types) {
    return strlen(types);
}

inline bool is_aligned(void* addr, size_t alignment) {
    // return if remainder zero for addr divided by alignment
    return ((uintptr_t)addr % alignment) == 0;
}

inline void* align_addr(void *addr, size_t alignment){
    uintptr_t uint_addr = (uintptr_t) addr;
    return (void *)((uint_addr + alignment-1) & ~(alignment -1));   // uint_addr + alignment - 1
}

inline void set_aligned_pointer(void **ptr_arr, int idx, void **next_free_memory, size_t size, unsigned int shift) {
    if (!is_aligned(*next_free_memory, size)) {
        *next_free_memory = align_addr(*next_free_memory, size);
    }
    ptr_arr[idx] = (void *)((uintptr_t)pushm + ((uintptr_t)*next_free_memory >> shift));
    *next_free_memory += size;
    return;
}

int init_buffer(char *types, void *ptr_arr[]) {
    // TODO: add optional arg to get offset

    // As it is, it is equivalent to what will be if receiving 1 as argument for offset // TODO: Review this statement
    // That is necessary if using interleaved blocks - buffer size is also necessary in that case:
    // last_ocuppied_memory += buffer_size instead - it'd be better to find a better name for last_ocuppied_memory
    // Probably it needs a flag whether the blocks are interleaved or not - or use it when arg[0] exists


    // Initialize buffer on USHM
    size_t frame_len = 0;
    int idx; // loop iterator
    void* next_free_memory = (void*)USHM_BASE_ADDR;

    frame_len = get_frame_len(types);
    if (frame_len > MAX_FRAME_SIZE) {
        return -1;
    }

    for (idx = 0; idx < frame_len; idx++) {
        switch (types[idx]) {
            case 'i':
                set_aligned_pointer(ptr_arr, idx, &next_free_memory, sizeof(double), 3);
                break;
            case 'd':
                set_aligned_pointer(ptr_arr, idx, &next_free_memory, sizeof(double), 3);
                break;
            default:
                return -1;
        }
    }
    return 0;
}

int main(void)
{
    // TODO: Make it into arg[] - Test with different settings
    // Usage example: for a scan each frame refers to a point in the trajectory
    // They could be arranged subsequently or interleaved(not yet supported)
    // Each frame can contain MAX_FRAME_SIZE values - Currently supports: int and double
    char *frame_types = "iiiddd";

    // TODO: Make it into arg[] or calculate from frame_types
    // Necessary for offset between frames - for initializing and setting values
    size_t frame_bytesize = 5*sizeof(double); // 5 bytes

    void *ptr_arr[MAX_FRAME_SIZE];

    InitLibrary();  // Required for accessing Power PMAC library
    init_buffer(frame_types, ptr_arr); // TODO: return to status variable to check errors
    int idx;

    // TEST: Print the values stored at the addresses
    for (idx = 0; idx < get_frame_len(frame_types); idx++) {
        switch (frame_types[idx]) {
            case 'i':
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

	CloseLibrary();
	return 0;
}

