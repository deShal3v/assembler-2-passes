#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

    #define MAX_LINE_SIZE 80
    #define CODING_REGION_START 100

    /*Type contains symbol type */
    typedef enum  {CODE, DATA, MACRO, EXTERNAL,ENTRY,  NO_VALUE}SYMBOL_TYPE;

    typedef struct {

        char name[MAX_LINE_SIZE];
        SYMBOL_TYPE type;
        long value;
    }SYMBOL;

    /*Exported functions for taking care of the sym table*/
    int pushToSymbolTable(char* name, SYMBOL_TYPE type, long value);

    /*Get a symbol value from the table */
    long getExistingSymbol(char* , int *);

    /*Check if macro with the specified name exist */
    int checkForExistingMacro(char* , long* );

    /*Updates each symbol in the symbol's value table */
    void updateEachDataSymbol(void);

    /*Frees the symbol table */
    void freeSymbolTable(void);


    /*Macros controllng the IC */
    #define incIC IC++;
    #define decIC IC--;

#endif /*DATASTRUCTURES_H */
