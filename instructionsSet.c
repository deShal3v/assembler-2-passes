/*Contains symbols and operations related to the instruction set and possible commands */

#include "instructionsset.h"
#include <string.h>


/*Lets hold the instructions struct*/
struct{

    char* name;
    int numofparams;
    unsigned  int src : 4;	/*LSB WILL hold method number 0  and so on*/
    unsigned int dst : 4;
    char opcode;
} static instructionsarr[NUM_OF_ASSEMBLY_INTSRUCTIONS] = { {"mov",2, ALL,  ONETOTHREE, '\x00'} , {"cmp", 2,ALL, ALL , 1},{"add" , 2, ALL, ONETOTHREE, 2}, {"sub" , 2, ALL , ONETOTHREE, 3},{"not", 1, 0  , ONETOTHREE, 4}, {"clr", 1,0, ONETOTHREE, 5},\
{"lea", 2, ONEANDTWO, ONETOTHREE, 6}, { "inc", 1,0, ONETOTHREE , 7 }, { "dec", 1, 0, ONETOTHREE, 8 }, { "jmp", 1, 0,  ONEANDTHREE , 9 }, { "bne", 1, 0, ONEANDTHREE, 10 }, { "red", 1, 0, ONETOTHREE, 11 }, { "prn", 1,0, ALL, 12 },\
{"jsr", 1, 0, ONEANDTHREE, 13}, { "rts", 0 ,0,0, 14 }, {"stop", 0,0,0, 15} };

const char* expectedtyped[] = { ".define", ".string", ".data", ".entry", ".extern", 0};

int validateInstruction(char * name , unsigned int * nofparams , char * opcode){
    int i;
    for (i = 0; i < NUM_OF_ASSEMBLY_INTSRUCTIONS; i++) {

        if (strcmp(name, instructionsarr[i].name) == 0){

            *nofparams = (unsigned int)instructionsarr[i].numofparams;
            *opcode = instructionsarr[i].opcode;
            return 1;
        }

   }
   return 0;
}

int getSrcAdressing(int i){
    return instructionsarr[i].src;
}

int getDstAdressing(int i){
    return instructionsarr[i].dst;
}

/*checks if token is a name of instruction */
int isNameOfInstruction(char * token){
    int i;
    for (i = 0; i < NUM_OF_ASSEMBLY_INTSRUCTIONS; i++) {

        if (strcmp(token, instructionsarr[i].name) == 0)
           return 1;

    }

    return 0;
}

/*checks if token is a name of command */
int isNameOfCommand(char * token){

    int i;
    for (i = 0;i < NUM_OF_NON_ASM_INSTRUCTION;i++) {
        if(strcmp(token , expectedtyped[i]) == 0 || strcmp(token , expectedtyped[i] + 1) == 0)
            return 1;
    }
    return 0;
}


/*checks if the operand actual is a valid adressing method
 * actual(int) - the actual adressing method was used
 * expected(int) - specifies all valid adressing method, every bit contains data about another adressing method)*/
int isValidArgumentByAddrMethod(int expected , int actual){

    int actualformat  = 1 << actual;

    if(actual == -1)    /*its an error_adressing method , shouldnt complain about anything */
        return 1;

    if(!(expected & actualformat))  /*this adressing bit method is off , the adressing method is invalid */
        return 0;
    return 1;
}


/*retrieves the number of arguments for the command with the name `name` */
int getNumOfArgsByName(char* name) {

    int i;
    for (i = 0; i < NUM_OF_ASSEMBLY_INTSRUCTIONS; i++) {
        if (strcmp(instructionsarr[i].name, name) == 0)
            return instructionsarr[i].numofparams;
    }

    return -1;
}
