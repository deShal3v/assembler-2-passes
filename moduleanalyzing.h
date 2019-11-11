#ifndef MODULEANALYZING_H
#define MODULEANALYZING_H

    #define ASSEMBLY_SOURCE_SUFFIX ".as"
    #define MAX_FILE_NAME_SIZE 2048
    /*specifies wheteher or not the current module contains error */
    enum{NO_ERROR_IN_MODULE , ERROR_IN_MODULE};

    /*Exposes the current module name */
    char * getCurModuleName(void);
    /*Sets the current module name*/
    void setCurModuleName(char *);
    /*managing the input files assemblying process */
    void processInputFIles(char * [], int);
#endif /* MODULEANALYZING_H */
