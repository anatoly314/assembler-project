#include "data_structures.h"



int INSTRUCTION_COUNTER = 0;
int DATA_COUNTER = 0;


LabelsTable* LABELS_TABLE = NULL;
LabelsTable* EXTERNAL_LABELS_TABLE = NULL;
DataImage* DATA_IMAGE = NULL;
RamMemory* RAM = NULL;

/* helper function which checks if it's label*/
boolean testIsLabel(const char *str, int lineIndex){
    
    size_t length = strlen(str);
    boolean result = (str && *str && str[length - 1] == ':') ? TRUE : FALSE;
    
    
    if (result) {   /*if label not within 30 chars, if yes it will print error but won't compile but will keep parsing*/
        if (length - 1 > 30) {
            printError(NULL, INVALID_LABEL, lineIndex);
            return TRUE;
        }
        
        
        length-=2;  /*length pointing to char befor ':' in a label*/
        
        while (length) { /*in a label it should be only digits or letters, else it will print error but will keep parsing*/
            if(!isalpha(*(str + length)) && (*(str + length) > '9' || *(str + length) < '0' )){
                printError(NULL, INVALID_LABEL, lineIndex);
                return TRUE;
            };
            length--;
        }
    }
    
    return result;
}


/* our assembler operates with 12 bits word, so we need to take first 12 bits from number only*/
int getFirst12Bit(int value){
    unsigned mask = pow(2, 12) - 1;/*create mask*/
    value = value & mask;           /*apply mask*/
    return value;                   /*return value*/
}








/* This method add single word to RAM of Assembler*/
boolean addWordToRam(int address, int value){
    
    
    if (!RAM) {
        RAM = calloc(1, sizeof(RamMemory));
        RAM -> size = 0;
    }
    
    if (RAM -> size == MAX_RAM_SIZE) {
        printError(NULL, MAX_RAM_SIZE_EXCEEDED, 0);
        return FALSE;
    }
    
    (RAM -> ramWords + (RAM -> size)) -> address = address;
    (RAM -> ramWords + (RAM -> size)) -> value = value;
    RAM -> size ++;
    
    INSTRUCTION_COUNTER++;
    
    return TRUE;
}

/* This method take all DATA from DATA_IMAGE which were added by directives .data and .string 
 and places it in a RAM of assembler after code instructions*/
void addDataToRam(){
    int i = 0;
    while (i < DATA_IMAGE -> size && DATA_IMAGE) {
        int wordValue = getFirst12Bit((DATA_IMAGE -> dataRows + i) -> intValue);
        addWordToRam(INSTRUCTION_COUNTER, wordValue);
        i++;
    }
}


/* Method to add Label and Label's type to LABELS_TABLE*/
boolean addLabelToLabelsTable(char* label, LABEL_TYPE labelType, int lineIndex){
    if(isLabelExists(label)) {
        printError(NULL, LABEL_ALREADY_EXISTS, lineIndex);
        return FALSE;
    }
    
    if (!LABELS_TABLE) {
        LABELS_TABLE = calloc(1, sizeof(LabelsTable));
        LABELS_TABLE -> size = 0;
        LABELS_TABLE -> labelAddressPairs = calloc(1, sizeof(LabelAddressPair));
    }else{
        LABELS_TABLE -> labelAddressPairs = realloc(LABELS_TABLE -> labelAddressPairs,((LABELS_TABLE -> size) + 1) * sizeof(LabelAddressPair));
    }
    
    
    strcpy((LABELS_TABLE -> labelAddressPairs + (LABELS_TABLE -> size)) -> label, label);
    (LABELS_TABLE -> labelAddressPairs + (LABELS_TABLE -> size)) -> labelType = labelType;
    if (labelType == DATA_LABEL) {              /*if label of type Data*/
        (LABELS_TABLE -> labelAddressPairs + (LABELS_TABLE -> size)) -> decimalValue = DATA_COUNTER;     /*its value DC, DATA_COUNTER*/
    }else if(labelType == CODE_LABEL){          /*if label of type Code*/
        (LABELS_TABLE -> labelAddressPairs + (LABELS_TABLE -> size)) -> decimalValue = INSTRUCTION_COUNTER; /*its value IC, INSTRUCTION COUNTER*/
    }if (labelType == EXTERNAL_LABEL) {          /*if label of type External*/
        (LABELS_TABLE -> labelAddressPairs + (LABELS_TABLE -> size)) -> decimalValue = 0;   /*its value 0*/
    }
    
    LABELS_TABLE -> size ++;
    
    
    return TRUE;
}

/* we use this function to add label to table of external labels*/
boolean addLabelToExternalLabelsTable(char* label, int value){
    if (!EXTERNAL_LABELS_TABLE) {
        EXTERNAL_LABELS_TABLE = calloc(1, sizeof(LabelsTable));
        EXTERNAL_LABELS_TABLE -> size = 0;
        EXTERNAL_LABELS_TABLE -> labelAddressPairs = calloc(1, sizeof(LabelAddressPair));
    }else{
        EXTERNAL_LABELS_TABLE -> labelAddressPairs = realloc(EXTERNAL_LABELS_TABLE -> labelAddressPairs,((EXTERNAL_LABELS_TABLE -> size) + 1) * sizeof(LabelAddressPair));
    }
    
    
    strcpy((EXTERNAL_LABELS_TABLE -> labelAddressPairs + (EXTERNAL_LABELS_TABLE -> size)) -> label, label);
    (EXTERNAL_LABELS_TABLE -> labelAddressPairs + (EXTERNAL_LABELS_TABLE -> size)) -> decimalValue = value;
    
    EXTERNAL_LABELS_TABLE -> size ++;
    
    
    return TRUE;
}

