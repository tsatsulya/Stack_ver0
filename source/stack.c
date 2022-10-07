#include "stack.h"


#define min(x, y) (x < y) ? x : y
#define max(x, y) (x > y) ? x : y




static Canary_data CANARY = 0x00000DEAD00000; // TODO: hex speak!

#define stack_rehash_(stack)                            \
    ({                                                  \
        if (CHECK_SECURITY)                             \
            stack_rehash(stack);                        \
    })  


size_t stack_get_full_buffer_size(size_t length) {

    #ifdef CHECK_SECURITY
        size_t size = length * sizeof(Stack_data) + 2 * sizeof(Canary_data);
    #else
        size_t size = length * sizeof(Stack_data);
    #endif

    debug_foo(ULTRA_MEGA_DEBUG_, printf("BUFFER SIZE: %lu\n", size));
    debug_foo(ULTRA_MEGA_DEBUG_, printf("DATA SIZE: %lu\n", length * sizeof(Stack_data)));
    debug_foo(ULTRA_MEGA_DEBUG_, printf("CANARIES SIZE: %lu\n", 2 * sizeof(Canary_data)));


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


int check_stack_hash(Stack* stack) {

    if (!stack) return STACK_NULL;
    
    unsigned long int correct_stack_hash = stack->hash;

    unsigned long int actual_stack_hash = gnu_hash(stack->data, stack->length * sizeof(Stack_data)) +
                     gnu_hash(stack, sizeof(Stack));
    
    return (actual_stack_hash == correct_stack_hash) ? SUCCESS : INVALID_HASH;

}

int check_stack_canaries(Stack* stack) {

    if (!stack) return STACK_NULL;

    if (*((long long int*)stack->data - 1) != CANARY) 
        return DAMAGED_LEFT_CANARY;

    if (*(long long int*)(stack->data + stack->length) != CANARY) 
        return DAMAGED_RIGHT_CANARY;

    return SUCCESS;
}

int stack_base_check(Stack* stack) { // (((((naming((((((

    int status = SUCCESS;
    
    if ( stack->length < stack->element_count)  status |= STACK_OVERFLOW;
    if ( !stack ) status |= STACK_NULL;
    if ( !stack->data ) status |= DATA_NULL;
    if ( !stack->is_alive ) status |= DEAD_STACK;
    
    return status;
}

int check_stack_status(Stack* stack) {

    int status = SUCCESS; 

    status |= stack_base_check(stack);
    status |= check_stack_canaries(stack);
    status |= check_stack_hash(stack);

    return status;
}

int stack_assert_invariants(Stack* stack, const char* file, int line) {

    int status = SUCCESS;                  

    if (!stack) status = STACK_NULL;
    if (status) {
        stack_dump(stack, file, line, status);
        return status;
    }
    #ifdef CHECK_SECURITY 
        status = check_stack_status(stack);
    #else
        status = stack_base_check(stack);
    #endif
    
    if (status) {
        stack_dump(stack, file, line, status);
        //print_error + dump                   
    }

    return status;

}


#define stack_assert_invariants(stack) stack_assert_invariants(stack, __FILE__, __LINE__) 



static int check_empty_stack(Stack* stack) {

    stack_assert_invariants(stack);

    if ( stack->element_count )
        return NOT_EMPTY;

    return EMPTY;
}

static int check_full_stack(Stack* stack) {
    
    stack_assert_invariants(stack);

    if ( stack->element_count == stack->length )
        return FULL;

    return NOT_FULL;
}

static int change_stack_length(Stack* stack, double multiplier) { 

    stack_assert_invariants(stack);

    char* data_start = (char*) stack_get_data_base_ptr(stack);

    size_t length_before_resize =  stack->length;

    stack->length = max(1, length_before_resize * multiplier);

    size_t new_size = stack_get_full_buffer_size(stack->length);

    data_start = (char*) realloc_(stack->data, new_size);

    if (!data_start) return NULL_PTR;

    #ifdef CHECK_SECURITY 
        stack->data = (Stack_data*) set_canaries(data_start, stack->length);
        stack_rehash(stack);
    #else
        stack->data = (stack_data_t*)data_start;
    #endif
    
    stack_assert_invariants(stack);

    return SUCCESS;

}

static int stack_length_up(Stack* stack) {

    if (!stack) return STACK_NULL;

    return change_stack_length(stack, 2);
}

static int stack_length_down(Stack* stack) {

    if (!stack) return STACK_NULL;

    return change_stack_length(stack, 0.5);
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

    set_canaries((char*) stack->data, length);
    stack_rehash_(stack);

    stack_assert_invariants(stack);

    return SUCCESS; 
}

int stack_push(Stack* stack, Stack_data* element) {
    
    stack_assert_invariants(stack);

    //debug_foo(ULTRA_MEGA_DEBUG_, stack_print(stack));

    stack_resize(stack); 

    stack_assert_invariants(stack);

    stack->element_count++;
    stack->data[stack->element_count-1] = *element;

    debug_foo(ULTRA_MEGA_DEBUG_, stack_print(stack));

    #ifdef CHECK_SECURITY 
        stack_rehash(stack);
    #endif

    stack_assert_invariants(stack);

    return SUCCESS;
}

int stack_pop(Stack* stack, Stack_data* last_element) {

    stack_assert_invariants(stack);
    
    if ( check_empty_stack(stack) == EMPTY )
        return EMPTY_STACK;

    *last_element = stack->data[--stack->element_count]; 

    stack_resize(stack); // если занято меньше половины, уменьшить!!!!!

    stack_rehash_(stack);
    
    stack_assert_invariants(stack);

    return SUCCESS;
    
}

int stack_resize(Stack* stack) {

    stack_assert_invariants(stack);

    int stack_full = check_full_stack(stack);

    if ( stack_full == FULL )
        stack_length_up(stack);
    
    if ( stack_full == EMPTY ) // TODO: better to use different strategy
        stack_length_down(stack);

    stack_assert_invariants(stack);

    return SUCCESS;
}

int stack_destruct(Stack* stack) {

    stack_assert_invariants(stack);
    
    void* ptr_to_free = stack_get_data_base_ptr(stack);

    free(ptr_to_free);

    stack->is_alive = false; // TODO: don't you like named designators still?
    stack->length = 0;
    stack->hash = 0;
    

    return SUCCESS;
}



int stack_print(Stack* stack) {

    if (!stack) return STACK_NULL;

    printf("length of stack is : %lu\n", stack->length);
    printf("num of elem in stack is : %lu\n", stack->element_count);

    return SUCCESS;
}

int stack_fprint(FILE* file, Stack* stack) {

    if (!stack) return STACK_NULL;
    fprintf (file, "length of stack is : %lu\n", stack->length);
    fprintf(file, "num of elem in stack is : %lu\n", stack->element_count);

    return SUCCESS;
}

int stack_data_print(Stack* stack) {
    
    if (!stack) return STACK_NULL;

    stack_assert_invariants(stack);

    for (int i = 0; i < (int) stack->element_count; i++) {
        printf("..elem #%d : %d\n", i+1, stack->data[i]);
    }
    return SUCCESS;
}

int stack_data_fprint(FILE* file, Stack* stack) {
    
    if (!stack) return STACK_NULL;


    for (int i = 0; i < (int) stack->element_count; i++) {
        fprintf(file, "..elem #%d : %d : %p\n", i+1, stack->data[i], stack->data+i);
    }
    return SUCCESS;
}


int stack_rehash(Stack* stack) {
    
    stack_assert_invariants(stack);

    int data_hash = gnu_hash(stack->data, stack->length * sizeof(Stack_data));
    int struct_hash = gnu_hash(stack, sizeof(Stack));

    stack->hash = data_hash ^ struct_hash;
    
    stack_assert_invariants(stack);

    return SUCCESS; 

}

void* set_canaries(char* data, size_t length) {
   
    assert(data && "null ptr!!");

    char* start = data - sizeof(Canary_data);
    char* end = (char*) data + sizeof(Canary_data) + 
                ((int) length) * sizeof(Stack_data);

    *(Canary_data*)start = *(Canary_data*)end = CANARY;
    
    return start + sizeof(Canary_data);
}


void fprint_canary(FILE* file, char* adress) {
    for (int i = (int) sizeof(Canary_data)-1; i>-1; i--) {
        fprintf(file, "%x", *((unsigned char*)adress + i));
    }
    fprintf(file, " : %p\n", adress);

}

void stack_dump(Stack* stack, const char* file, int line, int status)
{

    debug_foo(ULTRA_MEGA_DEBUG_, printf("DUMP was called from %s :: %s :: %d\n", file, "aosijfoisd", line));

    FILE* dump_file = fopen ("dump.txt", "w");
    assert(dump_file && "opening file error");

    fprintf (dump_file, "DUMP was called from %s :: %s :: %d\n", file, "aosijfoisd", line);

    fprintf (dump_file, "The stack address: [%p]\n", stack);


    if ((status | STACK_NULL) == status)
    {
        fprintf(dump_file, "Dump aborted, stack has NULL pointer");
        fclose (dump_file);
        return;
    }

    fprintf (dump_file, "The data begining address: [%p]\n", stack->data);

    if ((status | DATA_NULL) == status)
    {
        fprintf (dump_file, "Dump aborted, data has NULL pointer\n");
        fclose (dump_file);
        return;

    }
    

    char* start = (char*) stack->data - sizeof(Canary_data);
    char* end = (char*) stack->data + sizeof(Canary_data) + 
                ((int) stack->length) * sizeof(Stack_data);

    fprintf(dump_file, "...left canary : ");
    fprint_canary(dump_file, start);
    stack_data_fprint(dump_file, stack);
    fprintf(dump_file, "...right canary : ");
    fprint_canary(dump_file, end);


    if ((status | STACK_OVERFLOW) == status)
    {
        fprintf (dump_file, "StackOverflow!\n");
        stack_fprint(dump_file, stack);
        return;
    }


    fprintf (dump_file, "length of stack is : %lu\n", stack->length);
    fprintf(dump_file, "num of elem in stack is : %lu\n", stack->element_count);

    fclose (dump_file);
    dump_file = NULL;

    return;
}
