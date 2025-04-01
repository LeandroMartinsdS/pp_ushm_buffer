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

#include "memory_pool.h"

#ifdef DEBUG
    void *pushm = NULL;
#endif

void init_memory_pool(MemoryPool *pool, void *base) {
    pool->base = base;
    pool->current = base;
    pool->size = 0; // Set size to 0 once
}

int main(void)
{
    #ifdef DEBUG
    pushm = (void *)malloc(sizeof(pushm)); // HACK
    // printf("sizeof(void): %zu\n", sizeof(char)); // Should fail in standard C
    #else
    OpenLibrary();
    #endif


    #ifdef DEBUG
        free(pushm);
    #else
        CloseLibrary();
    #endif
    return 0;
}