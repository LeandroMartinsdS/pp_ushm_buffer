// Buffer definitions

#ifndef USHM_BUFFER_H
#define USHM_BUFFER_H

////////////////////////////////////////////////////////////////////////////////////////
// Constants
#define INT_SHIFT       2   // sizeof(int) == 2^2
#define DOUBLE_SHIFT    3   // sizeof(double) == 2^3
////////////////////////////////////////////////////////////////////////////////////////
// User defined
#define USHM_BASE_ADDR    4000  // Base Address
#define USHM_BUFF_SIZE    1000
#define MAX_FRAME_NUMEL   20

// Mode
// PVT_MODE or RT_MODE
//#define PVT_MODE
//#define RT_MODE
// FRAMES_INTERLEAVED or FRAMES_SEQUENTIAL
//#define FRAMES_INTERLEAVED
//#define FRAMES_SEQUENTIAL

////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
typedef union {
    double d;
    int i;
} Point;

size_t get_frame_len(char *types);
bool is_aligned(void* addr, size_t alignment);
void* align_addr(void *addr, size_t alignment);
void set_aligned_pointer(Point *ptr_arr[], int idx, void **next_free_memory, size_t size);
int init_buffer(char *types, Point *ptr_arr[], size_t *frame_bytesize);
void set_double_value(Point *ptr_arr[], int idx, void **next_free_memory, size_t size);
// void write_frame(char *types, void *ptr_arr[], void value);
void test_print_buffer(char *frame_types, Point *ptr_arr[]);

#endif // USHM_BUFFER_H

////////////////////////////////////////////////////////////////////////////////////////