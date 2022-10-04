#include "stack.h"
#include "memory_functions.h"
#include <assert.h>
#include "debug.h"




static int check_empty_stack(Stack* stack) {

    //?????

    if ( stack->element_count )
        return NOT_EMPTY;

    return EMPTY;
}

static int check_full_stack(Stack* stack) {
    
    //??? что можно проверить и как показать

    if ( stack->element_count == stack->length )
        return FULL;

    return NOT_FULL;
}

static int stack_length_up(Stack* stack) {

    //

    size_t length_before_resize =  stack->length;

    stack->length = !length_before_resize + length_before_resize * 2;

    stack->data = (Stack_data*) realloc_(stack->data, stack->length);
    
    //

    return SUCCESS;
}

static int stack_length_down(Stack* stack) {

    //

    stack->length = stack->element_count;

    stack->data = (Stack_data*) realloc_(stack->data, stack->length);

    //

    return SUCCESS;
}



int stack_base_assert(Stack* stack) { // (((((naming((((((

    if ( stack->length < stack->element_count)  return STACK_OVERFLOW;
    if ( !stack ) return STACK_NULL;
    if ( !stack->data ) return DATA_NULL;
    if ( !stack->is_alive ) return DEAD_STACK;
    return SUCCESS;
}

int stack_init(Stack* stack, size_t length) {

    if ( !stack )
        return STACK_NULL;
    
    stack->is_alive = true;
    stack->data = (Stack_data*) calloc(length, sizeof(Stack_data*));
    stack->length = length;
    stack->element_count = 0;

    return SUCCESS;
}

int stack_push(Stack* stack, Stack_data* element) {
    
    //

    debug_foo(ULTRA_MEGA_DEBUG_ , stack_print(stack));

    if ( stack->element_count == stack->length ) {
        stack_resize(stack);
    } 

    //

    stack->element_count++;
    stack->data[stack->element_count-1] = *element;

    debug_foo(ULTRA_MEGA_DEBUG_ , stack_print(stack));

    //

    //* rehash

    return SUCCESS;
}

int stack_pop(Stack* stack, Stack_data* last_element) {

    //
    debug_foo(ULTRA_MEGA_DEBUG_, puts("aaaa"));
    if ( check_empty_stack(stack) == EMPTY )
        return EMPTY_STACK;

    *last_element = stack->data[stack->element_count-1];

    stack->element_count--;

    //* rehash
    // resize  down ???????
    //

    return SUCCESS;
    
}

int stack_resize(Stack* stack) {

    // (stack)

    int stack_full = check_full_stack(stack);

    if ( stack_full == FULL )
        stack_length_up(stack);
    
    if ( stack_full == EMPTY ) 
        stack_length_down(stack);

    // (stack)
    return SUCCESS;
}

int stack_destruct(Stack* stack) {

    // (stack)

    stack->is_alive = false;
    free(stack->data);
    stack->length = 0;
    

    return SUCCESS;
}

int stack_print(Stack* stack) {

    printf("length of stack is : %lu\n", stack->length);
    printf("num of elem in stack is : %lu\n", stack->element_count);

    return SUCCESS;
}

int stack_data_print(Stack* stack) {

    for (int i = 0; i < (int) stack->element_count; i++) {
        printf("..elem #%d : %d\n", i+1, stack->data[i]);
    }
    return SUCCESS;
}
// () - error_check