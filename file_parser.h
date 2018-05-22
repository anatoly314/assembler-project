/*
 All methods to pass line and tokenize it to commands, labels, operands and data
 (c) Anatoly Tarnavsky 2015

 */

#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include "data_structures.h"
#include "parse_helpers.h"
#include "pass1.h"
#include "pass2.h"

/* takes the line from a file and split it to the tokens, it designed to permit only maximum 3 tokens.
 For example if first line is label it may be 3 tokens, label, directive and data.
 If it will receive the following line LENGTH:	     .data	    6,    -9     ,          15   it will remove all unnecessary spaces
 and convert it to LENGTH:	.data	6,-9,15
 */
void parseLine(ParsedLine* parsedLine);

/* this function parse operands after commands*/
void parseOperands(char* operands, ParsedLine* line);

/* If we found that line contains directive .data or .string this function parse it*/
void parseDirectiveData(char* operands, ParsedLine* line);

/* we call this function with filename and boolean value firstPass = true it means first pass, if false it means second pass*/
boolean processPass(const char* fileName, boolean firstPass);



#endif /* defined(FILE_PARSER_H) */
