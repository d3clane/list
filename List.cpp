#include <assert.h>

#include "Log.h"
#include "List.h"
#include "string.h"

static const size_t MinCapacity    = 16;
static const int POISON            = 0xDEAD;

static inline void ListDataCtor   (ListType* list);
static inline void ListDataInit   (ListElemType* list, 
                                   const size_t leftBorder, const size_t rightBorder,
                                   const size_t listCapacity);
static inline void DeleteFreeBlock(ListType* list);
static inline void AddFreeBlock   (ListType* list, const size_t newPos);

static inline ListErrors ListCapacityIncrease(ListType* list);

//-------Graphic dump funcs---------

static inline void CreateImgInLogFile(const size_t imgIndex);
static inline void BeginDotFile(FILE* outDotFile);
static inline void EndDotFile  (FILE* outDotFile);

static inline void CreateMainNodeDotFile      (FILE* outDotFile, const ListType* list, 
                                                                 const size_t nodeId);
static        void CreateMainNodesDotFile     (FILE* outDotFile, const ListType* list);
static        void CreateMainEdgesDotFile     (FILE* outDotFile, const ListType* list);

static inline void CreateAuxiliaryInfoDotFile (FILE* outDotFile, const ListType* list);
static        void CreateFictiousEdgesDotFile (FILE* outDotFile, const ListType* list);

static inline ListErrors GetPosForNewVal(ListType* list, size_t* pos);

