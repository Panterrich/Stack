#include "Stack.h"

//===================================================

int main()
{
    struct Stack stk = {}; 
    Stack_construct(&stk, 1);

    printf("0x%lx 0x%lx\n\n", *(stk.data - sizeof(canary_t)), *((canary_t*)&(stk.data[stk.capacity])));
    fflush(stderr);
    Stack_push(&stk, 20);

    printf("0x%lx 0x%lx\n\n", *(stk.data - sizeof(canary_t)), *((canary_t*)&(stk.data[stk.capacity])));
    fflush(stderr);
    Stack_push(&stk, 30);
    printf("0x%lx 0x%lx\n\n", *(stk.data - sizeof(canary_t)), *((canary_t*)&(stk.data[stk.capacity])));
    Stack_push(&stk, 40);
    printf("0x%lx 0x%lx\n\n", *(stk.data - sizeof(canary_t)), *((canary_t*)&(stk.data[stk.capacity])));
    Stack_push(&stk, 50);
    printf("0x%lx 0x%lx\n\n", *(stk.data - sizeof(canary_t)), *((canary_t*)&(stk.data[stk.capacity])));

    printf("Pop: %lg\n", Stack_pop(&stk));
    printf("Pop: %lg\n", Stack_pop(&stk));
    printf("Pop: %lg\n", Stack_pop(&stk));
    printf("Pop: %lg\n", Stack_pop(&stk));
    printf("Pop: %lg\n", Stack_pop(&stk));



    Stack_destruct(&stk);
}

