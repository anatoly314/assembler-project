/*
 In  assembler.h and assembler.c file I put together all default parameters related to assembler,
 such as commands, registers, permitted values
 (c) Anatoly Tarnavsky 2015

 */

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include "data_structures.h"

typedef struct{     /*struct which represents register*/
    char name[MAX_LINE_SIZE];
    int number;
}Register;

static const Register registers[] = {   /*array with available registers, its name and decimal value*/
    {"r0",  0},
    {"r1",  1},
    {"r2",  2},
    {"r3",  3},
    {"r4",  4},
    {"r5",  5},
    {"r6",  6},
    {"r7",  7}
};

typedef struct {        /*struct to contain various fields of our command*/
    char *name;         /*command name*/
    int decimalCode;    /*decimal code*/
    int operatorsNum;   /*how many operators it should receive*/
    int srcAddrMode;    /*permitted source addressing mode*/
    int destAddrMode;   /*permitted destination addressing mode*/
} Command;


static const Command commands[] = {         /*array with predefined command and permitted value (parameters)*/
    {"mov",     0,       2, IMMEDIATE|DIRECT|DISPLACEMENT|REGISTER, DIRECT|REGISTER},
    {"cmp",     1,       2, IMMEDIATE|DIRECT|DISPLACEMENT|REGISTER, IMMEDIATE|DIRECT|DISPLACEMENT|REGISTER},
    {"add",     2,       2, IMMEDIATE|DIRECT|DISPLACEMENT|REGISTER, DIRECT|REGISTER},
    {"sub",     3,       2, IMMEDIATE|DIRECT|DISPLACEMENT|REGISTER, DIRECT|REGISTER},
    {"not",     4,       1, NONE,                                   DIRECT|REGISTER},
    {"clr",     5,       1, NONE,                                   DIRECT|REGISTER},
    {"lea",     6,       2, DIRECT,                                 DIRECT|REGISTER},
    {"inc",     7,       1, NONE,                                   DIRECT|REGISTER},
    {"dec",     8,       1, NONE,                                   DIRECT|REGISTER},
    {"jmp",     9,       1, NONE,                                   DIRECT|DISPLACEMENT|REGISTER},
    {"bne",     10,      1, NONE,                                   DIRECT|DISPLACEMENT|REGISTER},
    {"red",     11,      1, NONE,                                   DIRECT|DISPLACEMENT|REGISTER},
    {"prn",     12,      1, NONE,                                   IMMEDIATE|DIRECT|DISPLACEMENT|REGISTER},
    {"jsr",     13,      1, NONE,                                   DIRECT},
    {"rts",     14,      0, NONE,                                   NONE},
    {"stop",    15,      0, NONE,                                   NONE}
};

typedef struct {        /*struct which represents directive*/
    char *name;
} Directive;

static const Directive directives[] = { /*array with permitted directives*/
    {".data"},
    {".string"},
    {".entry"},
    {".extern"}
};

/* We check if str is register*/
boolean isRegister(char* str);

/* test if a string inside an array of directives*/
boolean testIsDirective(const char *str);

/* test if a string inside an array of commands*/
boolean testIsCommand(const char *str);

/* we get command decimal value*/
int getCommandDecimalValue(const char *str);

/* We calculate how many words in RAM required*/
int getCommandRamWordsNumber(ParsedLine* line);

/* This function returns how many operands required by each command*/
int getCommandOperandsNumber(const char *str);

/* We get register's value, if wrong register it return -1*/
int getRegisterNumber(char* str);

/* we pass command name, Addressing mode and if isDestination is false we check sourceAddressingMode and vice versa*/
boolean isValidAddressingMode(const char* cmd, ADDRESSING_MODE mode, boolean isDestination);


#endif /* defined(ASSEMBLER_H) */
