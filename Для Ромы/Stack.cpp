#include "Guard.h"
//===================================================

void Stack_construct(struct Stack* stk, size_t capacity)
{
    NULL_check(stk);

    stk->canary_struct_left  = Canary;
    stk->canary_struct_right = Canary;
    stk->hash = 0;

    if      (stk->count_construct > 0)  stk->error = REPEATED_CONSTRUCTION;
    else if (capacity < 0)              stk->error = NEGATIVE_CAPACITY;
    else if (capacity == 0)             stk->error = NULL_ARRAY;
    else
    {
        stk->capacity = capacity;

        void* temp = malloc((stk->capacity)*sizeof(element_t) + 2 * sizeof(canary_t));

        if (temp == nullptr)
        {
            stk->error = OUT_OF_MEMORY;
            ASSERT_OK(stk);
        }

        canary_t* canary_array_left = (canary_t*) temp;
        *canary_array_left = Canary;

        stk->data = ((element_t*) &(canary_array_left[1]));

        canary_t* canary_array_right = (canary_t*) &(stk->data[stk->capacity]);
        *canary_array_right = Canary;

        stk->size = 0;
        stk->error = 0;
        ++(stk->count_construct);

        Poison_filling(stk, stk->size, stk->capacity);

        stk->hash = HASHFAQ6(stk);
    }
}

void Stack_push(struct Stack* stk, element_t element)
{
    NULL_check(stk);
    ASSERT_OK(stk);

    Stack_reallocation_memory(stk);

    ASSERT_OK(stk);

    stk->data[(stk->size)++] = element;
    stk->hash = HASHFAQ6(stk);

    ASSERT_OK(stk);
}

element_t Stack_pop(struct Stack* stk)
{
    NULL_check(stk);
    ASSERT_OK(stk);

    Stack_reverse_reallocation_memory(stk);

    ASSERT_OK(stk);

    element_t temp = Poison;

    if (stk->size == 0)
    {
        stk->error = NULL_POP;
        ASSERT_OK(stk);
    }

    temp = stk->data[--(stk->size)];
    stk->data[stk->size] = Poison;
    stk->hash = HASHFAQ6(stk);

    return temp;
}

void Stack_reallocation_memory(struct Stack* stk)
{
    NULL_check(stk);

    if (stk->size == (stk->capacity - 1))
    {
        void* temp = (void*) stk->data;

        temp = realloc(&((canary_t*)stk->data)[-1], 2 * stk->capacity * sizeof(element_t) + 2 * sizeof(canary_t));

        if (temp == nullptr)
        {
            temp = realloc(&((canary_t*)stk->data)[-1], 1.5 * stk->capacity * sizeof(element_t) + 2 * sizeof(canary_t));

            if (temp == nullptr)
            {
                temp = realloc(&((canary_t*)stk->data)[-1], (stk->capacity + 1) * sizeof(element_t) + 2 * sizeof(canary_t));
                
                if (temp == nullptr)
                {
                  stk->error = OUT_OF_MEMORY; 
                }
                
                else
                {   
                    stk->capacity += 1;

                    Placing_canary(stk, temp);

                    stk->data[stk->capacity-1] = Poison;
                    stk->hash = HASHFAQ6(stk);
                }

            }

            else
            {   
                stk->capacity *= 1.5;

                Placing_canary(stk, temp);
                Poison_filling(stk, stk->size + 1, stk->capacity);

                stk->hash = HASHFAQ6(stk);
            }
            
        }
        
        else
        {   
            stk->capacity *= 2;

            Placing_canary(stk, temp);
            Poison_filling(stk, stk->size + 1, stk->capacity);

            stk->hash = HASHFAQ6(stk);
        }
    }
}

void Stack_reverse_reallocation_memory(struct Stack* stk)
{
    NULL_check(stk);

    if ((stk->capacity >= 4) && (stk->size < ((stk->capacity) / 4)))
    {
        stk->capacity /= 4;

        void* temp = realloc(&((canary_t*)stk->data)[-1], (stk->capacity + 1) * sizeof(element_t) + 2 * sizeof(canary_t));

        Placing_canary(stk, temp);

        stk->hash = HASHFAQ6(stk);
    }

    ASSERT_OK(stk);
}

void Stack_destruct(struct Stack* stk)
{
    NULL_check(stk);

    free(stk->data);

    stk->data = nullptr;
    stk->capacity = -1;
    stk->size = -1;
    stk->hash = HASHFAQ6(stk);
}
