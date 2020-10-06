#include "Guard.h"

//===================================================

int main()
{
    struct Stack stk = {}; 
    Stack_construct(&stk, 1);
   
    Stack_push(&stk, 20);
    
    Stack_push(&stk, 30);
    
    Stack_push(&stk, 40);
    
    Stack_push(&stk, 50);
    

    printf("Pop: %ld\n", Stack_pop(&stk));
    printf("Pop: %ld\n", Stack_pop(&stk));
    printf("Pop: %ld\n", Stack_pop(&stk));
    printf("Pop: %ld\n", Stack_pop(&stk));
    printf("Pop: %ld\n", Stack_pop(&stk));

    Stack_destruct(&stk);
}