/* we use this function to add Data to image, we send struct of type DirectiveData and it parses it, evaluates
 and adds DATA to DATA_IMAGE*/
void addData2DataImage(DirectiveData* directiveData){
    
    int i = 0;
    if (directiveData -> dataType == INT) { /*if DIRECTIVE DATA of type INT*/
        while (i < directiveData -> intLength) {
            if (!DATA_IMAGE) {
                DATA_IMAGE = calloc(1, sizeof(DataImage));
                DATA_IMAGE -> size = 0;
                DATA_IMAGE -> dataRows = calloc(1, sizeof(DataRow));
            }else{
                DATA_IMAGE -> dataRows = realloc(DATA_IMAGE -> dataRows,((DATA_IMAGE -> size) + 1) * sizeof(DataRow));
            }
            
            (DATA_IMAGE -> dataRows + (DATA_IMAGE -> size)) -> intValue = (directiveData -> intArray)[i];
            DATA_IMAGE -> size++;
            DATA_COUNTER++;
            i++;
        }
    }else if (directiveData -> dataType == STRING) {    /*if DIRECTIVE DATA of type STRING*/
        while (i < strlen(directiveData -> charArray) + 1) {
            if (!DATA_IMAGE) {
                DATA_IMAGE = calloc(1, sizeof(DataImage));
                DATA_IMAGE -> size = 0;
                DATA_IMAGE -> dataRows = calloc(1, sizeof(DataRow));
            }else{
                DATA_IMAGE -> dataRows = realloc(DATA_IMAGE -> dataRows,((DATA_IMAGE -> size) + 1) * sizeof(DataRow));
            }
            
            if (i < strlen(directiveData -> charArray)) {
                (DATA_IMAGE -> dataRows + (DATA_IMAGE -> size)) -> intValue = (directiveData -> charArray)[i];
            }else{ /*adding '\0' terminator to string*/
                (DATA_IMAGE -> dataRows + (DATA_IMAGE -> size)) -> intValue = '\0';
            }
            
            DATA_IMAGE -> size++;
            DATA_COUNTER++;
            i++;
        }
        
    }
}

/* We can't know absolute addresses of DATA in DATA_IMAGE prior to complete pass1
 therefor we add DATA with value of DC - DATA COUNTER and after we've completed pass1 we know
 how many memory use code part of our program, and then we use this function to add IC - INSTRUCTION COUNTER
 value, it will point to first empty absolute address to relative addresses of DATA values
 */
void updateDataImageAndDataLabels(){
    if (!DATA_IMAGE) {  /*there wasn't data in data image*/
        return;
    }
    
    DATA_IMAGE -> startAddress = INSTRUCTION_COUNTER;   /*IC points toward first empty cell and from this cell DI will be begin*/
    
    int i = 0;
    while (i < LABELS_TABLE -> size) {
        if ((LABELS_TABLE -> labelAddressPairs + i) -> labelType == DATA_LABEL) {
            (LABELS_TABLE -> labelAddressPairs + i) -> decimalValue += INSTRUCTION_COUNTER;
        }
        i++;
    }
    
}

/* We get label name and new label's type and perform the change*/
boolean changeLabelToType(char* label, LABEL_TYPE labelType){
    int i = 0;
    while (i < LABELS_TABLE -> size && label) {
        if (strcmp((LABELS_TABLE -> labelAddressPairs + i) -> label, label) == 0) {
            (LABELS_TABLE -> labelAddressPairs + i) -> labelType = labelType;
            return TRUE;
        }
        i++;
    }
    return FALSE;
}

/* We get label's value*/
int getLabelValue(char* label){
    int i = 0;
    while (i < LABELS_TABLE -> size && label) {
        if (strcmp((LABELS_TABLE -> labelAddressPairs + i) -> label, label) == 0) {
            return (LABELS_TABLE -> labelAddressPairs + i) -> decimalValue;
        }
        i++;
    }
    return -1;
}


/* We get label's value*/
boolean isLabelExists(char* label){
    int i = 0;
    while (LABELS_TABLE && i < LABELS_TABLE -> size && label) {
        if (strcmp((LABELS_TABLE -> labelAddressPairs + i) -> label, label) == 0) {
            return TRUE;
        }
        i++;
    }
    return FALSE;
}




/* This function checks if this label exists and if it's of type external*/
boolean isExternalLabel(const char* label){
    int i = 0;
    while (i < LABELS_TABLE -> size && label) {
        if (strcmp((LABELS_TABLE -> labelAddressPairs + i) -> label, label) == 0 && (LABELS_TABLE -> labelAddressPairs + i) -> labelType == EXTERNAL_LABEL) {
            return TRUE;
        }
        i++;
    }
    return FALSE;
}

