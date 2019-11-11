/*Module contains current module analyzing functions and implementations */

#include "moduleanalyzing.h"
#include "firstpass.h"
#include "outputfiles.h"
#include "secondpass.h"

#include <stdio.h>
#include <string.h>

void freeCodeDataRegions(void);
void freeSymbolTable(void);
void setCurModuleName(char *);  /*prototype for the set current module functio  */

/*finishes the current source module processing */
#define FINISH_PROCESS_CUR_MODULE(isError) {\
    if(isError)\
        fprintf(stderr, "Aborting! compilation  of file %s terminated\n",getCurModuleName());\
    clearOutputFiles(); /* clearing the output files data */\
    freeSymbolTable();\
    freeCodeDataRegions();  /*frees the symbol table */\
    continue;\
}

/*Assemblying given source file */
void processInputFIles(char ** inputfiles, int numofinputs){
    FILE* fd;
    static char* filenamenoext; /*file name with no extension */
    char filename[MAX_FILE_NAME_SIZE];
    int i;
    memset(filename, 0, MAX_FILE_NAME_SIZE);

    for (i = 1; i < numofinputs; i++) {


        strcpy(filename, inputfiles[i]);    /*copying the current source name */
        strcat(filename, ASSEMBLY_SOURCE_SUFFIX);

        /*open the file specified */
        fd = fopen(filename, "r");

        if (!fd) {  /*couldnt read source */
            fprintf(stderr, "%s %s , No such file or directory", "Unable to read source file , ", filename);
            continue;
        }
        filenamenoext = inputfiles[i];
        setCurModuleName(filename); /*sets the current module name */
        if (!firstPass(fd))
            FINISH_PROCESS_CUR_MODULE(ERROR_IN_MODULE)

        /*updating the file pointer to point at the beginning of the file */
        rewind(fd);

        if (!secondPass(fd, filenamenoext))
            FINISH_PROCESS_CUR_MODULE(ERROR_IN_MODULE)

        fclose(fd); /*closed the file pointer */

        if(!generateRemainingOutputFiles(filenamenoext))
            FINISH_PROCESS_CUR_MODULE(ERROR_IN_MODULE); /*finish current processing with error */
        FINISH_PROCESS_CUR_MODULE(NO_ERROR_IN_MODULE)   /*finish current processing with no error */
     }

}

/*current module info of the current file */

static char * module_name;

char * getCurModuleName(){
     return module_name;
}

void setCurModuleName(char * new_name){
    module_name = new_name;
}

