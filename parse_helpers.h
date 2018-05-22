/*
    All helper methods for parsers
 (c) Anatoly Tarnavsky 2015

 */

#ifndef PARSE_HELPERS_H
#define PARSE_HELPERS_H

#include <stdio.h>
#include "data_structures.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* empty line is comment line or line of spaces*/
boolean isEmpty(char *text);

/* helper function for strtok_a, it looks is there a symbol inside an array of delimeters*/
boolean isCharPresent(char symbol, char* delimeters);

/* helper function which checks if a valid number, valid numbers are: +5, -5, 5, -5*/
boolean isNumber(const char *str);


/* helper function which checks if token type matches token's position, for example label can be only at first position*/
boolean isTokenTypeMatchesPosition(TokenType type, Position position);

/* custom strtok_a, a - advanced which knows to take multiple delimeters, each delimeter should be single char*/
char* strtok_a(char *str, int delimNum, ...);

/*There isn't getline in ANSI C library so I've done it by myself*/
int getline_c(char** line, FILE* file);



#endif /* defined(PARSE_HELPERS_H) */


