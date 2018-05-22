#include "file_parser.h"


/* we call this function with filename and boolean value firstPass = true it means first pass, if false it means second pass*/
    boolean processPass(const char* fileName, boolean firstPass){
    INSTRUCTION_COUNTER = MEMORY_START_ADDRESS;
    DATA_COUNTER = 0;
    boolean LINE_ERROR = FALSE; /*flag which means is there error in current line*/
    
    FILE *fp = NULL;
    
    char* asFileName = calloc(strlen(fileName) + strlen(".as") + 1, sizeof(char));
    strcat(asFileName, fileName);
    strcat(asFileName, ".as");
    fp = fopen(asFileName, "r");
    
    if (fp == NULL){
        printError(NULL, NO_SUCH_FILE, 0);
    }else{
        char * rawLine = NULL;
        int lineLength = 0;
        int lineCounter = 0;
        
        while ((lineLength = getline_c(&rawLine, fp)) != -1) {
            lineCounter++;
            ParsedLine* line = calloc(1, sizeof(ParsedLine));
            line -> lineIndex = lineCounter;
            
            if (lineLength > MAX_LINE_SIZE) {
                printError(NULL, LINE_TOO_LONG, lineCounter);
                free(line);
                continue;
            }
            
            if (rawLine[lineLength-1] == '\n') {
                rawLine[lineLength-1] = '\0';
            }
            
            
            if (isEmpty(rawLine)) {
                free(line);
                continue;
            }else{
                strcpy(line -> rawText, rawLine);
                parseLine(line);
                LINE_ERROR = IS_ERROR;
                if (!LINE_ERROR && firstPass) { /*if current line with error we don't need to process it*/
                    pass1(line);
                }else if(!LINE_ERROR){
                    pass2(line);
                }
            }
            
            if (line -> dirData) {
                if (line -> dirData -> dataType == INT) {
                    free(line -> dirData -> intArray);
                }else{
                    free(line -> dirData -> charArray);
                }
                free(line -> dirData);
            }
            free(line);
           
        }
        fclose(fp);
        free(rawLine);
    }
    
    free(asFileName);
    updateDataImageAndDataLabels();
    return TRUE;
}



/* it takes the raw line from a file and split it to the tokens, it designed to permit only maximum 3 tokens.
 For example if the first token is a label it may be 3 tokens, label, directive and data.
 If it will receive the following line LENGTH:	     .data	    6,    -9     ,          15   then remove all unnecessary spaces
 and convert it to LENGTH:	.data	6,-9,15
 */
void parseLine(ParsedLine* line){
    
    boolean labelExists = FALSE;
    boolean directiveExists = FALSE;
    boolean commandExists = FALSE;
    char* operands = NULL;
    char* token = strtok_a(line -> rawText, 3, " ","\t","\n");  /*strtok_a is function which can receive multiple delimeters*/
    
    Position tokenPosition = FIRST; /*We not only check if this is correct label or command but also is it in right place*/
    
    while (token) {
        size_t tokenSize = strlen(token);
        if(testIsLabel(token, line -> lineIndex) && !labelExists){/*if it's label*/
            if (!isTokenTypeMatchesPosition(TOKEN_LABEL, tokenPosition)) {/*and is it in right position*/
                printError(NULL, WRONG_LABEL_POSITION, line -> lineIndex);
                return;
            }
            token[tokenSize-1] = '\0';  /*we remove (:) at the end of string*/
            labelExists = TRUE;         /*flag that label exists*/
            strncpy(line -> label,token,tokenSize); /*and add label to our line*/
        }else if(testIsCommand(token) && !commandExists){       /*if it's command, and only one command*/
            if (!isTokenTypeMatchesPosition(TOKEN_COMMAND, tokenPosition)) {/*and is it in right position*/
                printError(NULL, WRONG_COMMAND_POSITION, line -> lineIndex);
                return;
            }
            line -> type = COMMAND;
            commandExists = TRUE;
            strcpy(line -> command,token);
        }else if(testIsDirective(token) && !directiveExists){       /*if it's directive*/
            if (!isTokenTypeMatchesPosition(TOKEN_DIRECTIVE, tokenPosition)) {
                printError(NULL, WRONG_DIRECTIVE_POSITION, line -> lineIndex);
                return;
            }
            line -> type = DIRECTIVE;
            directiveExists = TRUE;
            strcpy(line -> command,token);
            
        }else if(directiveExists || commandExists){/*operands can be only after command or directive*/
            if(!operands){
                operands = calloc(tokenSize + 1, sizeof(char));
            }else{
                operands = realloc(operands, (tokenSize + strlen(operands) + 1) * sizeof(char));
            }
            strcat(operands, token);    /*we trail spaces and concatenates operands in a single string without spaces*/
        }else{      /*else it's error*/
            printError(NULL, CANNOT_PARSE_LINE, line -> lineIndex);
            return;
        }
        
        
        free(token);/*token allocated in a strtok_a, so we need free it after we've used it*/
        token = strtok_a(NULL, 2);
        
        
        /*next position for token*/
        if (tokenPosition == FIRST) {
            tokenPosition = SECOND;
        }else if (tokenPosition == SECOND){
            tokenPosition = THIRD;
        }else if (tokenPosition == THIRD){
            tokenPosition = FORTH;
        }
    }
   
    
    
    
    
    if (directiveExists) {/*if directive exists we parse directive data*/
        parseDirectiveData(operands, line);
    }else {              /*else we parse operands*/
        parseOperands(operands, line);
        
        if(getCommandOperandsNumber(line -> command) != line -> numOfOperands){
            printError(NULL, WRONG_NUMBER_OF_OPERATORS, line -> lineIndex);
        }
        
        
    }
    free(operands);
    return;
    
}


