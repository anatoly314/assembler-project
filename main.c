/*
    Main method
    (c) Anatoly Tarnavsky 2015
 */

#include <stdio.h>
#include <dirent.h>



#include "parse_helpers.h"
#include "file_parser.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"

#define printLV(L,I,i,p1,p2) (L -> I + i) -> p1, (L -> I + i) -> p2 /*macro to simplify access to labels values*/

boolean IS_ERROR; /*Global variable which represents is there error in current file*/

void write2File(const char* fileName);  /* if compilation successfull will be called this method to write output file*/
void freeTempData();    /* FREEING DATA BETWEEN CALLS*/

int main(int argc, const char * argv[]) {
    
    IS_ERROR = FALSE;
    
    if (argc < 2) {
        printError(NULL, MISSING_FILENAME, 0);
        return 0;
    }
    
    while (*++argv) {
        printError(*argv, INITIALIZING, 0); /*initializing error printError method, so it will be able print fileName too*/
        processPass(*argv, TRUE);           /*process first pass*/
        if (!IS_ERROR) {                    /*if wasn't error process second pass*/
            processPass(*argv, FALSE);
        }
        if (!IS_ERROR) {                    /*if wasn't error calculate codeSize of program*/
            RAM -> codeSize = RAM -> size;
            addDataToRam();                 /*add data from DATA_IMAGE to RAM*/
            RAM -> dataSize = RAM -> size - RAM -> codeSize;    /*calculate dataSize of program*/
            write2File(*argv);              /*write to file*/
        }
        
        freeTempData();                     /*reset for next file*/
    }
    
    return 0;
}


/* FREEING DATA BETWEEN CALLS*/
void freeTempData(){
    
    if (LABELS_TABLE) {
        if (LABELS_TABLE -> labelAddressPairs) {
            free(LABELS_TABLE -> labelAddressPairs);
        }
        free(LABELS_TABLE);
        LABELS_TABLE = NULL;
    }
    
    if (EXTERNAL_LABELS_TABLE) {
        if (EXTERNAL_LABELS_TABLE -> labelAddressPairs) {
            free(EXTERNAL_LABELS_TABLE -> labelAddressPairs);
        }
        free(EXTERNAL_LABELS_TABLE);
        EXTERNAL_LABELS_TABLE = NULL;
    }
    
    if (RAM) {  /*RESET RAM*/
        RAM -> size = 0;
        RAM -> codeSize = 0;
        RAM -> dataSize = 0;
    }
    
    if (DATA_IMAGE) {   /*FREE DATA IMAGE where stored data from Directives .string, .data*/
        if (DATA_IMAGE -> dataRows) {
            free(DATA_IMAGE -> dataRows);
        }
        
        free(DATA_IMAGE);
        DATA_IMAGE = NULL;
    }
    
    IS_ERROR = FALSE; /*Reset error flag*/
    printError(NULL, FINISHIZING, 0);
    
}

/* if compilation successfull will be called this method to write output file*/
void write2File(const char* fileName){
    
    char* asmFileName = calloc(strlen(fileName) + strlen(".as") + 1, sizeof(char));
    char* objFileName = calloc(strlen(fileName) + strlen(".ob") + 1, sizeof(char));
    char* extFileName = calloc(strlen(fileName) + strlen(".ext") + 1, sizeof(char));
    char* entFileName = calloc(strlen(fileName) + strlen(".ent") + 1, sizeof(char));
    
    strcat(asmFileName, fileName);
    strcat(asmFileName, ".as");
    
    strcat(objFileName, fileName);
    strcat(objFileName, ".ob");
    
    strcat(extFileName, fileName);
    strcat(extFileName, ".ext");
    
    strcat(entFileName, fileName);
    strcat(entFileName, ".ent");
    
    FILE *fp;
    
    /*writing object file*/
    fp = fopen(objFileName, "w+");
    fprintf(fp, "%X %X", RAM -> codeSize, RAM -> dataSize);
    int i = 0;
    while (i < RAM -> size) {

        fprintf(fp, "\n%X %03X", printLV(RAM, ramWords, i, address,value));

        i++;
    }
    fclose(fp);
    
    /*writing external labels file*/
    if (EXTERNAL_LABELS_TABLE) {
        fp = fopen(extFileName, "w+");
        
        i = 0;
        while (i < EXTERNAL_LABELS_TABLE -> size) {
            
            fprintf(fp, "%s %X\n",printLV(EXTERNAL_LABELS_TABLE, labelAddressPairs, i, label, decimalValue));
            i++;
        }
        fclose(fp);
    }
    
    
    
    
    /*writing entry labels file*/
    if (LABELS_TABLE) {
        fp = fopen(entFileName, "w+");
        
        i = 0;
        while (i < LABELS_TABLE -> size) {
            if ((LABELS_TABLE -> labelAddressPairs + i) -> labelType == ENTRY_LABEL) {
                fprintf(fp, "%s %X\n",printLV(LABELS_TABLE, labelAddressPairs, i, label, decimalValue));
            }
            
            i++;
        }
        fclose(fp);
    }
    
    
    printf("\nFile %s has been successfully compiled.\n", asmFileName);
    
    free(asmFileName);
    free(objFileName);
    free(extFileName);
    free(entFileName);


}






