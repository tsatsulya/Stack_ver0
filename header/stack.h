#include <stdio.h>
#include <stdbool.h>
#include "hash.h"
#include "memory_functions.h"
#include <assert.h>
#include "debug.h"

#define CHECK_SECURITY 1
typedef int Stack_data;
typedef const long long int Canary_data;

typedef struct Stack {
    size_t length;
    size_t element_count;
    Stack_data* data;
    bool is_alive;
    unsigned long int hash;
} Stack;


enum error_list {
    SUCCESS = 0,
    STACK_NULL = 1,
    EMPTY_STACK = 2,
    STACK_OVERFLOW = 3,
    DATA_NULL = 4,
    DEAD_STACK = 5,
    DAMAGED_HASH = 6,
    DAMAGED_LEFT_CANARY = 7,
    DAMAGED_RIGHT_CANARY = 8,

};

enum fullness {
    EMPTY = 0,
    NOT_EMPTY = 1,
    NOT_FULL = 2,
    FULL = 3,
};


int stack_init(Stack*, size_t);

int stack_push(Stack*, Stack_data*);

int stack_pop(Stack*, Stack_data*);

int stack_print(Stack*);

int stack_resize(Stack*);

int stack_destruct(Stack*);

int stack_assert(Stack*);

int stack_data_print(Stack*);

unsigned long int gnu_hash(void* data, size_t size); 