#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#define DOUBLE_T

#ifdef DOUBLE_T
    const int code_t = 1;
    const double Poison = NAN;
    const char*  Poison_text = "NAN";
    typedef double element_t;
#endif

#ifdef INT_T
    const int code_t = 2;
    const int   Poison = 0xBADDED;
    const char* Poison_text = "0xBADDED";
    typedef int element_t;
#endif

#ifdef CHAR_T
    const int code_t = 3;
    const char  Poison = '\0';
    const char* Poison_text = "\\0";
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
#define case_of_switch(enum_const) case enum_const: return #enum_const;

typedef unsigned long long canary_t;
//===================================================

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
    WRONG_SIZE,
    NULL_POP,
    WRONG_CANARY_STRUCT_LEFT,
    WRONG_CANARY_STRUCT_RIGHT,
    WRONG_CANARY_ARRAY_LEFT,
    WRONG_CANARY_ARRAY_RIGHT,
    WRONG_HASH
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

const char* Text_ERROR(struct Stack* stk);

void Print_array(FILE*file, struct Stack* stk);

int Comparator_poison(element_t element);

void Placing_canary(struct Stack* stk, void* temp);

unsigned int HASHFAQ6(struct Stack* stk);

int Compare_hash(struct Stack* stk);
//===================================================

int main()
{
    struct Stack stk = {Canary, (size_t)(-1), (size_t)(-1), nullptr, 0, 0, 0, Canary}; 
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

    stk.data[2] = 50;

    printf("Pop: %lg\n", Stack_pop(&stk));
    printf("Pop: %lg\n", Stack_pop(&stk));
    printf("Pop: %lg\n", Stack_pop(&stk));
    printf("Pop: %lg\n", Stack_pop(&stk));
    printf("Pop: %lg\n", Stack_pop(&stk));



    Stack_destruct(&stk);
}

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
    }
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
                }

            }

            else
            {   
                stk->capacity *= 1.5;

                Placing_canary(stk, temp);

                Poison_filling(stk, stk->size + 1, stk->capacity);
            }
            
        }
        
        else
        {   
            stk->capacity *= 2;

            Placing_canary(stk, temp);

            Poison_filling(stk, stk->size + 1, stk->capacity);
        }
    }
}

void Stack_destruct(struct Stack* stk)
{
    NULL_check(stk);

    free(stk->data);

    stk->data = nullptr;
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

        if ((stk->size <= (stk->capacity - 1)) && (stk->size > 0))
        {
            if (!(Comparator_poison(stk->data[stk->size]) && !Comparator_poison(stk->data[stk->size - 1])))
            {
                stk->error = WRONG_SIZE;
                return WRONG_SIZE;
            }
        }

        else if (stk->size == 0)
            {
                if (!Comparator_poison(stk->data[stk->size]))
                {
                stk->error = WRONG_SIZE;
                return WRONG_SIZE;
                }
            }

        if (stk->canary_struct_left != Canary)
        {
            stk->error = WRONG_CANARY_STRUCT_LEFT;
            return WRONG_CANARY_STRUCT_LEFT;
        }

        if (stk->canary_struct_right != Canary)
        {
            stk->error = WRONG_CANARY_STRUCT_RIGHT;
            return WRONG_CANARY_STRUCT_RIGHT;
        }

        if (((canary_t*)(stk->data))[-1] != Canary)
        {
            stk->error = WRONG_CANARY_ARRAY_LEFT;
            return WRONG_CANARY_ARRAY_LEFT;
        }

        if (*((canary_t*)&((stk->data)[stk->capacity])) != Canary)
        {
            stk->error = WRONG_CANARY_ARRAY_RIGHT;
            return WRONG_CANARY_ARRAY_RIGHT;
        }

        return 0;
    }
}

void Stack_dump(FILE* file, struct Stack* stk)
{
    NULL_check(stk);

    const char* code = Text_ERROR(stk);

    fprintf(file, "Stack (ERROR #%d: %s [0x%x] \"stk\" \n", stk->error, code, stk);
    fprintf(file, "{\n");
    fprintf(file, "\tsize = %u\n",      stk->size);
    fprintf(file, "\tcapacity = %u\n",  stk->capacity);
    fprintf(file, "\tdata[0x%x]\n",       stk->data);
    fprintf(file, "\t{\n");

    Print_array(file, stk);

    fprintf(file, "\t}\n");
    fprintf(file, "}\n\n\n");

    fflush(file);
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

    if ((stk->capacity >= 4) && (stk->size < ((stk->capacity) / 4)))
    {
        stk->capacity /= 4;

        void* temp = realloc(&((canary_t*)stk->data)[-1], (stk->capacity + 1) * sizeof(element_t) + 2 * sizeof(canary_t));

        Placing_canary(stk, temp);
    }

    ASSERT_OK(stk)
}

