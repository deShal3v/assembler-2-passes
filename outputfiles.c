/* this files contains function handles the output files generation process
 Main tasks:
    - generating the .ob file using the codesection array
    - generating the .entry file by the symbol table (just if we have an entry instruction in the source)
    - generating the .extern file by the symbol table (just if we have an extern instruction in the source)
*/

#include <stdio.h>
#include <string.h>
#include "dataManagment.h"
#include "outputfiles.h"

/*Externing the counters */
extern int DC, IC;

static char * fname_ext , * fname_ent;


/*Internal functions protos */
int generateObjectFile(char *);
int generateExtFile(char *);
int generateEntFile(char *);
char tobase4(int);

/*Generates the remaining output files */
int generateRemainingOutputFiles(char * name){

    return generateObjectFile(name);
}

void codeLine(FILE * foutput , char * line , int line_num){
    fprintf(foutput , "%04d  %.*s\n", line_num, CODING_LINE_SIZE, line);   /*print the line number in four digits */
}

/*codes initial headers in .ob file  */
void codeInitials(FILE * foutput , int ic , int dc){
    fprintf(foutput, "   %d %d\n", ic , dc);
}

/*generates object file from stored values */
int generateObjectFile(char * name){

    FILE * foutput;
    char ofilename[MAX_FILE_NAME];
    char codingline[CODING_LINE_SIZE];
    int i, j;
    strncpy(ofilename, name , MAX_FILE_NAME);
    strcat(ofilename, OBJECT_SOURCE_SUFFIX);
    foutput = fopen(ofilename, "w");    /*open the dst file for writing  */
    if(!foutput){
        fprintf(stderr , "Aborting! Unbale to create output file %s \n", ofilename);
        return 0;
    }
    codeInitials(foutput, IC ,DC);
    for(i = 0; i< IC ;i++){
        for(j = 0; j < CODING_LINE_SIZE; j++)
            codingline[CODING_LINE_MAX_INDEX - j] = tobase4(getTwoBItsValueCode(j *2, i));
        codeLine(foutput, codingline , i + 100);
    }
    for(i = 0; i< DC ;i++){
        for(j = 0; j < CODING_LINE_SIZE; j++)
            codingline[ CODING_LINE_MAX_INDEX - j] = tobase4(getTwoBItsValueData(j *2 , i));
        codeLine(foutput, codingline , i + 100 + IC);
    }

    fclose(foutput);    /*closing the output file in the end of the writing */

    return  1;
}

/*interacts with the ext output file */
int addToExternFile(char * name , int word_number, char * fname){
    FILE * fd;
    static int isfirst =1;
    static char ofilename[MAX_FILE_NAME];   /*this buffer will hold the filename */

    if(*ofilename && strncmp(fname , ofilename, (size_t)(strchr(ofilename, '.') - ofilename)))
        isfirst = 1;

    if(isfirst){    /*if its the first line to add to this file */
        strncpy(ofilename, fname , MAX_FILE_NAME);
        strcat(ofilename, EXT_OUTPUT_SUFFIX);
        fname_ext = (char *)ofilename;
    }
    if(!(fd = (isfirst)?fopen(ofilename, "w"):fopen(ofilename, "a")))   /*creating /updating the file */
        return 0;

    fprintf(fd, "%s   %04d\n", name , word_number);
    fclose(fd);
    isfirst = 0;
    return 1;

}


/*Adds a line to the extern file */
int addToEntryFile(char * name , long value, char * fname){

    FILE * fd;
    static int isfirst =1;
    static char ofilename[MAX_FILE_NAME];   /*this buffer will hold the filename */


    if(*ofilename && strncmp(fname , ofilename, (size_t)(strchr(ofilename, '.') - ofilename)))
        isfirst = 1;

    if(isfirst){    /*we have to make a new file , either for a new source */
        strncpy(ofilename, fname , MAX_FILE_NAME);
        strcat(ofilename, ENT_OUTPUT_SUFFIX);
        fname_ent = (char *)ofilename;
    }
    if(!(fd = (isfirst)?fopen(ofilename, "w"):fopen(ofilename, "a")))   /*creating /updating the file */
        return 0;

    fprintf(fd, "%s   %04li\n", name , value);
    fclose(fd); /*closes teh file in the end */
    isfirst = 0;
    return 1;

}

/*clears the static pointes  for the next use */
void clearOutputFiles(){
    fname_ext = fname_ent =NULL;
}

/*removes the output fiels if necessary */
void removeOutputFiles(){
    if(fname_ent != NULL)
        remove(fname_ent);
    if(fname_ext != NULL)
        remove(fname_ext);
    clearOutputFiles();
}

/*converting to special - base4 as desdcribed in page 39*/
char tobase4(int a) {

    switch (a) {
        case 0:
            return '*';
        case 1:
            return '#';
        case 2:
            return '%';
        case 3:
            return '!';
    }

    return EOF;
}






