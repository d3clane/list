#include "List.h"
#include "Log.h"

int main(const int argc, const char* argv[])
{
    LogOpen(argv[0]);

    ListType list;
    ListCtor(&list);

    size_t lastPos = 0;

    //LIST_DUMP(&list);
    //printf("HERE1\n");
    //printf("next free blockAA: %d\n", list.data[list.freeBlockHead].nextPos);

    ListInsert(&list, LIST_END, 78, &lastPos);

    //printf("lastPos: %d\n", lastPos);
    //printf("next free blockAB: %d\n", list.data[list.freeBlockHead].nextPos);
    size_t newLastPos = 0;
    ListInsert(&list, lastPos, 123, &newLastPos);
    
    //printf("lastPos: %d\n", lastPos);

    ListInsert(&list, lastPos, 1488, &newLastPos);

    //printf("lastPos: %d\n", lastPos);

    ListErase(&list, newLastPos);
    ListInsert(&list, lastPos, 1111, &lastPos);

    //printf("lastPos: %d\n", lastPos);

    ListInsert(&list, LIST_END, 9999, &newLastPos);

    //printf("lastPos: %d\n", lastPos);

    ListInsert(&list, lastPos, 123, &lastPos);

    //printf("lastPos: %d\n", lastPos);

    ListErase(&list, newLastPos);

    LIST_DUMP(&list);

    ListDtor(&list);
}