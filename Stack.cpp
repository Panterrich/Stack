#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#define DOUBLE_T

#ifdef DOUBLE_T
    const double Poison = NAN;
    typedef double element_t;
    //#define COMPARE isnan(a) /}/ На всякий случай фишка
#endif

#ifdef INT_T
    const int Poison = 0xBADDED;
    typedef int element_t;
#endif

#ifdef CHAR_T
    const char Poison = NULL;
    typedef char element_t;
#endif

#define ASSERT_OK(a) if (Stack_ERROR(a))                                                                            \
                        {                                                                                           \
                            FILE* log = fopen("log.txt", "a");                                                      \
                            assert(log != 0);                                                                       \
                            fprintf(log, "ERROR: file %s line %d function %s\n", __FILE__, __LINE__, __FUNCTION__); \
                            Stack_dump(log, a);                                                                     \
                            abort();                                                                                \
                        }


#define case_of_switch(enum_const) case enum_const return #enum_const;
//===================================================

const unsigned long long Canary = 0xBADF00DDEADBEAF;

//===================================================

struct Stack
{
    element_t* data;
    size_t size;
    size_t capacity;
    int error;
};

//===================================================

enum ERROR 
{
    SIZE_OUT_OF_CAPACITY = 1,
    OUT_OF_MEMORY,
    NULL_ARRAY,
    NEGATIVE_SIZE,
    NEGATIVE_CAPACITY,
    NULL_POINTER_TO_ARRAY,
    ARRAY_AND_STRUCTURE_POINTERS_MATCHED,
    REPEATED_CONSTRUCTION,
    WRONG_SIZE
};

//===================================================
void Stack_construct(struct Stack* stk, size_t capacity);

void Stack_push(struct Stack* stk, element_t element);

element_t Stack_pop(struct Stack* stk);

void Stack_reallocation_memory(struct Stack* stk);

void Stack_destruct(struct Stack* stk);

void NULL_check(struct Stack* stk);

int Stack_ERROR(struct Stack* stk);

void Stack_dump(FILE* file, struct Stack* stk);

void Poison_filling(struct Stack* stk, size_t start, size_t end);

void Stack_reverse_reallocation_memory(struct Stack* stk);
//===================================================

int main()
{
    struct Stack stk = {nullptr, (size_t)(-1), (size_t)(-1), 0}; 
    Stack_construct(&stk, 1);


    Stack_destruct(&stk);
}

//===================================================
void Stack_construct(struct Stack* stk, size_t capacity)
{
    NULL_check(stk);
    
    static size_t count_construct = 0;

    if      (count_construct > 0)   stk->error = REPEATED_CONSTRUCTION;
    else if (capacity < 0)          stk->error = NEGATIVE_CAPACITY;
    else if (capacity == 0)         stk->error = NULL_ARRAY;
    else
    {
        stk->capacity = capacity;
        stk->data = (element_t*) calloc(stk->capacity, sizeof(element_t));

        if (stk->data == nullptr)
        {
            stk->error = OUT_OF_MEMORY;
        }

        else 
        {
            stk->size = 0;
            stk->error = 0;
            count_construct++;

            Poison_filling(stk, stk->size, stk->capacity);
        }
    }
    
    ASSERT_OK(stk)
}

void Stack_push(struct Stack* stk, element_t element)
{
    NULL_check(stk);
    ASSERT_OK(stk);

    Stack_reallocation_memory(stk);

    ASSERT_OK(stk);

    stk->data[(stk->size)++] = element;

    ASSERT_OK(stk);
}

element_t Stack_pop(struct Stack* stk)
{
    assert(stk != nullptr);

    Stack_reverse_reallocation_memory(stk);

    return (stk->data[--(stk->size)]);
}

void Stack_reallocation_memory(struct Stack* stk)
{
    NULL_check(stk);

    if (stk->size == (stk->capacity - 1))
    {
        element_t* temp = stk->data;

        temp = (element_t*) realloc(stk->data, 2 * stk->capacity * sizeof(element_t));

        if (temp == nullptr)
        {
            temp = (element_t*) realloc(stk->data, 1.5 * stk->capacity * sizeof(element_t));

            if (temp == nullptr)
            {
                temp = (element_t*) realloc(stk->data, (stk->capacity + 1) * sizeof(element_t));
                
                if (temp == nullptr)
                {
                  stk->error = OUT_OF_MEMORY; 
                }
                
                else
                {   
                    stk->data = temp;
                    stk->capacity += 1;
                }

            }

            else
            {   
                stk->data = temp;
                stk->capacity *= 1.5;
            }
            
        }
        
        else
        {   
            stk->data = temp;
            stk->capacity *= 2;
        }
    }
}

void Stack_destruct(struct Stack* stk)
{
    NULL_check(stk);

    free(stk->data);
    stk->capacity = -1;
    stk->size = -1;
}

void NULL_check(struct Stack* stk)
{
    if (stk == nullptr)
    {
        FILE* log = fopen("log.txt", "a");

        fprintf(log, "Stack (ERROR NULL PTR) [0x000000]\n");
        fflush(log);

        abort();
    }
}

int Stack_ERROR(struct Stack* stk)
{
    if (stk->error != 0)
    {
        return stk->error;
    }

    else 
    {
        if (stk->size >= stk->capacity)
        {
        stk->error = SIZE_OUT_OF_CAPACITY;
        return SIZE_OUT_OF_CAPACITY;
        }
        
        if (stk->capacity == 0)
        {
            stk->error == NULL_ARRAY;
            return NULL_ARRAY;
        }

        if (stk->size < 0)
        {
            stk->error = NEGATIVE_SIZE;
            return NEGATIVE_SIZE;
        }

        if (stk->capacity < 0)
        {
            stk->error = NEGATIVE_CAPACITY;
            return NEGATIVE_CAPACITY;
        }

        if (stk->data == nullptr)
        {
            stk->error = NULL_POINTER_TO_ARRAY;
            return NULL_POINTER_TO_ARRAY;
        }

        if (stk->data == (element_t*)stk)
        {
            stk->error = ARRAY_AND_STRUCTURE_POINTERS_MATCHED;
            return ARRAY_AND_STRUCTURE_POINTERS_MATCHED;
        }


        return 0;
    }
}

void Stack_dump(FILE* file, struct Stack* stk)
{

}

void Poison_filling(struct Stack* stk, size_t start, size_t end)
{
    for (size_t i = start; i < end; ++i)
    {
        stk->data[i] = Poison;
    }
}

void Stack_reverse_reallocation_memory(struct Stack* stk)
{
    NULL_check(stk);

    if (stk->size < ((stk->capacity) / 2))
    {
       ; //stk->data = realloc()
    }
}