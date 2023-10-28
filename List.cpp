#include <assert.h>

#include "List.h"
static const size_t MinCapacity    = 64;
static const int POISON            = 0xDEAD;
static const size_t ExtraInfoShift = 1;

static inline void ListDataCtor            (ListType* list);
static inline void MoveFreeBlockHeadForward(ListType* list);
static inline void MoveFreeBlockHeadBack   (ListType* list, const size_t newPos);

#define LIST_CHECK(list)                    \
do                                          \
{                                           \
    ListErrors listErr = ListVerify(list);  \
                                            \
    if (listErr != ListErrors::NO_ERR)      \
    {                                       \
        LIST_DUMP(list);                    \
        LIST_ERRORS_LOG_ERROR(listErr);     \
        return listErr;                     \
    }                                       \
} while (0)

ListErrors ListCtor(ListType* list, const size_t listStandardCapacity)
{
    assert(list);

    size_t capacity = listStandardCapacity;
    if (capacity < MinCapacity)
        capacity = MinCapacity;
    
    list->capacity = capacity;
    list->size     = 0;

    list->data = (ListElemType*) calloc(capacity, sizeof(*list->data));

    if (list->data == nullptr)
    {
        return ListErrors::MEMORY_ERR;
    }

    ListDataCtor(list);

    list->head           = 0;
    list->tail           = 0;
    list->freeBlockHead  = 1;

    LIST_CHECK(list);

    return ListErrors::NO_ERR;
}

ListErrors ListDtor(ListType* list)
{
    assert(list);

    free(list->data);
    list->data = nullptr;
    list->head = list->tail = list->freeBlockHead = 0;

    list->capacity = list->size = 0;

    return ListErrors::NO_ERR;
}

#define LOG_ERR(error)            \
do                                \
{                                 \
    LIST_ERRORS_LOG_ERROR(error); \
    LIST_DUMP(list);              \
    return error;                 \
} while (0)

ListErrors ListVerify(ListType* list)
{
    assert(list);

    if (list->data == nullptr)
        LOG_ERR(ListErrors::DATA_ERR);
    
    if (list->capacity < list->size)
        LOG_ERR(ListErrors::OUT_OF_RANGE);

    if (list->data[0].value   != POISON || 
        list->data[0].prevPos != 0      || list->data[0].nextPos != 0)
    {
        //printf("val, prev, next: %d, %d, %d\n", list->data->value, list->data->prevPos, list->data->nextPos);

        LOG_ERR(ListErrors::INVALID_NULLPTR);
    }

    size_t freeBlockIndex = list->freeBlockHead;
    if (list->data[list->freeBlockHead].prevPos != 0)
        LOG_ERR(ListErrors::INVALID_DATA);

    while (list->data[freeBlockIndex].nextPos != 0)
    {
        if (list->data[freeBlockIndex].value != POISON)
            LOG_ERR(ListErrors::INVALID_DATA);
        
        /*if (list->data[freeBlockIndex].nextPos == freeBlockIndex)
            LOG_ERR(ListErrors::INVALID_DATA);
        
        if (list->data[freeBlockIndex].prevPos == freeBlockIndex)
            LOG_ERR(ListErrors::INVALID_DATA);*/

        if (list->data[freeBlockIndex].nextPos > list->capacity)
            LOG_ERR(ListErrors::OUT_OF_RANGE);

        if (list->data[freeBlockIndex].prevPos > list->capacity)
            LOG_ERR(ListErrors::OUT_OF_RANGE);
        
        freeBlockIndex = list->data[freeBlockIndex].nextPos;
    }

    return ListErrors::NO_ERR;
}

void ListDump  (ListType* list, const char* fileName,
                                const char* funcName,
                                const int   line)
{
    assert(list);
    assert(fileName);
    assert(funcName);
    
    printf("Line: %d\n", line);
    printf("free head: %d\n", list->freeBlockHead);
    printf("head: %d, tail: %d\n", list->head, list->tail);
    for (size_t i = 0; i < list->capacity; ++i)
    {
        printf("%lu: value: %d prevPos: %d, nextPos: %d\n", i, list->data[i].value, list->data[i].prevPos, list->data[i].nextPos);
    }
    /*for (size_t i = list->head; i != list->tail; i = list->data[list->head].nextPos)
    {
        printf("%lu: value: %d prevPos: %d, nextPos: %d\n", i, list->data[i].value, list->data[i].prevPos, list->data[i].nextPos);
    }*/
    //TODO: 
}