const char* Text_ERROR(struct Stack* stk)
{
    switch (stk->error)
    {
    case_of_switch(SIZE_OUT_OF_CAPACITY)
    case_of_switch(OUT_OF_MEMORY)
    case_of_switch(NULL_ARRAY)
    case_of_switch(NEGATIVE_SIZE)
    case_of_switch(NEGATIVE_CAPACITY)
    case_of_switch(NULL_POINTER_TO_ARRAY)
    case_of_switch(ARRAY_AND_STRUCTURE_POINTERS_MATCHED)
    case_of_switch(REPEATED_CONSTRUCTION)
    case_of_switch(WRONG_SIZE)
    case_of_switch(NULL_POP)
    case_of_switch(WRONG_CANARY_STRUCT_LEFT)
    case_of_switch(WRONG_CANARY_STRUCT_RIGHT)
    case_of_switch(WRONG_CANARY_ARRAY_LEFT)
    case_of_switch(WRONG_CANARY_ARRAY_RIGHT)
    case_of_switch(WRONG_HASH)
    default: return "Unknown ERROR";
    }
}

void Print_array(FILE*file, struct Stack* stk)
{
    switch (code_t)
    {
    case 1:
        for (int i = 0; i <= stk->size; ++i)
        {
            if (isnan(stk->data[i])) 
                fprintf(file, "\t\t*[%d] = NAN (Poison!)\n", i);
            else
                fprintf(file, "\t\t*[%d] = %lg\n", i, stk->data[i]);
        }

        for (int i = stk->size + 1; i < stk->capacity; ++i)
        {
            if (isnan(stk->data[i])) 
                fprintf(file, "\t\t[%d] = NAN (Poison!)\n", i);
            else
                fprintf(file, "\t\t[%d] = %lg\n", i, stk->data[i]);
        }
        break;

    case 2:
        for (int i = 0; i <= stk->size; ++i)
        {
            if (stk->data[i] == 0xBADDED)
                fprintf(file, "\t\t*[%d] = 0xBADDED (Poison!)\n", i);
            else
                fprintf(file, "\t\t*[%d] = %ld\n", i, stk->data[i]);
        }

        for (int i = stk->size + 1; i < stk->capacity; ++i)
        {
            if (stk->data[i] == 0xBADDED) 
                fprintf(file, "\t\t[%d] = 0xBADDED (Poison!)\n", i);
            else
                fprintf(file, "\t\t[%d] = %ld\n", i, stk->data[i]);
        }
        break;
    
    case 3:
        for (int i = 0; i <= stk->size; ++i)
        {
            if (stk->data[i] == '\0')
                fprintf(file, "\t\t*[%d] = \\0 (Poison!)\n", i);
            else
                fprintf(file, "\t\t*[%d] = %c\n", i, stk->data[i]);
        }

        for (int i = stk->size + 1; i < stk->capacity; ++i)
        {
            if (stk->data[i] == '\0') 
                fprintf(file, "\t\t[%d] = \\0 (Poison!)\n", i);
            else
                fprintf(file, "\t\t[%d] = %c\n", i, stk->data[i]);
        }
        break;
    
    default:
        fprintf(file, "\t\t Unknow type element\n");
        break;
    }
}

int Comparator_poison(element_t element)
{
    switch(code_t)
    {
    case 1: return (isnan(element));
    case 2: return (element == 0xBADDED);
    case 3: return (element == '\0');
    }
}

void Placing_canary(struct Stack* stk, void* temp)
{
    NULL_check(stk);
    assert(temp != nullptr);

    canary_t* canary_array_left = (canary_t*) temp;
    *canary_array_left = Canary;

    stk->data = ((element_t*) &(canary_array_left[1]));
                    
    canary_t* canary_array_right = (canary_t*) &(stk->data[stk->capacity]);
    *canary_array_right = Canary;
}

unsigned int HASHFAQ6(struct Stack* stk)
{
    unsigned int hash = 0;

    for (int i = 0; i < stk->capacity; ++i)
    {
        hash += (unsigned char)(stk->data[i]);
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    for (int i = 0; i < (3 * sizeof(size_t) + sizeof(int) + sizeof(element_t*)); ++i)
    {
        hash += (unsigned char)(*((char*)stk + i));
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

int Compare_hash(struct Stack* stk)
{
    unsigned int temp_hash = stk->hash;
    stk->hash = 0;
    stk->hash = HASHFAQ6(stk);

    if (stk->hash != temp_hash)
    {
        stk->error = WRONG_HASH;
        return WRONG_HASH;
    }
}