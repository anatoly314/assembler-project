/*
 Defining all constant which are used throughout assembler
 (c) Anatoly Tarnavsky 2015

 */

#ifndef GLOBALS_H
#define GLOBALS_H

#define TRUE 1
#define FALSE 0
typedef int boolean;                /*define boolean datatype*/

#define MAX_LABEL_SIZE 30
#define MAX_LINE_SIZE 80

#define MAX_RAM_SIZE 1000   /*Max word which can contain our memory are 1000 words*/
#define MEMORY_START_ADDRESS 100    /*Assembler begin to load program from this address*/


#define MAX_10BIT_NUMBER 1023       /*Two complement max and min values for 10bit and 12bit numbers*/
#define MIN_10BIT_NUMBER -512
#define MAX_12BIT_NUMBER 4095
#define MIN_12BIT_NUMBER -2048

#define BITS_IN_WORD 12             /*how many bits in our word*/


typedef enum{                       /*defining first bit of specific field in a command word */
    CMD_START = 0,
    CMD_SRC_ADDR_TYPE_START = 4,    /*first bit of source addressing mode type field*/
    CMD_DEST_ADDR_TYPE_START = 2,   /*first bit of destination addressing mode type field*/
    CMD_TYPE_START = 6,             /*first bit of command type field*/
    CMD_GRP_START = 10,             /*first bit of command group field*/
}CMD_START_BIT;

typedef enum {                      /*when we using REGISTER addressing mode here we define*/
    REG_START = 0,                  /*first bit for source and destination register*/
    REG_SRC_START = 7,              /*first bit of source register*/
    REG_DEST_START = 2,             /*first bit of destination register*/
}REG_START_BIT;

typedef enum {                      /*defining first bit of data word for value field and allocation memory type field*/
    WORD_START = 0,                 /*allocation memory field*/
    WORD_VALUE_START = 2,           /*value field*/
}WORD_START_BIT;

typedef enum{   /*defining types of memory allocation*/
    ABSOLUTE = 0, EXTERNAL = 1, RELOCATABLE = 2
}CODING_TYPE;/*A,R,E*/

    /*we use such values for using later bitwise & to make compare much simplier*/
    /*for example (DIRECT | DISPLACEMENT) & REGISTER <=> (2|4) & 8 <=> (10 | 100) & 1000  <=> 110 & 1000 = 0, not contain*/
    /*for example (DIRECT | DISPLACEMENT) & DISPLACEMENT <=> (2|4) & 4 <=> (10 | 100) & 100  <=> 110 & 100 = 100 > 0, contains*/
typedef enum {
    NONE=0, IMMEDIATE = 1,DIRECT = 2,DISPLACEMENT = 4,REGISTER = 8
}ADDRESSING_MODE;

#endif