/* this function parse operands after commands*/
void parseOperands(char* operands, ParsedLine* line){
    char* op_src = NULL;
    char* op_dest = NULL;
    char* delimeter = NULL;
    
    if (!operands) {    /*if there aren't operands*/
        line -> numOfOperands = 0;
        return;
    }
    
    size_t operandsLength = strlen(operands);
    
    
    /*if operands contains string ")," it may be operand of type  ~(K,END),W  and we will try to parse it*/
    if((delimeter = strstr(operands, "),"))){
        size_t offset = delimeter - operands + 1;   /*finding first operand*/
        size_t operand1Length = offset + 1;
        op_src = calloc(operand1Length + 1, sizeof(char));
        strncpy(op_src, operands, offset);          /*writing it to ParsedLine*/
        op_src[offset+1] = '\0';                    /*terminating by '\0' */
        offset++;
        size_t operand2Length = operandsLength - operand1Length;    /*finding second operand*/
        op_dest = calloc((operand2Length+1), sizeof(char));
        strncpy(op_dest, operands+offset, operand2Length);          /*writing it to ParsedLine*/
        op_dest[operand2Length] = '\0';                              /*terminating by '\0' */
        
    }else if((delimeter = strstr(operands, ","))){      /*if not previous case we try to parse case r1,r4*/
        op_src = strtok_a(operands, 2, ",","\n");
        op_dest = strtok_a(NULL, 2);
    }else if(operands){                                 /*if there isn't delimeters it should be a single operand*/
        op_dest = calloc(strlen(operands)+1,sizeof(char));/*writing it to ParsedLine*/
        strcpy(op_dest, operands);
    }
    
    if(op_src && op_dest){                              /*storing number of operands which we found, we'll use it later*/
        line -> numOfOperands = 2;
        strcpy(line -> op_src, op_src);
        strcpy(line -> op_dest, op_dest);
    }else if(op_dest){
        line -> numOfOperands = 1;
        strcpy(line -> op_dest, op_dest);
        
    }
    
    free(op_src);                                   /*freeing memory which we have been used*/
    free(op_dest);
}


/* If we found that line contains directive .data or .string this function parse it*/
void parseDirectiveData(char* operands, ParsedLine* line){
    
    line -> isDirData = TRUE;             /*turning on flag that line contains a directive*/
    if(strcmp(line -> command, ".data") == 0){  /*if data directive*/
        
        char* digitString = strtok_a(operands, 1, ",");     /*tokenizing input string */
        
        while (digitString) {
            
            if (!isNumber(digitString)) {                   /*each token should represent number*/
                printError(NULL, DATA_NOT_NUMBER, line -> lineIndex); /*else we call error function*/
                return;
            }
            
            int digit = atoi(digitString);                  /*convert to number*/
            if (digit > MAX_12BIT_NUMBER || digit < MIN_12BIT_NUMBER) { /*check if it's within permitted bounds*/
                printError(NULL, NUMBER_OUT_OF_BOUNDS_12BITS, line -> lineIndex);    /*if not error*/
                return;
            }
            
            if (!line -> dirData) {
                line -> dirData = calloc(1, sizeof(DirectiveData)); /*initializing*/
                *(line -> dirData) = DEFAULT_DIRECTIVE_DATA;  /*initializing DirectiveData value, else it may contain garbage*/
                line -> dirData -> dataType = INT;    /*DirectiveData of type INT*/
                line -> dirData -> intArray = calloc(1, sizeof(int)); /*Initializing Union, it may be INT or STRING(char*) */
                
                line -> dirData -> intLength = 1; /*setting initial size*/
            }else{
                int newSize = ++line -> dirData -> intLength;    /*increase size*/
                line -> dirData -> intArray = realloc(line -> dirData -> intArray, newSize * sizeof(int));   /*reallocation*/
            }
            
            
            line -> dirData -> intArray[line -> dirData -> intLength - 1] = digit; /*add digit to array*/
            
            if (digitString) {  /*free memory allocated by strtok_a*/
                free(digitString);
            }
            digitString = strtok_a(NULL, 1);    /*get next token*/
        }
        
        if (digitString) { /*free memory allocated by strtok_a*/
            free(digitString);
        }
        
    }else if(strcmp(line -> command, ".string") == 0 && operands){
        operands+=1;    /*removing the first char (quotes), - " */
        operands[strlen(operands) - 1] = '\0';  /*removing last char (quotes) - , " */
        
        line -> dirData = calloc(1, sizeof(DirectiveData));
        *(line -> dirData) = DEFAULT_DIRECTIVE_DATA;  /*initializing DirectiveData value, else it may contain garbage*/
        line -> dirData -> dataType = STRING;       /*DirectiveData of type STRING*/
        line -> dirData -> charArray = calloc(strlen(operands)+1, sizeof(char));    /*initializing union to store string and '\0' */
        strcpy(line -> dirData -> charArray, operands);         /*copy string to directiveData struct*/
        
    }else if(operands){                     /*if directive of type .entry or .extern*/
        line -> numOfOperands = 1;          /*there is single operands and it should be label*/
        strcpy(line -> op_src, operands);   /*we write it as source operand*/
    }
}






