#ifndef COLORS_H
#define COLORS_H

/// \file
/// \brief File contains console collors defines

#define REDTEXT      "\x1b[31;1m"
#define GREENTEXT    "\x1b[32;1m"
#define YELLOWTEXT   "\x1b[33;1m"
#define BLUETEXT     "\x1b[34;1m"
#define MAGENTATEXT  "\x1b[35;1m"
#define CYANTEXT     "\x1b[36;1m"
#define WHITETEXT    "\x1b[37;1m"
#define STDTEXT      "\x1b[30;0m"

#define REDCONSOLE      "\x1b[41;1m"
#define GREENCONSOLE    "\x1b[42;1m"
#define YELLOWCONSOLE   "\x1b[43;1m"
#define BLUECONSOLE     "\x1b[44;1m"
#define MAGENTACONSOLE  "\x1b[45;1m"
#define CYANCONSOLE     "\x1b[46;1m"
#define WHITECONSOLE    "\x1b[47;1m"
#define STDCONSOLE      "\x1b[40;0m"

#define RED_TEXT(X)        REDTEXT X STDTEXT
#define GREEN_TEXT(X)    GREENTEXT X STDTEXT
#define COLOR_TEXT(COLOR, X) COLOR X STDTEXT

#define SET_RED_CONSOLE          printf(REDCONSOLE)
#define SET_GREEN_CONSOLE        printf(GREENCONSOLE)
#define SET_COLOR_CONSOLE(COLOR) printf(COLOR)
#define SET_STD_CONSOLE          printf(STDCONSOLE)

#define PRINT_RED_TEXT(STRING_FORMATER, ...)          printf(REDTEXT STRING_FORMATER STDTEXT, __VA_ARGS__)
#define PRINT_GREEN_TEXT(STRING_FORMATER, ...)        printf(GREENTEXT STRING_FORMATER STDTEXT, __VA_ARGS__)
#define PRINT_COLOR_TEXT(COLOR, STRING_FORMATER, ...) printf(COLOR STRING_FORMATER STDTEXT, __VA_ARGS__)

#define HTML_RED_HEAD_BEGIN    "<h style=\"color:red\">"
#define HTML_GREEN_HEAD_BEGIN  "<h style=\"color:green\">"
#define HTML_CYAN_HEAD_BEGIN   "<h style=\"color:cyan\">"

#define HTML_HEAD_END          "</h>"

#endif // COLORS_H
