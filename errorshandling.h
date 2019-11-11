#ifndef ERRORSHANDLING_H
#define ERRORSHANDLING_H
    #include <stdio.h>
    #include "moduleanalyzing.h"
    extern int LC;

    /*Raise a parser error */
    #define raiseParserError(error) {\
        fprintf(stderr, "%s:%d:""\x1b[31m" "error" "\x1b[0m" ": %s\n", getCurModuleName(),  LC , error);\
        firstpassErrorFlag = 1;\
    }

    /*Raise a warning */
    #define raiseWarning(warning){\
        fprintf(stderr, "%s:%d:""\x1b[35m" "warning" "\x1b[0m" ": %s\n", getCurModuleName(),  LC , warning);\
    }
    /*Raise an adressing method error */
    #define raiseAdressingMethodError(error , addrm){\
        fprintf(stderr, "%s:%d:""\x1b[31m" "error" "\x1b[0m" ": %s "  ":adressing method %d is not allowed\n",getCurModuleName(), LC , error , addrm);\
        firstpassErrorFlag =1;\
    }
    /*Symbol errors */
    #define raiseSymError(sym_name){\
        extern int firstpassErrorFlag;\
        fprintf(stderr, "%s:%d:""\x1b[31m" "error" "\x1b[0m" ": Symbol name '%s' is already exist\n", getCurModuleName(),  LC , sym_name);\
        firstpassErrorFlag = 1;\
    }

    /*Raise error occured during the second pass */
    #define secondPassError(error, name){\
        if (name != NULL)\
            fprintf(stderr, "%s:%d:""\x1b[31m" "error" "\x1b[0m" ": %s : '%s'\n", getCurModuleName(),  LC ,error, name);\
        else{\
            fprintf(stderr, "Error , line %d %s\n", LC , error);\
        }\
        secondpassErrorFlag = 1;\
    }

#endif /*ERRORSHANDLING_H*/
