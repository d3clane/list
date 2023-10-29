#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdint.h>

struct ListElemType
{
    int value;

    size_t prevPos;
    size_t nextPos;
};

struct ListType
{
    ListElemType* data;

    size_t head;
    size_t tail;
    size_t freeBlockHead;

    size_t size;
    size_t capacity;
};

enum class ListErrors
{
    NO_ERR,

    MEMORY_ERR,

    DATA_ERR,
    OUT_OF_RANGE,

    INVALID_NULLPTR,
    INVALID_DATA,

};

//for listInsert push back
#define LIST_END SIZE_MAX
void ListElemInit(ListElemType* elem, const int value, 
                                      const size_t prevPos, 
                                      const size_t nextPos);

ListErrors ListCtor  (ListType* list, const size_t capacity = 0);
ListErrors ListDtor  (ListType* list);
ListErrors ListVerify(ListType* list);
ListErrors ListInsert(ListType* list, const size_t anchorPos, const int value, 
                      size_t* insertedValPos);
ListErrors ListErase (ListType* list, const size_t anchorPos);
#define LIST_DUMP(list) ListDump((list), __FILE__, __func__, __LINE__)
void ListDump  (ListType* list, const char* fileName,
                                      const char* funcName,
                                      const int   line);

#define LIST_ERRORS_LOG_ERROR(error) ListErrorsLogError((error), __FILE__, __func__, __LINE__)
void ListErrorsLogError(ListErrors error, const char* fileName,
                                          const char* funcName,
                                          const int   line);
#endif