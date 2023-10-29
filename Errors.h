//
// Created by Arman on 21.08.2023.
//

/// \file
/// \brief Contains errors that may occur during the program working.
/// \details Contains errors info and function to print these errors.

#ifndef ERRORS_H
#define ERRORS_H

//#define NDEBUG

//-----------------------------------------------------------------------------------------------

/// \brief Errors than may occur during the program working. 
enum class Errors 
{
    NO_ERR,
    
    MEMORY_ALLOCATION_ERR,

    FILE_OPENING_ERR,
    GETTING_FILE_SIZE_ERR,
    PRINTING_TO_FILE_ERR
};

//-----------------------------------------------------------------------------------------------


/// \brief Contains info about errors - File with error, line with error, error code. 
/// \warning Have to be updated with UPDATE_ERR() only
struct ErrorInfoType 
{
    Errors error;              ///< error code

#ifndef NDEBUG
    const char* fileWithError; ///< __FILE__ (file name with error)
    const char* funcWithError; ///< __func__ (function name with error)
    int lineWithError;         ///< __LINE__ (line with error)
#endif
};

//-----------------------------------------------------------------------------------------------

#ifndef NDEBUG

    /// @brief function for updating error
    /// @param [in]error error to save
    /// @param [in]fileName file with error 
    /// @param [in]funcName function with error
    /// @param [in]lineNumber line with error
    void UpdateError(Errors error, const char* const fileName, 
                                   const char* const funcName, 
                                   const int lineNumber);

    /// \brief updates special struct with errors errorInfo 
    /// \details copyFileName copy of __FILE__ define at the moment macros is called 
    /// \details copyLineNumber __LINE__ define at the moment macros is valled
    /// \param [in]ERROR Errors enum with error occurred in program
    #define UPDATE_ERR(ERROR) UpdateError((ERROR), __FILE__, __func__, __LINE__)

    #define LOG_ERR(X) Log(HTML_RED_HEAD_BEGIN "\n" X "\n" HTML_HEAD_END "\n")

#else

    void UpdateError(Errors error);

    /// \brief updates only error code without debug info
    /// \param [in] ERROR Errors enum with error occurred in program
    #define UPDATE_ERR(ERROR) UpdateError(error);

#endif

#define HANDLE_ERR(ERROR)                                             \
do                                                                    \
{                                                                     \
    UPDATE_ERR(ERROR);                                                \
    LogStardardErrors();                                                     \
} while (0)


//----------------------------------------------------------------------------------------------

/// \brief print errors from special struct with errors errorInfo.
/// \attention errorInfo have to be updated only with UPDATE_ERR();
void LogStardardErrors();

//-----------------------------------------------------------------------------------------------

void LogError(const char* format, const char* fileName, const char* funcName, const int lineNumber, ...);

/// @brief checks if the error in ErrorInfo is fatal.
/// @returns true if error is fatal and have to leave the program, otherwise false
bool HasError();

//-----------------------------------------------------------------------------------------------

/// @brief returns ErrorInfo.error
/// @return returns ErrorInfo.error
Errors GetError();

//-----------------------------------------------------------------------------------------------

#endif // ERRORS_H