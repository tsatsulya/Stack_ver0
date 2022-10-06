#include "stack.h"


#define min(x, y) (x < y) ? x : y
#define max(x, y) (x > y) ? x : y

static Canary_data CANARY = 234235243634634; // TODO: hex speak!

#define stack_rehash_(stack)                            \
    ({                                                  \
        if (CHECK_SECURITY)                             \
            stack_rehash(stack);                               \
    })  



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


size_t stack_get_full_buffer_size(size_t length) {

    #ifdef CHECK_SECURITY
        size_t size = length * sizeof(Stack_data) + 2 * sizeof(Canary_data);
    #else
        size_t size = length * sizeof(Stack_data);
    #endif

    return size;
}

void* stack_get_data_base_ptr(Stack* stack) {

    #ifdef CHECK_SECURITY
        void* ptr_to_free = (char*)stack->data - sizeof(Canary_data);
    #else
        void* ptr_to_free = stack->data;
    #endif

    return ptr_to_free;
}



static int change_stack_length(Stack* stack, int multiplier) { // TODO: you can add min_size

    char* data_start = (char*)stack->data;
    #ifdef CHECK_SECURITY
        data_start -= sizeof(Canary_data); // TODO: stack_get_base_data_ptr?
    #endif

    size_t length_before_resize =  stack->length;

    stack->length = max(1, length_before_resize * multiplier);

    size_t new_size = stack_get_full_buffer_size(stack->length);

    data_start = (char*) realloc_(stack->data, new_size);

    // (data_start) realloc failed????

    #ifdef CHECK_SECURITY 
        stack->data = (Stack_data*) set_canaries(data_start, stack->length);
        stack_rehash(stack);
    #else
        stack->data = (stack_data_t*)data_start;
    #endif
    
    // (stack)

    return SUCCESS;

}

static int stack_length_up(Stack* stack) {
    return change_stack_length(stack, 2);
}

static int stack_length_down(Stack* stack) {
    return change_stack_length(stack, 0.5); // TODO: int multiplier?
}



int stack_init(Stack* stack, size_t length) {

    if ( !stack ) 
        return STACK_NULL;
    
    *stack = { .length = length, 
            .element_count = 0,
            .data = (Stack_data*) calloc(stack_get_full_buffer_size(length), sizeof(char)),
            .is_alive = true,
            .hash = 0
    };

    stack_rehash_(stack);

    //(stack)

    return SUCCESS; 
}

int stack_push(Stack* stack, Stack_data* element) {
    
    //

    debug_foo(ULTRA_MEGA_DEBUG_, stack_print(stack));

    stack_resize(stack); 

    //

    stack->element_count++;
    stack->data[stack->element_count-1] = *element;

    debug_foo(ULTRA_MEGA_DEBUG_, stack_print(stack));

    //

    #ifdef CHECK_SECURITY 
        stack_rehash(stack);
    #endif

    return SUCCESS;
}

int stack_pop(Stack* stack, Stack_data* last_element) {

    //
    
    if ( check_empty_stack(stack) == EMPTY )
        return EMPTY_STACK;

    *last_element = stack->data[--stack->element_count]; 

    stack_resize(stack); // если занято меньше половины, уменьшить!!!!!

    stack_rehash_(stack);
    
    //

    return SUCCESS;
    
}

int stack_resize(Stack* stack) {

    // (stack)

    int stack_full = check_full_stack(stack);

    if ( stack_full == FULL )
        stack_length_up(stack);
    
    if ( stack_full == EMPTY ) // TODO: better to use different strategy
        stack_length_down(stack);

    // (stack)
    return SUCCESS;
}

int stack_destruct(Stack* stack) {

    // (stack)
    
    void* ptr_to_free = stack_get_data_base_ptr(stack);

    free(ptr_to_free);

    stack->is_alive = false; // TODO: don't you like named designators still?
    stack->length = 0;
    stack->hash = 0;
    

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



int stack_rehash(Stack* stack) {
    // (stack)

    int data_hash = gnu_hash(stack->data, stack->length * sizeof(Stack_data));
    int struct_hash = gnu_hash(stack, sizeof(Stack));

    stack->hash = data_hash ^ struct_hash;
    
    // (stack)
    return SUCCESS; 

}




void* set_canaries(char* data, size_t length) {
    // (data)

    char* start = data;
    char* end = (char*) data + sizeof(Canary_data) + 
                ((int) length) * sizeof(Stack_data);

    *(long long *)start = *(long long *)end = CANARY;


    return start + sizeof(Canary_data);
}



int check_stack_hash(Stack* stack) {
    
    unsigned long int correct_stack_hash = stack->hash;

    unsigned long int actual_stack_hash = gnu_hash(stack->data, stack->length * sizeof(Stack_data)) +
                     gnu_hash(stack, sizeof(Stack));
    
    return (actual_stack_hash == correct_stack_hash) ? SUCCESS : INVALID_HASH;

}

int check_stack_canaries(Stack* stack) {

    if (*((long long int*)stack->data - 1) != CANARY) 
        return DAMAGED_LEFT_CANARY;

    if (*(long long int*)(stack->data + stack->length) != CANARY) 
        return DAMAGED_RIGHT_CANARY;

    return SUCCESS;
}

int stack_base_assert(Stack* stack) { // (((((naming((((((
    // TODO:   ^~~~~~ not assert, more like check

    int status = SUCCESS;
    
    if ( stack->length < stack->element_count)  status |= STACK_OVERFLOW;
    if ( !stack ) status |= STACK_NULL;
    if ( !stack->data ) status |= DATA_NULL;
    if ( !stack->is_alive ) status |= DEAD_STACK;
    
    return status;
}

int check_stack_status(Stack* stack) {

    int status = SUCCESS; 

    status |= stack_base_assert(stack);
    status |= check_stack_canaries(stack);
    status |= check_stack_hash(stack);

    return status;
}

int stack_assert_invariants(Stack* stack) {

    int status = SUCCESS;                  

    if (!stack) return STACK_NULL;

    #ifdef CHECK_SECURITY 
        status = check_stack_status(stack);
    #else
        status = stack_base_assert(stack);
    #endif
    
    if (status) {
        //print_error + dump                   
    }

    return status;

}
