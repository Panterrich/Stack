#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#define DOUBLE_T

#ifdef DOUBLE_T
    typedef double element_t;
#endif

#ifdef INT_T
    typedef int element_t;
#endif

#ifdef CHAR_T
    typedef char element_t;
#endif

//===================================================
typedef unsigned long long canary_t;

const canary_t Canary = 0xBADF00DDEADBEAF;

//===================================================

struct Stack
{
    canary_t canary_struct_left;
    size_t size;
    size_t capacity;
    element_t* data;
    int error;
    size_t count_construct;
    unsigned int hash;
    canary_t canary_struct_right;
};

//===================================================

void Stack_construct(struct Stack* stk, size_t capacity);

void Stack_push(struct Stack* stk, element_t element);

element_t Stack_pop(struct Stack* stk);

void Stack_reallocation_memory(struct Stack* stk);

void Stack_destruct(struct Stack* stk);

void Stack_reverse_reallocation_memory(struct Stack* stk);
