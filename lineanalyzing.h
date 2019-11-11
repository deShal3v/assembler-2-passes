/*LINEANALYZING */
#ifndef  LINEANALYZING_H
#define LINEANALYZING_H

    #include <stdio.h>
    #define MAX_LINE_SIZE 80
    extern int LC;

    typedef enum { CMD_MACRO, CMD_STRING, CMD_DATA, CMD_ENTRY, CMD_EXTERN, INVALID }CMD_TYPE;
    /*macro strips a : from a valid label */
    #define stripLabel(label){\
        char tmpbuf[MAX_LINE_SIZE];\
        strcpy(tmpbuf, label);\
        memset(label, '\x00', strlen(label));\
        strncpy(label, tmpbuf, strchr(tmpbuf, ':') - tmpbuf);\
    }

    /*enum contains num of params */
    enum { NO_PARAMS, ONE_PARAM, TWO_PARAMS };

    /*typedef indicates if we need to keep analyzing this line
     * probably because of a label declared at the beggining */
    typedef enum { NOT_FINISHED_THIS_LINE, FINISHED_THIS_LINE }LINE_STATE;


    /*functions prototypes */

    /*Inputing a line from the source file with additional length check */
    int getLineCheckLen(char * , int , FILE *);
    /*Analyze a macro line */
    void analyzeMacroLine(int);
    /*Analyze a data line */
    void analyzeDataLine(void);
    /*Analyze a string line */
    void analyzeStringLine(void);
    /*Analyze an external line */
    int analyzeExternLine(char*);
    /*Analyze a code line */
    int analyzeCodeLine(char *);
    /*Checks if a given string is a valid lable name */
    int isValidLabel(char *, int);
    /*Checks if a string is in a number structure */
    int isNumberString(char *);
    /*Gets number of arguments for instruction by name */
    int getNumOfArgsByName(char*);
    /*Supply symbolic value represents a command type */
    CMD_TYPE typetoIndex(char*);
    /*Raise error if multiple labels were in use for the first pass */
    void multipleLabelsError(void);
    /*Removes initial white chars from string */
    char* removeInitailSpaces(char* buf);
    /*Checks whether or not a given string is lable declaration */
    char * isLabelDeclaration(char* token);

    /*not content after label error */
    void noLineContent(void);
    /*analyzes an entry line's syntax during the first pass */
    void fpAnalyzeEntryLine(int);

#endif /*LINEANALYZING_H*/
