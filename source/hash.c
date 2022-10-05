#include "hash.h"

unsigned long int gnu_hash(void* data, size_t size) {

    unsigned long int h = 0;
    
    for (unsigned long int c = 0; c < size; c++) {    
        h = h * 33 + (char*) (data+c);
    }

    return h;
}
