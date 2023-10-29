#ifndef LOG_H
#define LOG_H

#include <time.h>
#include <stdio.h>

#include "Colors.h"

/// @brief Opens log file with name argv0
/// @param [in]argv0 log file name (usually argv[0])
void LogOpen(const char* argv0);

/// @brief Begins new logging part
/// @param [in]fileName file from which logging is called
/// @param [in]funcName function from which logging is called
/// @param [in]line line from which logging is called 
void LogBegin(const char* fileName, const char* funcName, const int line);

/// @brief LogBegin with __FILE__, __func__, __LINE__
#define LOG_BEGIN() LogBegin(__FILE__, __func__, __LINE__)

/// @brief Prints string to log file
/// @param [in]format string format as in printf
/// @param [in]params as in printf
ssize_t Log(const char* format, ...);

/// @brief Ends logging part
/// @param [in]fileName file from which logging is called
/// @param [in]funcName function from which logging is called
/// @param [in]line line from which logging is called 
void LogEnd(const char* fileName, const char* funcName, const int line);

/// @brief LogEnd with __FILE__, __func__, __LINE__
#define LOG_END() LogEnd(__FILE__, __func__, __LINE__);

#endif
