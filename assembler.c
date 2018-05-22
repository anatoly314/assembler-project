#include "assembler.h"

/* We check if str is register*/
boolean isRegister(char* str){
    int i;
    for (i=0; i < (sizeof(registers)/sizeof(registers[0])); i++) {
        if(strcmp(str, registers[i].name) == 0){
            return TRUE;
        }
    }
    return FALSE;
}

/* test if a string inside an array of directives*/
boolean testIsDirective(const char *str){
    
    int i;
    for (i=0; i < (sizeof(directives)/sizeof(directives[0])); i++) {
        if(strcmp(str, directives[i].name) == 0){
            return TRUE;
        }
    }
    return FALSE;
}

/* test if a string inside an array of commands*/
boolean testIsCommand(const char *str){
    int i;
    for (i=0; i < (sizeof(commands)/sizeof(commands[0])); i++) {
        if(strcmp(str, commands[i].name) == 0){
            return TRUE;
        }
    }
    return FALSE;
}


/* we get command decimal value*/
int getCommandDecimalValue(const char *str){
    int i;
    for (i=0; i < (sizeof(commands)/sizeof(commands[0])); i++) {
        if(strcmp(str, commands[i].name) == 0){
            return commands[i].decimalCode;
        }
    }
    return -1;
}

/* This function returns how many operands required by each command*/
int getCommandOperandsNumber(const char *str){
    int i;
    for (i=0; i < (sizeof(commands)/sizeof(commands[0])); i++) {
        if(strcmp(str, commands[i].name) == 0){
            return commands[i].operatorsNum;
        }
    }
    return -1;
}

/* We calculate how many words in RAM required*/
int getCommandRamWordsNumber(ParsedLine* line){
    
    if (line -> op_src && line -> op_dest && isRegister(line -> op_src) && isRegister(line -> op_dest)) {
        return 1;
    }
    
    int i;
    for (i=0; i < (sizeof(commands)/sizeof(commands[0])); i++) {
        if(strcmp(line -> command, commands[i].name) == 0){
            return commands[i].operatorsNum;
        }
    }
    return -1;
}

/* We get register's value, if wrong register it return -1*/
int getRegisterNumber(char* str){
    int i;
    for (i=0; i < (sizeof(registers)/sizeof(registers[0])); i++) {
        if(strcmp(str, registers[i].name) == 0){
            return registers[i].number;
        }
    }
    return -1;
}

/* we pass command name, Addressing mode and if isDestination is false we check sourceAddressingMode and vice versa*/
boolean isValidAddressingMode(const char* cmd, ADDRESSING_MODE mode, boolean isDestination){
    
    int srcAddrMode = 0;
    int destAddrMode = 0;
    boolean result = FALSE;
    
    int i;
    for (i=0; i < (sizeof(commands)/sizeof(commands[0])); i++) {
        if(strcmp(cmd, commands[i].name) == 0){
            srcAddrMode = commands[i].srcAddrMode;
            destAddrMode = commands[i].destAddrMode;
        }
    }
    
    if (isDestination) {/*Destination operand*/
        result = destAddrMode & mode;
    }else{              /*Source operand*/
        result = srcAddrMode & mode;
    }
    
    return result;
    
}
