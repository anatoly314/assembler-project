#include "pass2.h"

/* Function responsible for second pass, parse code and replace labels with values*/
boolean pass2(ParsedLine* line){
    
    if (strcmp(line-> command, ".data") == 0
        || strcmp(line-> command, ".string") == 0
        || strcmp(line-> command, ".extern") == 0) { /* if command is directive .data, .string or .extern we do nothing
                                                      because we have done all we've need in pass1*/
        return TRUE;
        
    }
    
    if (strcmp(line-> command, ".entry") == 0) { /*if label exists, step 5*/
        changeLabelToType(line-> op_src, ENTRY_LABEL);  /*we change it's type to entry*/
        
    }else{
        parseCodeLine(line);
    }
    
    
    return TRUE;
}


/* Parse code line with all operands, define source and destination addressing mode and evaluate labels
 It defines command memory word, source and destination memory words and write them to RAM
 */
boolean parseCodeLine(ParsedLine* line){
    
    int ramWordNumber = getCommandRamWordsNumber(line);/*how many words in RAM it uses*/
    
    int comWord = 0;        /*word code, command + addressing mode + A,E or R*/
    int comOperandsNumber = getCommandOperandsNumber(line -> command);/*bits 10-11 group of command*/
    int comCode = getCommandDecimalValue(line -> command); /*bits 6-9, command code */
    int comMemoryType = ABSOLUTE;   /*bits 0-1 command memory allocation type*/
    int opSrcAddrMode = 0;    /*bits 4-5 command operand source addressing mode*/
    int opDestAddrMode = 0;   /*bits 2-3 command operand destination addressing mode*/
    
    int opWordSrc = 0;      /*source operand*/
    int opWordDest = 0;      /*destination operand*/
    
    /*parsing operands and defining source and destination addressing mode*/
    parseOperands2Words(&opSrcAddrMode, &opDestAddrMode, &opWordSrc, &opWordDest, line);
    
    /*checking if source and/or destination addressing mode is/are correct*/
    int srcAddrModeCorrect = isValidAddressingMode(line -> command, opSrcAddrMode, FALSE);
    int destAddrModeCorrect = isValidAddressingMode(line -> command, opDestAddrMode, TRUE);
    
    /*if not it will print error*/
    if ((!srcAddrModeCorrect || !destAddrModeCorrect) && comOperandsNumber == 2) {
        printError(NULL, WRONG_ADDRESSING_MODE, line -> lineIndex);
    }else if(!destAddrModeCorrect && comOperandsNumber == 1){
        printError(NULL, WRONG_ADDRESSING_MODE, line -> lineIndex);
    }
    
    /*create command word according to source and destination addressing mode*/
    comWord  = mergeBits(comWord, comMemoryType, CMD_START); /*0-1 bits, A,E,R*/
    comWord = mergeBits(comWord, comCode, CMD_TYPE_START);/*6-9 bits cmd type*/
    comWord = mergeBits(comWord, getValueOfAddrMode(opSrcAddrMode), CMD_SRC_ADDR_TYPE_START);/*4-5 bits, source operand addressing type*/
    comWord = mergeBits(comWord, getValueOfAddrMode(opDestAddrMode), CMD_DEST_ADDR_TYPE_START);/*2-3 bits, destination operand addressing type*/
    comWord = mergeBits(comWord, comOperandsNumber, CMD_GRP_START);/*10-11 bits, group type, num of operands*/
    
    
    /*write to RAM according to number of words are being to be used*/
    switch (ramWordNumber) {
        case 0:
            addWordToRam(INSTRUCTION_COUNTER, comWord);
            break;
        case 1:
            addWordToRam(INSTRUCTION_COUNTER, comWord);
            addWordToRam(INSTRUCTION_COUNTER, opWordDest);
            break;
        case 2:
            addWordToRam(INSTRUCTION_COUNTER, comWord);
            addWordToRam(INSTRUCTION_COUNTER, opWordSrc);
            addWordToRam(INSTRUCTION_COUNTER, opWordDest);
            break;
        default:
            break;
    }
    
    return TRUE;
}


/* This function takes operands and evaluates them, defining addressing mode, memory allocation and code it to
 source memory mord and destination memory word*/
boolean parseOperands2Words(int* srcAddrMode, int* destAddrMode, int* opWordSrc, int* opWordDest, ParsedLine* line){
    int DC_OFFSET = 1; /*we can know right address of label only after we'd added it to table*/
    
    if (strlen(line -> op_src)!=0 && strlen(line -> op_dest)!=0) {      /*if there're two operands*/
        if (isRegister(line -> op_src) && isRegister(line -> op_dest)) {/*case when two operands coded in a single word, both are registers*/
            *opWordDest = mergeBits(*opWordDest, getRegisterNumber(line -> op_dest), REG_DEST_START);
            *opWordDest = mergeBits(*opWordDest, getRegisterNumber(line -> op_src), REG_SRC_START);
            *srcAddrMode = REGISTER;    /*both source and addressing modes are REGISTER addressing mode*/
            *destAddrMode = REGISTER;
            return TRUE;
        }else if(isRegister(line -> op_src)){  /*if only source operand is register*/
            *opWordSrc = mergeBits(*opWordSrc, getRegisterNumber(line -> op_src), REG_SRC_START);
            *srcAddrMode = REGISTER;    /*only source addressing mode are register*/
            parseNonRegisterSingleOperand2Word(line -> op_dest, opWordDest, destAddrMode, DC_OFFSET, line);
        }else if(isRegister(line -> op_dest)){/*if only destination operand is register*/
            *opWordDest = mergeBits(*opWordSrc, getRegisterNumber(line -> op_dest), REG_DEST_START);
            *destAddrMode = REGISTER;
            parseNonRegisterSingleOperand2Word(line -> op_src, opWordSrc, srcAddrMode, DC_OFFSET, line);  /*call to function which parse single operand which
                                                                                                           from other type than DIRECT addressing mode,
                                                                                                           not register*/
        }else{/*if not source neither destination operands not registers*/
            parseNonRegisterSingleOperand2Word(line -> op_src, opWordSrc, srcAddrMode, DC_OFFSET, line);
            DC_OFFSET++;                                                                    /*increasing DC_OFFSET to use it later when we
                                                                                             evaluate EXTERNAL LABELs*/
            parseNonRegisterSingleOperand2Word(line -> op_dest, opWordDest, destAddrMode, DC_OFFSET, line);
            DC_OFFSET++;
        }
    }else if(strlen(line -> op_dest)!=0){    /*if there's single operator*/
        if(isRegister(line -> op_dest)){
            *opWordDest = mergeBits(*opWordDest, getRegisterNumber(line -> op_dest), REG_DEST_START);
            *destAddrMode = REGISTER;
        }else{
            parseNonRegisterSingleOperand2Word(line -> op_dest, opWordDest, destAddrMode, DC_OFFSET, line);
        }
    }
    
    return TRUE;
}

