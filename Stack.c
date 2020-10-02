#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

typedef double element_t;

#define asserted || printf("ERROR on %d\n", __LINE__)

//===================================================

struct Stack
{
    element_t* data;
    size_t size;
    size_t capacity;
};

//===================================================
void Stack_construct(struct Stack* stk);

int Stack_push(struct Stack* stk, element_t element);

element_t Stack_pop(struct Stack* stk);

void Stack_reallocation_memory(struct Stack* stk);

void Stack_distruct(struct Stack* stk);
//===================================================

int main()
{
    struct Stack stk = {NULL, -1, -1}; 
    Stack_construct(&stk);

    stk.size = 0;



    Stack_distruct(&stk);
}

//===================================================
void Stack_construct(struct Stack* stk)
{
    assert(stk != NULL);
    stk->capacity = 100;
    stk->data = (element_t*) calloc(stk->capacity, sizeof(element_t));
}

int Stack_push(struct Stack* stk, element_t element)
{
    assert(stk != NULL);

    Stack_reallocation_memory(stk);

    stk->data[(stk->size)++] = element;
}

element_t Stack_pop(struct Stack* stk)
{
    assert(stk != NULL);

    return (stk->data[--(stk->size)]);
}

void Stack_reallocation_memory(struct Stack* stk)
{
     if (stk->size < (stk->capacity - 1))
    {
        struct Stack* temp = stk;

        temp->data = (element_t*) realloc(stk->data, 2 * stk->capacity * sizeof(element_t));

        if (temp->data == NULL)
        {
            temp->data = (element_t*) realloc(stk->data, 1.5 * stk->capacity * sizeof(element_t));

            if (temp->data == NULL)
            {
                temp->data = (element_t*) realloc(stk->data, (stk->capacity + 1) * sizeof(element_t));
                
                if (temp->data == NULL)
                {
                    printf("Memory reallocation ERROR\n");
                    free(stk->data);
                    abort();
                }
                
                else
                {   
                    stk->data = temp->data;
                    stk->capacity += 1;
                }

            }

            else
            {   
                stk->data = temp->data;
                stk->capacity *= 1.5;
            }
            
        }
        
        else
        {   
            stk->data = temp->data;
            stk->capacity *= 2;
        }
    }
}

void Stack_distruct(struct Stack* stk)
{
    assert(stk != NULL);

    free(stk->data);
    stk->capacity = -1;
    stk->size = -1;
}