ListErrors ListInsert(ListType* list, const size_t anchorPos, const size_t value, 
                      size_t* insertedValPos)
{
    assert(list);
    assert(insertedValPos);
    assert(anchorPos == LIST_END || anchorPos < list->capacity);
    assert(anchorPos != 0);
    assert(anchorPos == LIST_END || list->data[anchorPos].value   != POISON);

    //printf("next free block: %d\n", list->data[list->freeBlockHead].nextPos);
    LIST_CHECK(list);
    //printf("next free block: %d\n", list->data[list->freeBlockHead].nextPos);
    //printf("anchor pos: %lu\n", anchorPos);
    
    //TODO: realloc
    if (list->freeBlockHead == 0)
        return ListErrors::MEMORY_ERR;
    
    //printf("next free block: %d\n", list->data[list->freeBlockHead].nextPos);
    const size_t newValPos = list->freeBlockHead;
    MoveFreeBlockHeadForward(list);
    *insertedValPos        = newValPos;

    //printf("new pos: %d\n", newValPos);
    //printf("newvalPos: %d\n", newValPos);
    //printf("zero next pos: %d\n", list->data->nextPos);

    if (anchorPos == LIST_END)
    {
        //printf("zero next pos: %d\n", list->data->nextPos);
        ListElemInit(&list->data[newValPos], value, list->tail, 0);

        if (list->tail != 0)
            list->data[list->tail].nextPos = newValPos;

        list->tail = newValPos;
    }
    else
    {
        ListElemInit(&list->data[newValPos], 
                     value, list->data[anchorPos].prevPos, anchorPos);
        //printf("zero next pos: %d\n", list->data->nextPos);

        const size_t prevAnchor = list->data[anchorPos].prevPos;
        if (prevAnchor != 0)
            list->data[prevAnchor].nextPos = newValPos;

        list->data[anchorPos].prevPos = newValPos;
    }

    if (list->data[newValPos].prevPos == 0)
    {
        //printf("zero next pos: %d\n", list->data->nextPos);
        //printf("HEREEE\n");

        list->head = newValPos;
    }

    LIST_CHECK(list);
    return ListErrors::NO_ERR;
}

ListErrors ListErase (ListType* list, const size_t anchorPos)
{
    assert(list);
    assert(anchorPos < list->capacity);

    LIST_CHECK(list);

    if (anchorPos == list->tail)
    {
        list->tail = list->data[anchorPos].prevPos;
        list->data[list->tail].nextPos = 0;
    }
    else if (anchorPos == list->head)
    {
        list->head = list->data[anchorPos].nextPos;
        list->data[list->head].prevPos = 0;
    }
    else
    {
        list->data[list->data[anchorPos].prevPos].nextPos = list->data[anchorPos].nextPos;
        list->data[list->data[anchorPos].nextPos].prevPos = list->data[anchorPos].prevPos;
    }

    MoveFreeBlockHeadBack(list, anchorPos);
    LIST_CHECK(list);

    return ListErrors::NO_ERR;
}

static inline void ListDataCtor(ListType* list)
{
    assert(list);

    ListElemInit(&list->data[0], POISON, 0, 0);

    for (size_t i = 1; i < list->capacity - 1; ++i)
        ListElemInit(&list->data[i], POISON, i - 1, i + 1);

    ListElemInit(&list->data[list->capacity - 1], POISON, list->capacity - 2, 0);
}

void ListElemInit(ListElemType* elem, const size_t value, 
                                      const size_t prevPos, 
                                      const size_t nextPos)
{
    assert(elem);

    elem->value     = value;
    elem->prevPos = prevPos;
    elem->nextPos = nextPos;
}

void ListErrorsLogError(ListErrors error, const char* fileName,
                                          const char* funcName,
                                          const int   line)
{
    //TODO: 
}                                          

static inline void MoveFreeBlockHeadForward(ListType* list)
{
    assert(list);

    //TODO: realloc
    assert(list->freeBlockHead != 0);

    list->data[list->freeBlockHead].value = POISON;
    list->freeBlockHead = list->data[list->freeBlockHead].nextPos;
    list->data[list->freeBlockHead].prevPos = 0;
}

static inline void MoveFreeBlockHeadBack(ListType* list, const size_t newPos)
{
    assert(list);
    assert(newPos < list->capacity);

    //TODO: realloc
    assert(list->freeBlockHead != 0);

    if (list->freeBlockHead == 0)
    {
        list->freeBlockHead = newPos;
        ListElemInit(&list->data[list->freeBlockHead], POISON, 0, 0);

        return;
    }

    //do not change order!
    list->data[list->freeBlockHead].prevPos = newPos;
    ListElemInit(&list->data[newPos], POISON, 0, list->freeBlockHead);
    list->freeBlockHead = newPos;
}
