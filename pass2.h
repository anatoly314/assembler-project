/*
 All related variables and methods for second pass
 (c) Anatoly Tarnavsky 2015

 */

#ifndef PASS_2_H
#define PASS_2_H

#include <stdio.h>
#include "constants.h"
#include "data_structures.h"
#include "parse_helpers.h"
#include "assembler.h"
#include <math.h>

/* Function responsible for second pass, parse code and replace labels with values*/
boolean pass2(ParsedLine* line);

/* to use bitwise comparision we should store values 1,2,4,8,etc... where next number is previous number + shift left
 if we need get real value like 0,1,2,3... we should take log2 of stored value
 */
int getValueOfAddrMode(int mode);


/* Parse code line with all operands, define source and destination addressing mode and evaluate labels
 It defines command memory word, source and destination memory words and write them to RAM
 */
boolean parseCodeLine(ParsedLine* parsedLine);


/* This function takes operands and evaluates them, defining addressing mode, memory allocation and code it to
 source memory mord and destination memory word*/
boolean parseOperands2Words(int* srcAddrMode, int* destAddrMode, int* opWord1, int* opWord2, ParsedLine* line);

/* When operand not register we use this function to evaluate it's value, addressing mode, etc..*/
boolean parseNonRegisterSingleOperand2Word(char* op, int* word, int* addrMode, int DC_OFFSET, ParsedLine* line);


/* function to perform bitwise operations.
 destination - number to be modified and returned,
 value - number to be inserted,
 offset - start position of modified sector, from right to left*/
int mergeBits(int value, int destination, int offset);
#endif /* defined(PASS_2_H) */
