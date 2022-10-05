#include "stack.h"

static Canary_data CANARY = 234235243634634;

#define stack_rehash_(stack)                            \
    ({                                                  \
        if (CHECK_SECURITY)                             \
            stack_rehash;                               \
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

    size_t length_before_resize =  stack->length;

    stack->length = !length_before_resize + length_before_resize / 2;

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
    
    size_t size = ({
        if (CHECK_SECURITY) 
            length * sizeof(Stack_data) + 2 * sizeof(Canary_data);

        length * sizeof(Stack_data);
    });

    stack->is_alive = true;
    stack->data = (Stack_data*) calloc(size, sizeof(char));
    stack->length = length;
    stack->element_count = 0;
    stack->hash = 0;

    stack_rehash_(stack);

    return SUCCESS;
}

int stack_push(Stack* stack, Stack_data* element) {
    
    //

    debug_foo(ULTRA_MEGA_DEBUG_ , stack_print(stack));

    stack_resize(stack); 

    //

    stack->element_count++;
    stack->data[stack->element_count-1] = *element;

    debug_foo(ULTRA_MEGA_DEBUG_ , stack_print(stack));

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

    *last_element = stack->data[stack->element_count-1];
    stack->element_count--;

    stack_resize(stack); // если занято меньше половины, уменьшить!!!!!

    stack_rehash_(stack);

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
    #ifdef CHECK_SECURITY
        void* ptr_to_free = (char*)stack->data - sizeof (Canary_data);
    #else
        void* ptr_to_free = stack->data;
    #endif

    free(ptr_to_free);

    stack->is_alive = false;
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

    stack->hash = data_hash + struct_hash;
    
    // (stack)
    return SUCCESS;

}

int check_stack_hash(Stack* stack) {
    
    unsigned long int correct_stack_hash = stack->hash;

    int actual_stack_hash = gnu_hash(stack->data, stack->length * sizeof(Stack_data)) +
                     gnu_hash(stack, sizeof(Stack));
    
    return (actual_stack_hash == correct_stack_hash) ? SUCCESS : DAMAGED_HASH;

}

void* set_canaries(Stack* stack) {
    // (data)

    char* start = stack->data;
    char* end = (char*)stack->data + sizeof(Canary_data) + 
                ((int) stack->length) * sizeof(Stack_data);

    *(long long *)start = *(long long *)end = CANARY;

    return start + sizeof(Canary_data);
}

int check_stack_canaries(Stack* stack) {

    if (*((long long int*)stack->data - 1) != CANARY) 
        return DAMAGED_LEFT_CANARY;

    if (*(long long int*)(stack->data + stack->length) != CANARY) 
        return DAMAGED_RIGHT_CANARY;

    return SUCCESS;
}


int check_stack_status(Stack* stack) {

    int error_code = SUCCESS;

    error_code = stack_base_assert(stack);
    if (error_code) return error_code;

    error_code = check_stack_canaries(stack);
    if (error_code) return error_code;

    error_code = check_stack_hash(stack);
    if (error_code) return error_code;

    return error_code;
}

int stack_assert_with_security(Stack* stack) {
    int error_code = SUCCESS;                                                            
    if ( error_code = check_stack_status(stack) ) {                                                                           
        // PrintErr (ErrCode, stack_ptr, __func__, __LINE__);                      
        // StackDump (stack_ptr, __func__, __LINE__, ErrCode);                     
        return error_code;                                                         
    }  
}

int stack_assert_without_security(Stack* stack) {
    int error_code = SUCCESS;                                                            
    if ( error_code = stack_base_assert(stack) ) {                                                                           
        // PrintErr (ErrCode, stack_ptr, __func__, __LINE__);                      
        // StackDump (stack_ptr, __func__, __LINE__, ErrCode);                     
        return error_code;                                                         
    }  
}

#define stack_assert_(stack) ({                     \
    if (CHECK_SECURITY)                             \
        stack_assert_with_security(stack);          \
    else                                            \
        stack_assert_without_security(stack);       \
})  
