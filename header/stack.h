#include <stdio.h>
#include <stdbool.h>

typedef int Stack_data;


typedef struct Stack {
    size_t length;
    size_t element_count;
    Stack_data* data;
    bool is_alive;
} Stack;


enum ERROR_LIST {
    SUCCESS = 0,
    STACK_NULL = 1,
    EMPTY_STACK = 2,
    STACK_OVERFLOW = 3,
    DATA_NULL = 4,
    DEAD_STACK = 5
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