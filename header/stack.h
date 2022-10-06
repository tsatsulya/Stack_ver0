#pragma once

#include "debug.h"
#include "hash.h"
#include "memory_functions.h"
#include "my_types.h"

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>


#define CHECK_SECURITY 1 // -TODO: better to use -D



typedef struct Stack {
    size_t length; // TODO: maybe use capacity instead
    size_t element_count;
    Stack_data* data;
    bool is_alive;
    unsigned long int hash;
} Stack;

// Google antonim-Sparse Hash

// struct my_cool_struct {
//     long long x;
//     char y;
//     // ...
// };

// Alignment:
// [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
// \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/
// \_____/ \_____/ \_____/ \_____/
// \_____________/ \_____________/
// \_____________________________/
// %4 == 0


enum stack_error_list {
    SUCCESS =               0b000000000, 
    STACK_NULL =            0b000000001,
    EMPTY_STACK =           0b000000010,
    STACK_OVERFLOW =        0b000000100,
    DATA_NULL =             0b000001000,
    DEAD_STACK =            0b000010000,
    INVALID_HASH =          0b000100000,
    DAMAGED_LEFT_CANARY =   0b001000000,
    DAMAGED_RIGHT_CANARY =  0b010000000,
};

// TODO: There's a technic:
// EMPTY     = 0b0001
// NOT_EMPTY = 0b0010
// NOT_FULL  = 0b0100
// FULL      = 0b1000

// EMPTY | NOT_FULL ==> 0b0101
// (value & NOT_FULL) instead of (value == EMPTY || value == NOT_FULL)

enum fullness {
    EMPTY = 0,
    NOT_EMPTY = 1,
    NOT_FULL = 2,
    FULL = 3,
};


int stack_init(Stack* stack, size_t);

int stack_push(Stack*, Stack_data*);

int stack_pop(Stack*, Stack_data*);

int stack_print(Stack*); // TODO: maybe make static in .c or rename in releation to dump

int stack_resize(Stack*); // TODO: std::vector::resize(new_capacity)

int stack_destruct(Stack*);

int check_stack_status(Stack*);
//  ^~~~~ ^~~~~ художественный прием инверсия

int stack_assert_with_security(Stack*);
// TODO: Maybe stack_check_invariants or stack_

int stack_assert_without_security(Stack*);

int stack_data_print(Stack*); // [1, 2, 3, 4]

int stack_rehash(Stack*);     // TODO: probably should be static in .cpp

int check_stack_hash(Stack*); // TODO: static

void* set_canaries(char* data, size_t length); // TODO: add stack

int check_stack_canaries(Stack*);
