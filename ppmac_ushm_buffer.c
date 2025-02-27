/*For more information see notes.txt in the Documentation folder */
#include <gplib.h>   
#include <string.h>

#define _PPScriptMode_		// for enum mode, replace this with #define _EnumMode_
#include "../../Include/pp_proj.h"
#include "ppmac_ushm_buffer.h"

//size_t get_block_size(char *types) {

//    return strlen(types);
//}

inline size_t get_block_len(char *types) {
    return strlen(types);
}

inline bool is_aligned(void* addr, size_t alignment) {
    return ((uintptr_t)addr % alignment) == 0;
}

inline void* align_addr(void *addr, size_t alignment){
    uintptr_t uint_addr = (uintptr_t) addr;
    return (void *)((uint_addr + alignment-1) & ~(alignment -1));
}

int init_buffer(char *types, void *ptr_arr[]) {
    // TODO: add optional arg to get offset
    // As it is, it is equivalent to what will be if receiving 1 as argument for offset
    // That is necessary if using interleaved blocks - buffer size is also necessary in that case:
    // last_ocuppied_memory += buffer_size instead - it'd be better to find a better name for last_ocuppied_memory
    // Probably it needs a flag whether the blocks are interleaved or not - or use it when arg[0] exists
        
    // Initialize buffer on USHM
    size_t block_len = 0;
    int idx; // loop iterator
    void* last_ocuppied_memory = (void*)USHM_BASE_ADDR;

    block_len = get_block_len(types);
    if (block_len > MAX_BLOCK_SIZE) {
        return -1;
    }

    for (idx = 0; idx < block_len; idx++) {
        switch (types[idx]) {
            case 'i':
                if (is_aligned(last_ocuppied_memory,sizeof(int))) {
                    //Round up to the next alligned 4 bytes
                    last_ocuppied_memory = align_addr(last_ocuppied_memory, sizeof(int));
                }
//                ptr_arr[idx] = (int *) pushm + (int)last_ocuppied_memory/sizeof(int);
                ptr_arr[idx] = (int *)pushm + ((uintptr_t)last_ocuppied_memory >> 2);
                last_ocuppied_memory += sizeof(int);
                break;
     
            case 'd':
                if (is_aligned(last_ocuppied_memory,sizeof(double))) {
                    //Round up to the next alligned 8 bytes
                    last_ocuppied_memory = align_addr(last_ocuppied_memory, sizeof(double));
                }
//                ptr_arr[idx] = (double *) pushm + (int)last_ocuppied_memory/sizeof(double);
                ptr_arr[idx] = (double *) pushm + ((uintptr_t)last_ocuppied_memory>>3);
                last_ocuppied_memory += sizeof(double);
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
    // Usage example: for a scan each block refers to a point in the trajectory
    // They could be arranged subsequently or interleaved(not yet supported)
    // Each block can contain MAX_BLOCK_SIZE values - Currently supports: int and double
    char *block_types = "iiiddd";
    
    // TODO: Make it into arg[] or calculate from block_types
    // Necessary for offset between blocks - for initializing and setting values
    size_t block_bytesize = 5*sizeof(double); // 5 bytes

    void *ptr_arr[MAX_BLOCK_SIZE];

    InitLibrary();  // Required for accessing Power PMAC library
    init_buffer(block_types, ptr_arr); // TODO: return to status variable to check errors
    int idx;

    // TEST: Print the values stored at the addresses
    for (idx = 0; idx < get_block_len(block_types); idx++) {
        switch (block_types[idx]) {
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

