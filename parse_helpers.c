#include "parse_helpers.h"

/* empty line is comment line or line of spaces*/
boolean isEmpty(char *text) {
    while (*text != '\0' && *text !=';') {
        if (!isspace(*text))
            return FALSE;
        text++;
    }
    return TRUE;
}


/* custom strtok_a, a - advanced which knows to take multiple delimeters, each delimeter should be single char*/
char* strtok_a(char *str, int delimNum, ...){
    static char* start;             /*here we store start of unparsed string between calls*/
    static char* delimeters;         /*here we store list of delimeters between calls*/
    
    if (str) {  /*it means first run on the current string, each next call str will be NULL as original strtok*/
        
        if (delimeters) {   /*clean delimeters from previous iteration*/
            free(delimeters);
            delimeters = NULL;
        }
        
        start = str;
        delimeters = calloc(delimNum+1, sizeof(char));
        
        va_list p;              /*we fill delimeters array with delimeters, only single char!*/
        va_start(p,delimNum);
        int delimIndex = 0;
        while (delimIndex < delimNum) {
            *(delimeters+delimIndex) = *va_arg(p,char*);
            delimIndex++;
            
        }
        *(delimeters+delimIndex) = '\0';
    }
    
    char* result = NULL;
    
    if (start) {
        int offset = 0;
        for (;isCharPresent(*start,delimeters) && start!='\0';start++) /*looking for first char which isn't in a list*/
            ;
        
        for (;!isCharPresent(*(start+offset), delimeters) && *(start + offset)!= '\0'; offset++)/*looking for last chart in current word*/
            ;
        
        if (offset > 0) {/*if we found something we create variable result and return it*/
            result = calloc(offset+2, sizeof(char));
            strncpy(result, start, offset);
            result[offset+1] = '\0';
            start+=offset;
        }
        
    }
    return result;
}


/* helper function for strtok_a, it looks is there a symbol inside an array of delimeters*/
boolean isCharPresent(char symbol, char* delimeters){
    
    for (;*delimeters != '\0'; ++delimeters) {
        if (*delimeters == symbol) {
            return TRUE;
        }
    }
    return FALSE;
    
}

/* helper function which checks if a valid number, valid numbers are: +5, -5, 5, -5*/
boolean isNumber(const char *str){
    if (*str == '-' || *str == '+') {
        str++;
    }
    
    while(*str != '\0')
    {
        if(*str < '0' || *str > '9')
            return FALSE;
        str++;
    }
    return TRUE;
}

/* helper function which checks if token type matches token's position, for example label can be only at first position*/
boolean isTokenTypeMatchesPosition(TokenType type, Position position){
    int i;
    for (i=0; i < (sizeof(tokenPositionsPairs)/sizeof(tokenPositionsPairs[0])); i++) {
        if(tokenPositionsPairs[i].type == type && (tokenPositionsPairs[i].position & position) > 0){
            return TRUE;
        }
    }
    return FALSE;
}

/*There isn't getline in ANSI C library so I've done it by myself*/
int getline_c(char** line, FILE* file){
    
    if (*line != NULL) { /*if *line contains text from the previous call we clear it*/
        free(*line);
        *line = NULL;
    }
    
    int ch = 0;
    
    int count = 0;
    for( ;; )
    {
        if (!*line) {
            *line = calloc(1, sizeof(char));
        }else{
            *line = realloc(*line, (count+1)*sizeof(char));
        }
        
        ch = fgetc( file );
        
        if (ch == EOF) {
            return -1;
        } else if (ch != '\n'){
            *(*line + count) = ch;
        } else {
            *(*line + count) = ch;
            count++;
            return count;
        }
        
        count++;
    }
    
    return count;
}



