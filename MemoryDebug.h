// used for keeping track of number of memory allocations and deallocations
// created by Silent Akufishi

#ifndef MEMORY_DEBUG_H
#define MEMORY_DEBUG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define my_malloc(x)   my_mallocEX(x,__LINE__,__FUNCTION__)
#define my_free(x)     my_freeEX(x,__LINE__,__FUNCTION__)
#define my_calloc(x,y) my_callocEX(x,y,__LINE__,__FUNCTION__)

//same as malloc
void* my_mallocEX(size_t size,   int line, const char *func);
//same as calloc
void* my_callocEX(size_t nitems, size_t size, int line, const char *func);
//same as free
void  my_freeEX  (void* p,       int line, const char *func);

//returns difference between allocations and deallocations
//allocations() -> (number of malloc()) - (number of free())
size_t allocations(void);

//turning off and on debug information about allocations and deallocations
//when calling my_mallocEX and my_freeEX information about allocating
//and deallocating will be displayed when debug is turned on
#define ON  1
#define OFF 0
void   debug(bool x);

#endif
