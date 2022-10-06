#include "memory_functions.h"

void* realloc_(void* ptr, size_t new_size) { // TODO: name recalloc or something
    
    // (ptr)

    size_t old_size = malloc_usable_size(ptr);

    ptr = realloc(ptr, new_size);

    // (ptr)

    if ( new_size > old_size ) 
        memset((char*) ptr + old_size, '\0', new_size - old_size);
    
    return ptr;
}