#include <stdio.h>
#include "stack.h"
#include "memory_functions.h"



int main() {

    Stack my_stack;
    Stack_data r;
    int val[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    stack_init(&my_stack, 10);
    for (int i = 0; i < 10; i ++) 
        stack_push(&my_stack, val+i);

    //stack_data_print(&my_stack);


    stack_pop(&my_stack, &r);
    //stack_data_print(&my_stack);
    return 0;
}


//init
//push
//pop
//print
//dump
//resize
//destroy

//structure stack
