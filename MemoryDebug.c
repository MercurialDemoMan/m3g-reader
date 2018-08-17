#include "MemoryDebug.h"

//global variables
static size_t g_memory_debug_allocations = 0;
static bool   g_memory_debug_debug       = 0;

//malloc
//counts allocations
void* my_mallocEX(size_t size, int line, const char *func) {
    void *p = malloc(size);
    if(g_memory_debug_debug) {
        printf("Allocated: line %i, function %s, pointer %p[%li]\n", line, func, p, size);
    }
    g_memory_debug_allocations = g_memory_debug_allocations + 1;
    return p;
    
}

//malloc
//counts allocations
void* my_callocEX(size_t nitems, size_t size, int line, const char *func) {
    void *p = calloc(nitems, size);
    if(g_memory_debug_debug) {
        printf("Callocated: line %i, function %s, pointer %p[%li]\n", line, func, p, nitems * size);
    }
    g_memory_debug_allocations = g_memory_debug_allocations + 1;
    return p;
    
}

//free
//counts deallocations
void my_freeEX(void* p, int line, const char *func) {
    free(p);
    p = NULL;
    g_memory_debug_allocations = g_memory_debug_allocations - 1;
    if(g_memory_debug_debug) {
        printf("Deallocated: line %i, function %s, pointer %p\n", line, func, p);
    }
}

//return total allocations vs deallocations
size_t allocations(void) { return g_memory_debug_allocations; }

//turn on and off debug messages
void debug(bool x) { g_memory_debug_debug = x; }
