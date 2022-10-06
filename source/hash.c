#include "hash.h"

typedef Stack_data Hash_data;

unsigned long int gnu_hash(void* data, size_t size) {

    char* hash_data = (char*) data;

    unsigned long int h = 0;
    
    for (unsigned long int c = 0; c < size; c++) {    
        h = h * 33  + *(hash_data+c);
    }

    return h;
}
