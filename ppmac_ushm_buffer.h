// Buffer definitions

////////////////////////////////////////////////////////////////////////////////////////
// Base Address
#define USHM_BASE_ADDR    4000
#define USHM_BUFF_SIZE    1000
//#define _PVT_MODE_
#define _RT_MODE_                // _PVT_MODE_ or _RT_MODE_
////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////

#define MAX_BLOCK_SIZE 20

#ifdef _RT_MODE_
    #define DOUBLE_PER_POINT        NUM_AXES
#elif _PVT_MODE_
    #define DOUBLE_PER_POINT        2*NUM_AXES
#endif

// Offset between points
#define USHM_POINT_OFFSET_ADDR            (int)(2*sizeof(int)+DOUBLE_PER_POINT*sizeof(double))
#define USHM_POINT_OFFSET_INT_IDX         (int)(USHM_LINE_OFFSET_ADDR/sizeof(int))
#define USHM_POINT_OFFSET_DOUBLE_IDX      (int)(USHM_LINE_OFFSET_ADDR/sizeof(double))

// Offset between buffers
#define USHM_BUFF_OFFSET_ADDR            (int)((USHM_BUFF_SIZE)*USHM_LINE_OFFSET_ADDR)
#define USHM_BUFF_OFFSET_INT_IDX         (int)(USHM_BUFF_OFFSET_ADDR/sizeof(int))
#define USHM_BUFF_OFFSET_DOUBLE_IDX      (int)(USHM_BUFF_OFFSET_ADDR/sizeof(double))
////////////////////////////////////////////////////////////////////////////////////////