#define LIST_CHECK(list)                    \
do                                          \
{                                           \
    ListErrors listErr = ListVerify(list);  \
                                            \
    if (listErr != ListErrors::NO_ERR)      \
    {                                       \
        LIST_TEXT_DUMP(list);               \
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
    
    list->size = 0;

    list->data = (ListElemType*) calloc(capacity, sizeof(*list->data));

    if (list->data == nullptr)
        return ListErrors::MEMORY_ERR;

    list->capacity = capacity;

    ListDataCtor(list);

    list->end            = 0;
    list->freeBlockHead  = 1;

    LIST_CHECK(list);

    return ListErrors::NO_ERR;
}

ListErrors ListDtor(ListType* list)
{
    assert(list);

    list->end = list->freeBlockHead = 0;
    list->capacity = list->size = 0;

    if (list->data != nullptr)
        free(list->data);

    list->data = nullptr;
    return ListErrors::NO_ERR;
}

ListErrors ListCopy(const ListType* source, ListType* target)
{
    assert(source);
    assert(target);

    target->capacity      = source->capacity;
    target->freeBlockHead = source->freeBlockHead;
    target->data          = source->data;
    target->size          = source->size;
    
    return ListErrors::NO_ERR;
}

#define LOG_ERR(error)            \
do                                \
{                                 \
    LIST_ERRORS_LOG_ERROR(error); \
    LIST_TEXT_DUMP(list);         \
    return error;                 \
} while (0)

ListErrors ListVerify(ListType* list)
{
    assert(list);

    if (list->data == nullptr)
        LOG_ERR(ListErrors::DATA_IS_NULLPTR);
    
    if (list->capacity < list->size)
        LOG_ERR(ListErrors::OUT_OF_RANGE);

    if (list->data[0].value != POISON)
        LOG_ERR(ListErrors::INVALID_NULLPTR);

    size_t freeBlockIndex = list->freeBlockHead;
    if (freeBlockIndex == 0)
        return ListErrors::NO_ERR;

    if (list->data[list->freeBlockHead].prevPos != 0)
        LOG_ERR(ListErrors::INVALID_DATA);

    while (list->data[freeBlockIndex].nextPos != 0)
    {
        if (list->data[freeBlockIndex].value != POISON)
            LOG_ERR(ListErrors::INVALID_DATA);

        if (list->data[freeBlockIndex].nextPos > list->capacity)
            LOG_ERR(ListErrors::OUT_OF_RANGE);

        if (list->data[freeBlockIndex].prevPos > list->capacity)
            LOG_ERR(ListErrors::OUT_OF_RANGE);
        
        freeBlockIndex = list->data[freeBlockIndex].nextPos;
    }

    return ListErrors::NO_ERR;
}

void ListDump(const ListType* list, const char* fileName,
                                    const char* funcName,
                                    const int line)
{
    assert(list);
    assert(fileName);
    assert(funcName);
    
    ListTextDump(list, fileName, funcName, line);

    ListGraphicDump(list);
}

void ListTextDump(const ListType* list, const char* fileName,
                                        const char* funcName,
                                        const int   line)
{
    assert(list);
    assert(fileName);
    assert(funcName);
    
    LogBegin(fileName, funcName, line);

    static const size_t numberOfElementsToPrint = 16;

    Log("List head: %zu, list tail: %zu\n", ListGetHead(list), ListGetTail(list));
    Log("Free blocks head: %zu\n", list->freeBlockHead);

    Log("List capacity: %zu\n", list->capacity);
    Log("List size    : %zu\n", list->size);

    //-----Print all data----

    Log("Data[%p]:\n", list->data);

    for (size_t i = 0; i < numberOfElementsToPrint && i < list->capacity; ++i)
    {
        Log("\tElement id: %zu, value: %d, previous position: %zu, next position: %zu\n",
            i, list->data[i].value, list->data[i].prevPos, list->data[i].nextPos);
    }

    Log("\t...\n");

    //-----Print list-------

    Log("List:\n");

    size_t listTail = ListGetTail(list);
    for (size_t i = ListGetHead(list); i != listTail; i = list->data[i].nextPos)
    {
        Log("\tElement id: %zu, value: %d, previous position: %zu, next position: %zu\n",
            i, list->data[i].value, list->data[i].prevPos, list->data[i].nextPos);
    }

    Log("\tLast element: %zu, value: %d, previous position: %zu, next position: %zu\n",
         listTail, list->data[listTail].value, 
         list->data[listTail].prevPos, list->data[listTail].nextPos);
    
    LOG_END();
}

static inline void CreateImgInLogFile(const size_t imgIndex)
{
    static const size_t maxImgNameLength  = 64;
    static char imgName[maxImgNameLength] = "";
    snprintf(imgName, maxImgNameLength, "imgs/img_%zu_time_%s.png", imgIndex, __TIME__);

    static const size_t     maxCommandLength  = 128;
    static char commandName[maxCommandLength] = "";
    snprintf(commandName, maxCommandLength, "dot list.dot -T png -o %s", imgName);

    system(commandName);

    snprintf(commandName, maxCommandLength, "<img src = \"%s\">", imgName);    
    Log(commandName);
}

static inline void BeginDotFile(FILE* outDotFile)
{
    fprintf(outDotFile, "digraph G{\nrankdir=LR;\ngraph [bgcolor=\"#31353b\"];\n");
}

static inline void EndDotFile(FILE* outDotFile)
{
    fprintf(outDotFile, "\n}\n");
}

static inline void CreateMainNodeDotFile(FILE* outDotFile, const ListType* list, const size_t nodeId)
{
    fprintf(outDotFile, "node%zu"
                        "[shape=Mrecord, style=filled, fillcolor=\"#7293ba\","
                        "label  =\"id: %zu   |"
                                "value: %d   |" 
                            "<f0> next: %zu  |"
                            "<f1> prev: %zu\","
                            "color = \"#008080\"];\n",
                        nodeId, nodeId, 
                        list->data[nodeId].value, 
                        list->data[nodeId].nextPos,
                        list->data[nodeId].prevPos);    
}

static void CreateMainNodesDotFile(FILE* outDotFile, const ListType* list)
{
    for (size_t i = 0; i < list->capacity; ++i)
        CreateMainNodeDotFile(outDotFile, list, i);  
}

static inline void CreateAuxiliaryInfoDotFile(FILE* outDotFile, const ListType* list)
{
    fprintf(outDotFile, "node[shape = octagon, style = \"filled\", fillcolor = \"lightgray\"];\n");
    fprintf(outDotFile, "edge[color = \"darkgreen\"];\n");

    fprintf(outDotFile, "head->node%zu;\n", ListGetHead(list));
    fprintf(outDotFile, "tail->node%zu;\n", ListGetTail(list));
    fprintf(outDotFile, "end->node%zu;\n", 0lu);
    fprintf(outDotFile, "\"free block\"->node%zu;\n", list->freeBlockHead);
    fprintf(outDotFile, "nodeInfo[shape = Mrecord, style = filled, fillcolor=\"#19b2e6\","
                        "label=\"capacity: %zu | size : %zu\"];\n",
                        list->capacity, list->size);   
}

static void CreateFictiousEdgesDotFile(FILE* outDotFile, const ListType* list)
{
    assert(outDotFile);
    assert(list);

    fprintf(outDotFile, "node0");
    for (size_t i = 1; i < list->capacity; ++i)
        fprintf(outDotFile, "->node%zu", i);
    fprintf(outDotFile, "[color=\"#31353b\", weight = 1, fontcolor=\"blue\",fontsize=78];\n");
}

static void CreateMainEdgesDotFile(FILE* outDotFile, const ListType* list)
{
    assert(outDotFile);
    assert(list);

    fprintf(outDotFile, "edge[color=\"red\", fontsize=12, constraint=false];\n");

    for (size_t i = 0; i < list->capacity; ++i)
        fprintf(outDotFile, "node%zu->node%zu;\n", i, list->data[i].nextPos);
}

void ListGraphicDump(const ListType* list)
{
    assert(list);

    static const char* dotFileName = "list.dot";
    FILE* outDotFile = fopen(dotFileName, "w");

    if (outDotFile == nullptr)
        return;

    BeginDotFile(outDotFile);

    CreateMainNodesDotFile     (outDotFile, list);
    CreateFictiousEdgesDotFile (outDotFile, list);
    CreateMainEdgesDotFile     (outDotFile, list);
    CreateAuxiliaryInfoDotFile (outDotFile, list);

    EndDotFile(outDotFile);

    fclose(outDotFile);

    static size_t imgIndex = 0;
    CreateImgInLogFile(imgIndex);
    imgIndex++;
}

ListErrors ListInsert(ListType* list, const size_t anchorPos, const int value, 
                      size_t* insertedValPos)
{
    assert(list);
    assert(insertedValPos);
    assert(anchorPos < list->capacity);
    assert(anchorPos == list->end || list->data[anchorPos].value   != POISON);

    LIST_CHECK(list);
    
    size_t newValPos = 0;

    ListErrors error = ListErrors::NO_ERR;
    error            = GetPosForNewVal(list, &newValPos);

    if (error != ListErrors::NO_ERR)
        return error;

    *insertedValPos  = newValPos;

    ListElemInit(&list->data[newValPos], 
                  value, list->data[anchorPos].prevPos, anchorPos);

    const size_t prevAnchor = list->data[anchorPos].prevPos;

    list->data[prevAnchor].nextPos = newValPos;
    list->data[anchorPos].prevPos  = newValPos;

    list->size++;

    LIST_CHECK(list);

    return ListErrors::NO_ERR;
}

ListErrors ListErase (ListType* list, const size_t anchorPos)
{
    assert(list);
    assert(anchorPos < list->capacity);

    LIST_CHECK(list);

    list->data[list->data[anchorPos].prevPos].nextPos = list->data[anchorPos].nextPos;
    list->data[list->data[anchorPos].nextPos].prevPos = list->data[anchorPos].prevPos;

    AddFreeBlock(list, anchorPos);

    list->size--;

    LIST_CHECK(list);

    return ListErrors::NO_ERR;
}

static inline void ListDataInit(ListElemType* list, 
                                const size_t leftBorder, const size_t rightBorder,
                                const size_t listCapacity)
{
    assert(list);
    assert(leftBorder  <= rightBorder);
    assert(rightBorder <= listCapacity);
    assert(leftBorder != 0);

    for (size_t i = leftBorder; i < rightBorder; ++i)
        ListElemInit(&list[i], POISON, i - 1, i + 1);

    if (rightBorder == listCapacity)
        ListElemInit(&list[listCapacity - 1], POISON, listCapacity - 2, 0);  
}

static inline void ListDataCtor(ListType* list)
{
    assert(list);

    ListElemInit(&list->data[0], POISON, 0, 0);

    ListDataInit(list->data, 1, list->capacity, list->capacity);
}

void ListElemInit(ListElemType* elem, const int value, 
                                      const size_t prevPos, 
                                      const size_t nextPos)
{
    assert(elem);

    elem->value   = value;
    elem->prevPos = prevPos;
    elem->nextPos = nextPos;
}

void ListErrorsLogError(ListErrors error, const char* fileName,
                                          const char* funcName,
                                          const int   line)
{
    assert(fileName);
    assert(funcName);

    Log(HTML_RED_HEAD_BEGIN
        "Logging errors called from file: %s, func: %s, line: %d"
        HTML_HEAD_END, fileName, funcName, line);

    switch(error)
    {
        case ListErrors::DATA_IS_NULLPTR:
            Log("Data is nullptr\n");
            break;
        case ListErrors::INVALID_DATA:
            Log("Invalid data\n");
            break;
        case ListErrors::INVALID_NULLPTR:
            Log("Null adress in list is invalid\n");
            break;
        case ListErrors::MEMORY_ERR:
            Log("Memory error\n");
            break;
        case ListErrors::OUT_OF_RANGE:
            Log("List element is out of range\n");
            break;
        
        case ListErrors::NO_ERR:
        default:
            break;
    }
}                                          

static inline void DeleteFreeBlock(ListType* list)
{
    assert(list);

    if (list->freeBlockHead == 0)
        ListCapacityIncrease(list);

    assert(list->freeBlockHead != 0);

    list->data[list->freeBlockHead].value = POISON;
               list->freeBlockHead        = list->data[list->freeBlockHead].nextPos;

    if (list->freeBlockHead != 0)
        list->data[list->freeBlockHead].prevPos = 0;
}

static inline void AddFreeBlock(ListType* list, const size_t newPos)
{
    assert(list);
    assert(newPos < list->capacity);

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

static inline ListErrors ListCapacityIncrease(ListType* list)
{
    assert(list);
    
    if (list->freeBlockHead == 0)
        list->freeBlockHead = list->capacity;

    list->capacity *= 2;

    void* tmpPtr = realloc(list->data, list->capacity * sizeof(*list->data));

    if (tmpPtr == nullptr)
        return ListErrors::MEMORY_ERR;
    
    list->data = (ListElemType*)tmpPtr;
    
    ListDataInit(list->data, list->capacity / 2, list->capacity, list->capacity);

    return ListErrors::NO_ERR;
}

ListErrors ListRebuild(ListType* list)
{
    assert(list);

    ListType newList = {};
    ListCtor(&newList, list->capacity); 

    //-----rebuild used values-------
    size_t posInNewList = 1;

    size_t listTail = ListGetTail(list);
    for (size_t i = ListGetHead(list); i != listTail; i = list->data[i].nextPos)
    {
        ListElemInit(&newList.data[posInNewList], list->data[i].value, 
                                                  posInNewList - 1, posInNewList + 1);
        ++posInNewList;
    }
    ListElemInit(newList.data + posInNewList, list->data[listTail].value,
                                              posInNewList - 1, 0);
    ListElemInit(newList.data, POISON, posInNewList, 1);

    newList.end  = 0;
    newList.freeBlockHead = (posInNewList + 1) % list->capacity;
    newList.size = list->size;

    ListDtor(list);
    ListCopy(&newList, list);

    //NO newList Dtor because there is calloced memory used for list and nothing more dynamic
    return ListErrors::NO_ERR;
}

ListErrors ListCapacityDecrease(ListType* list)
{
    assert(list);

    ListRebuild(list);
    assert(ListGetTail(list) * 2 < list->capacity);

    list->capacity /= 2;

    void* tmpPtr = realloc(list->data, list->capacity * sizeof(*list->data));

    if (tmpPtr == nullptr)
        return ListErrors::MEMORY_ERR;
    
    list->data = (ListElemType*)tmpPtr;

    return ListErrors::NO_ERR;
}

size_t ListGetHead(const ListType* list)
{
    assert(list);

    return list->data[list->end].nextPos;
}

size_t ListGetTail(const ListType* list)
{
    assert(list);

    return list->data[list->end].prevPos;
}

static inline ListErrors GetPosForNewVal(ListType* list, size_t* pos)
{
    assert(list);
    assert(pos);

    ListErrors error = ListErrors::NO_ERR;
    if (list->freeBlockHead == 0)
        error = ListCapacityIncrease(list);

    if (error != ListErrors::NO_ERR)
        return error;
    
    *pos = list->freeBlockHead;
    DeleteFreeBlock(list);

    return ListErrors::NO_ERR;
}
