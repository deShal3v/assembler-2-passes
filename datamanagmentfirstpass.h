#include "dataManagment.h"

#ifndef DATAMANAGMENTSECONDPASS_H
#define DATAMANAGMENTSECONDPASS_H

    /*Data managment and encoding functions*/
    /*Encodes data into memory */
    void encodeData(long  [], int);
    /*Encode string into memory */
    void encodeString(char *);

    /*Codes first instruction word by opcode and adressing methods of operands */
    int codingInstructionWord(char, adressing_method, adressing_method);
#endif /* DATAMANAGMENTSECONDPASS_H  */