/* When operand not register we use this function to evaluate it's value, addressing mode, etc..*/
boolean parseNonRegisterSingleOperand2Word(char* op, int* word, int* addrMode, int DC_OFFSET, ParsedLine* line){
    if (*op == '#') {/*if IMMEDIATE ADDRESSING MODE*/
        op++;        /*first char after #*/
        if (isNumber(op)) { /*check if number*/
            int value = atoi(op);
            if (value < MAX_10BIT_NUMBER && value > MIN_10BIT_NUMBER) { /*check if it's within bounds of two complement 10 bit addressing*/
                *word = mergeBits(*word, value, WORD_VALUE_START);      /*"inject" its value in the word*/
                *word = mergeBits(*word, ABSOLUTE, WORD_START);         /*add ABSOLUTE memory allocation to the word*/
                *addrMode = IMMEDIATE;                                  /*ADDRESSING MODE is IMMEDIATE*/
            }else{
                printError(NULL, NUMBER_OUT_OF_BOUNDS_10BITS, line -> lineIndex); /*if number out of bounds*/
                return TRUE;
            }
        }else{
            printError(NULL, NUMBER_OUT_OF_BOUNDS_10BITS, line -> lineIndex);     /*if not valid number*/
            return FALSE;
        }
        
    }else if(*op == '~'){                       /*if DISPOSAL addressing mode*/
        op+=2;                                  /*remove unnessary chars, '~','(' and ')'*/
        op[strlen(op)-1] = '\0';
        char* firstLabel = strtok_a(op, 1, ",");    /*get first label*/
        char* secondLabel = strtok_a(NULL, 1, ","); /*get second label*/
        
        if (!isExternalLabel(firstLabel) && !isExternalLabel(secondLabel)) {    /*we can't use external labels in this addressing mode*/
            if (isLabelExists(firstLabel) && isLabelExists(secondLabel)) {      /*check if labels exist and we can evaluate them*/
                int firstLabelValue = getLabelValue(firstLabel);                /*get label's value*/
                int secondLabelValue = getLabelValue(secondLabel);
                int val1 = abs(firstLabelValue - secondLabelValue);             /*calculate options*/
                int val2 = abs(INSTRUCTION_COUNTER - firstLabelValue);
                int val3 = abs(INSTRUCTION_COUNTER - secondLabelValue);
                
                int val4 = val1 > val2 ? val1 : val2;                           /*choosing the biggest option*/
                int distance = val4 > val3 ? val4 : val3;
                
                *word = mergeBits(*word, distance, WORD_VALUE_START);           /*assembling result word, value + addressing mode*/
                *word = mergeBits(*word, ABSOLUTE, WORD_START);
                *addrMode = DISPLACEMENT;                                       /*define ADDRESSING MODE*/
            }else{
                printError(NULL, LABEL_DOESNT_EXISTS, line -> lineIndex);
            }
            
        }else{
            printError(NULL, DISPLACEMENT_EXTERNAL_LABELS_ERROR, line -> lineIndex);
        }
        
        if (firstLabel) {   /*free allocated memory in strtok*/
            free(firstLabel);
        }
        if (secondLabel) {  /*free allocated memory in strtok*/
            free(secondLabel);
        }
        
    }else { /*if operand is single label*/
        
        int labelValue = getLabelValue(op); /*get its values*/
        
        if(labelValue == -1){               /*if label doesn't exist*/
            printError(NULL, LABEL_DOESNT_EXISTS, line -> lineIndex);
            return FALSE;
        }else if (isExternalLabel(op)) {    /*if it's external label we add it to table of external labels and here we use value DC_OFFSET*/
            addLabelToExternalLabelsTable(op, INSTRUCTION_COUNTER + DC_OFFSET);
            *word = mergeBits(*word, EXTERNAL, WORD_START);
        }else{
            *word = mergeBits(*word, RELOCATABLE, WORD_START);
        }
        
        *word = mergeBits(*word, labelValue, WORD_VALUE_START);
        *addrMode = DIRECT;
    }
    
    return TRUE;
}

/* function to perform bitwise operations.
 destination - number to be modified and returned,
 value - number to be inserted,
 offset - start position of modified sector, from right to left*/
int mergeBits(int destination, int value,  int offset){
    value = value << offset;
    destination = destination | value;
    return getFirst12Bit(destination);
};

/* to use bitwise comparision we should store values 1,2,4,8,etc... where next number is previous number + shift left
 if we need get real value like 0,1,2,3... we should take log2 of stored value
 */
int getValueOfAddrMode(int mode){
    return log( mode ) / log( 2 );
}
