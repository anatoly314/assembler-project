/*
 All DataStructures and methods which are used throughout the assembler
 (c) Anatoly Tarnavsky 2015

 */

#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include "constants.h"
#include "errors.h"


typedef enum {  /*define data type, int or string*/
    INT,STRING
}DATA_TYPE;


typedef enum {  /*define label type: data, code, external or entry*/
    DATA_LABEL, CODE_LABEL, EXTERNAL_LABEL, ENTRY_LABEL
}LABEL_TYPE;



typedef struct{ /*DATASTRUCTURE TO STORE LABELS AND ITS ADDRESS*/
    char label[MAX_LABEL_SIZE];
    LABEL_TYPE labelType;
    int decimalValue;
}LabelAddressPair;


typedef struct{     /*DATASTRUCTURE TO STORE LABELS ADDRESS pairs*/
    int size;
    LabelAddressPair* labelAddressPairs;
}LabelsTable;



typedef struct {        /*DATASTRUCTURE which represents single data word in our RAM*/
    int intValue;
}DataRow;


typedef struct {        /*DATASTRUCTURE which represents region where we store all data from directives .string or .data*/
    int startAddress;   /*start address of region, it should be right after code region*/
    int size;
    DataRow* dataRows;
}DataImage;

typedef struct{         /*DATASTRUCTURE which represents parsed directive*/
    DATA_TYPE dataType; /*it may be INT or STRING*/
    int intLength;      /*it it's INT we need to store its length, otherwise we can check it by strlen*/
    union{              /*union, because it can be or int or chars*/
        int* intArray;
        char* charArray;
    };
}DirectiveData;

/*struct which represents a single word in a RAM*/
typedef struct {
    int address;
    int value;
}RamWord;

typedef struct {    /*struct which represents our RAM*/
    int size;       /*here we store total size*/
    int codeSize;   /*size of code words*/
    int dataSize;   /*size of data words*/
    RamWord ramWords[MAX_RAM_SIZE];
}RamMemory;

extern RamMemory* RAM;  /*Global variable RAM*/
extern LabelsTable* LABELS_TABLE;   /*Global Table of Labels*/
extern LabelsTable* EXTERNAL_LABELS_TABLE ; /*Global External labels table*/
extern DataImage* DATA_IMAGE;           /*Global Data Image where we store our data from directives */

extern boolean IS_ERROR;    /*Global variable which represents is there error in current file*/
extern int INSTRUCTION_COUNTER; /*Global IC*/
extern int DATA_COUNTER;        /*Global DC*/


typedef enum{           /*type of our ParsedLine, it can be or directive or command because we ignore empty or comment lines*/
    DIRECTIVE,
    COMMAND
}LINE_TYPE;


typedef struct{         /*datastracture to hold our parsed line*/
    int lineIndex;      /*line number in a file*/
    char rawText[MAX_LINE_SIZE];    /*raw, unparsed text*/
    LINE_TYPE type;     /*line type directive or command*/
    char label[MAX_LINE_SIZE];  /*if there is label it will be stored here*/
    char command[MAX_LINE_SIZE];    /*command*/
    int numOfOperands;              /*num of operands we've found*/
    char op_src[MAX_LINE_SIZE];     /*source operand*/
    char op_dest[MAX_LINE_SIZE];    /*destination operand*/
    boolean isDirData;              /*flag that it's DirectiveData*/
    DirectiveData* dirData;         /*DirectiveData struct*/
}ParsedLine;


typedef enum {  /*token positions*/
    FIRST = 1, SECOND = 2, THIRD = 4, FORTH = 8
}Position;


typedef enum {  /*token types*/
    TOKEN_LABEL = 0, TOKEN_COMMAND, TOKEN_DIRECTIVE
} TokenType;

typedef struct {    /*struct which define token type and permitted position*/
    TokenType type;
    Position position;
}TokenPositionsPair;

static const TokenPositionsPair tokenPositionsPairs[] = {   /*array with predefined token types and permitted position for each type*/
    {TOKEN_LABEL,       FIRST           },
    {TOKEN_COMMAND,     FIRST | SECOND  },
    {TOKEN_DIRECTIVE,   FIRST | SECOND  }
};

static const ParsedLine DEFAULT_PARSED_LINE = {         /*variable to initialize all parameters of a struct to 0 or NULL*/
    /*all values are set to 0, pointer to NULL*/
};

static const DirectiveData DEFAULT_DIRECTIVE_DATA = {    /*variable to initialize all parameters of a struct to 0 or NULL*/
    
};

static const DataRow DEFAULT_DATA_ROW = {            /*variable to initialize all parameters of a struct to 0 or NULL*/
    
};

/* helper function which checks if it's label*/
boolean testIsLabel(const char *str, int lineIndex);


/* Method to add Label and Label's type to LABELS_TABLE*/
boolean addLabelToLabelsTable(char* label, LABEL_TYPE labelType, int lineIndex);

/* we use this function to add Data to image, we send struct of type DirectiveData and it parses it, evaluates
 and adds DATA to DATA_IMAGE*/
void addData2DataImage(DirectiveData* directiveData);

/* we use this function to add label to table of external labels*/
boolean addLabelToExternalLabelsTable(char* label, int value);

/* This method add single word to RAM of Assembler*/
boolean addWordToRam(int address, int value);


/* We get label name and new label's type and perform the change*/
boolean changeLabelToType(char* label, LABEL_TYPE labelType);

/* We get label's value*/
int getLabelValue(char* label);

/* our assembler operates with 12 bits word, so we need to take first 12 bits from number only*/
int getFirst12Bit(int value);

/* We can't know absolute addresses of DATA in DATA_IMAGE prior to complete pass1
 therefor we add DATA with value of DC - DATA COUNTER and after we've completed pass1 we know
 how many memory use code part of our program, and then we use this function to add IC - INSTRUCTION COUNTER
 value, it will point to first empty absolute address to relative addresses of DATA values
 */
void updateDataImageAndDataLabels();


/* This function checks if this label exists and if it's of type external*/
boolean isExternalLabel(const char* label);

/* This method take all DATA from DATA_IMAGE which were added by directives .data and .string
 and places it in a RAM of assembler after code instructions*/
void addDataToRam();

/* We get label's value*/
boolean isLabelExists(char* label);


#endif /* defined(DATA_STRUCTURES_H) */


