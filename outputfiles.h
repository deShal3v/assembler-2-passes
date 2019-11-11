#ifndef OUTPUTFILES_H
#define OUTPUTFILES_H
    /*Constants */
    #define OBJECT_SOURCE_SUFFIX ".ob"
    #define EXT_OUTPUT_SUFFIX ".ext"
    #define ENT_OUTPUT_SUFFIX ".ent"
    #define MAX_FILE_NAME 30
    #define CODING_LINE_SIZE 7
    #define CODING_LINE_MAX_INDEX (CODING_LINE_SIZE -1)

    /*Generate remaining output files */
    int generateRemainingOutputFiles(char *);

    /*Add a line to the extern file
     * The line contains the symbol name and its counter value at the time of use */
    int addToExternFile(char * , int , char * );

    /*Add a line to the entry file
     * The line contains the symbol name and  value  */
    int addToEntryFile(char * , long , char * );

    /*clear the private information and storage of the module */
    void clearOutputFiles(void);

    /*remove output files in a case of an error */
    void removeOutputFiles(void);

#endif /* OUTPUTFILES_H */
