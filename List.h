#ifndef LIST_H
#define LIST_H

#include <stddef.h>
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

    size_t end;
    size_t freeBlockHead;

    size_t size;
    size_t capacity;
};

enum class ListErrors
{
    NO_ERR,

    MEMORY_ERR,

    DATA_IS_NULLPTR,
    OUT_OF_RANGE,

    INVALID_NULLPTR,
    INVALID_DATA,

    TRYING_TO_GET_NULL_ELEMENT,
    TRYING_TO_CHANGE_NULL_ELEMENT,
};

ListErrors ListCtor  (ListType* list, const size_t capacity = 0);
ListErrors ListCopy  (const ListType* source, ListType* target);
ListErrors ListDtor  (ListType* list);
ListErrors ListVerify(ListType* list);
ListErrors ListInsert(ListType* list, const size_t anchorPos, const int value, 
                      size_t* insertedValPos);
ListErrors ListErase (ListType* list, const size_t anchorPos);

ListErrors ListCapacityDecrease(ListType* list);

ListErrors ListGetNextElem (ListType* list, size_t pos, size_t *nextElemPos);
ListErrors ListGetPrevElem (ListType* list, size_t pos, size_t *prevElemPos);
ListErrors ListGetElemValue(ListType* list, size_t pos, int* elemValue);
ListErrors ListSetElemValue(ListType* list, size_t pos, int  newElemValue);

size_t ListGetHead(const ListType* list);
size_t ListGetTail(const ListType* list);

#define LIST_TEXT_DUMP(list) ListTextDump((list), __FILE__, __func__, __LINE__)
void ListTextDump(const ListType* list, const char* fileName,
                                        const char* funcName,
                                        const int   line);
                                        
void ListGraphicDump(const ListType* list);

#define LIST_DUMP(list) ListDump((list), __FILE__, __func__, __LINE__)
void ListDump(const ListType* list, const char* fileName,
                                    const char* funcName,
                                    const int line);

#define LIST_ERRORS_LOG_ERROR(error) ListErrorsLogError((error), __FILE__, __func__, __LINE__)
void ListErrorsLogError(ListErrors error, const char* fileName,
                                          const char* funcName,
                                          const int   line);
#endif