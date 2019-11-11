#ifndef INSTRUCTIONSSET_H
#define INSTRUCTIONSSET_H
    /*contains tables , constants and functions ralted to the instruction set and registers */

    #define NUM_OF_ASSEMBLY_INTSRUCTIONS 16
    #define NUM_OF_NON_ASM_INSTRUCTION 5
    #define NUM_OF_REGISTERS 8
    #define ALL 15
    #define ONETOTHREE 14
    #define ONEANDTHREE 10
    #define ONEANDTWO 6

    /*checks if the operand `actual` is a valid adressing method*/
    int isValidArgumentByAddrMethod(int expected , int actual);
    /*Get the number of  arguments by command name */
    int getNumOfArgsByName(char *);
    /*Checks if the given string is a name of instruction */
    int isNameOfInstruction(char * token);
    /*Checks if the given string is a name of command */
    int isNameOfCommand(char * token);
    /*Checks if the name is a name of instruction and updating the given properties */
    int validateInstruction(char *  , unsigned int *  , char * );    /*validare an instruction name*/

    /*Get the src allowed adressing methods for instruction */
    int getSrcAdressing(int i);
    /*Get the dst allowed adressing method for instruction */
    int getDstAdressing(int i);

#endif /* INSTRUCTIONSSET_H */
