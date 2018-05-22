#include "pass1.h"

/* Function responsible for first pass, take labels and calculates their values*/
boolean pass1(ParsedLine* line){
    
    
    boolean isFlag = FALSE;
    
    
    if (strlen(line -> label) > 0) {/*if label exists, step 3*/
        isFlag = TRUE;              /*we turn on the flag*/
    }
    
    if (strcmp(line -> command, ".data") == 0) {                /*if directive .data*/
        if (isFlag) {                                           /*if label exists, step 5*/
            addLabelToLabelsTable(line -> label, DATA_LABEL, line -> lineIndex);   /*we adding label to label table with symbol DATA*/
        }
        
        addData2DataImage(line -> dirData);                     /*we add data from DirectiveData of ParsedLine to DataImage*/
        return TRUE;
        
    }
    
    if (strcmp(line -> command, ".string") == 0) { /*if directive .string*/
        if (isFlag) {                               /*if label exists, step 5*/
            addLabelToLabelsTable(line -> label, DATA_LABEL, line -> lineIndex);
        }
        addData2DataImage(line -> dirData);
        return TRUE;
    }
    /*step 8*/
    
    if (strcmp(line -> command, ".extern") == 0 || strcmp(line -> command, ".entry") == 0) {   /*if directive .extern*/
        
        /*step 9*/
        if (strcmp(line -> command, ".extern") == 0) {
            addLabelToLabelsTable(line -> op_src, EXTERNAL_LABEL, line -> lineIndex);                  /*we add label to table with symbol external*/
            
        }
        
        
        
    }else{ /*step 11, if we reached here it means this's code line, not directive*/
        if (isFlag) {                                               /*if flag label is on, there's label at the beginning of line*/
            addLabelToLabelsTable(line -> label, CODE_LABEL, line -> lineIndex);       /*we add it to table labels with symbol CODE*/
        }
        
        
        INSTRUCTION_COUNTER ++; /*Increasing IC because we calculated command*/
        int tempIC = getCommandRamWordsNumber(line);    /*when both operands are of type direct addressing mode, we use only single
                                                         word to store both operands in RAM*/
        INSTRUCTION_COUNTER += tempIC; /*we increase IC according to RAM words which use both operands*/
        
    }
    
    return TRUE;
}
