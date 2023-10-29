#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "Colors.h"
#include "Errors.h"
#include "Log.h"

//---------------

#undef PRINT_ERR

#ifndef NDEBUG
    #define PRINT_ERR(X) Log(HTML_RED_HEAD_BEGIN "\n"                                \
                             X "Error occured in file %s in func %s in line %d\n"    \
                             HTML_HEAD_END "\n",                                     \
                             ErrorInfo.fileWithError, ErrorInfo.funcWithError, ErrorInfo.lineWithError)
#else
    #define PRINT_ERR(X) Log(HTML_RED_HEAD_BEGIN "\n" (X) "\n" HTML_HEAD_END "\n")
#endif

//---------------

static ErrorInfoType ErrorInfo = 
{
    .error = Errors::NO_ERR, 
    .fileWithError = "NO_ERRORS.txt", 
    .lineWithError = -1
};

#ifndef NDEBUG

    void UpdateError(Errors error, const char* const fileName, 
                                const char* const funcName, 
                                const int lineNumber)
    {
        ErrorInfo.fileWithError = fileName;
        ErrorInfo.lineWithError = lineNumber;
        ErrorInfo.funcWithError = funcName;
        ErrorInfo.error         = error;
    }

#else

    void UpdateError(Errors error)
    {
        ErrorInfo.error = error;
    }

#endif

void LogStardardErrors()
{
    LOG_BEGIN();

    switch(ErrorInfo.error)
    {
        case Errors::MEMORY_ALLOCATION_ERR:
            LOG_ERR("Memory allocation error.\n");
            break;

        case Errors::NO_ERR:
        default:
            break;
    }

    LOG_END();
}

/*void LogError(const char* format, const char* fileName, const char* funcName, const int lineNumber, ...)
{
    assert(format);
    assert(fileName);
    assert(funcName);
    assert(lineNumber);

    va_list args = {};

    va_start(args, format);

    Log()
    vfprintf()

    va_end(args);
}*/

bool HasError()
{
    switch(ErrorInfo.error)
    {
        case Errors::NO_ERR:
            return false;
        
        case Errors::MEMORY_ALLOCATION_ERR:
        default:
            return true;
    }
}

Errors GetError()
{
    return ErrorInfo.error;
}