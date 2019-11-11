/*Module handles the second pass operations and function calls */

#include "lineanalyzing.h"
#include "dataStructures.h"
#include "outputfiles.h"
#include "datamanagmentsecondpass.h"
#include "errorshandling.h"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>




extern int IC;  /*externing the instruction counter */


static int secondpassErrorFlag = 0;
static char * name;



/*prototypes for internal function in current module */
int finishCommandCoding(char *, int , int);
int codeLine(char*, FILE*);
int argAnalyzing(adressing_method , int);


/*Function responsible for the second pass operations and managing
 * args file pointer and filename
 * return value  is non zero in function succedded*/
int secondPass(FILE* fd, char * filenamenoext) {

    char* n;
	char* token;
    char lineread[MAX_LINE_SIZE];

    IC = 0; /*setting the IC for this pass */
    LC = 0;
    name = filenamenoext;
	while (fgets(lineread, MAX_LINE_SIZE, fd)) {
		/*during the second pass we can asume that the syntax is ok!! and we shouldnt check for syntax errors*/
		
        LC++;
		if (!(n = removeInitailSpaces(lineread)) || n[0] == ';') 	/*First we will check if this is a blank line/comment */
			continue;
        token = strtok(n, "\t\n\r ");
		if (isLabelDeclaration(token))	/*this is a lable declaration*/
            token = strtok(NULL, "\t\n\r ");

        switch (typetoIndex(token)) {   /* see what type of command it is */

			case CMD_DATA:	
            case CMD_MACRO:
            case CMD_EXTERN:
            case CMD_STRING:
                break;  /*All these cases were already handled */
            case CMD_ENTRY:
                /*we have to add it to the entry file */
            {
                long value;
                int state = -1;
                token = strtok(NULL , "\t\n\r ");
                value = getExistingSymbol(token , &state);
                if(value == -1 && state == -1)
                    secondPassError("Invalid label name for .entry instruction. Symbol is not defined", token);
                if(state == EXTERNAL)   /*Error , we shouldnt allow .entry for an external label */
                    secondPassError("Invalid label specified as an entry argument, label must be initialized in current source file", token)
                if(state == MACRO)  /* macro is not a proper symbol for entry */
                    secondPassError("Invalid label specified to entry command , got a macro", token)
                if(!secondpassErrorFlag)
                    addToEntryFile(token, value , filenamenoext);   /*add to entry file if we dont have any errors until now */
                break;
             }

			default:
                finishCommandCoding(token, getTwoBItsValueCode(SRC_ADRESSING_OFFSET, IC), getTwoBItsValueCode(DST_ADRESSING_OFFSET, IC));
		}
		

	}


    if(secondpassErrorFlag){ /* we had  have some errors during the second pass, now lets remove the ofiles */

        removeOutputFiles();
        return 0;
    }
    return 1;

}

enum{ARG_DST , ARG_SRC};


/*finished the command codint into memory
 * codes params words into memory
 * return value is non-zero if succeedded */
int finishCommandCoding(char * instr, adressing_method addrSrc , adressing_method addrDst) {
	int nofargs;

	/*
	1. coding the first argument's data words in the memory
	2/. see if we should share the same word for the second argument (if there is one)
	3. coding the second argument's data words in the memory
	-- we shouldt always parse the command all over again
	*/
    incIC
	nofargs = getNumOfArgsByName(instr);
    if (nofargs == 2) /*there is a source operand*/
        argAnalyzing((char)addrSrc, ARG_SRC);
    /*now we have to update the IC , for the seconf argument*/
    if (nofargs) {	/*there is a dst operand*/
        if(addrDst == REG_ADDRESSING && addrSrc == REG_ADDRESSING)
            decIC
        argAnalyzing(addrDst, ARG_DST);
    }
    return 1;
}

/*analyze argument , and updating a char array saves the final line to code*/
int argAnalyzing(adressing_method indic, int issrc) {
	char* operand;

    operand = (issrc == ARG_SRC)?strtok(NULL, ","):strtok(NULL, "\t\n\r "); /*decide which argument it is */
    sscanf(operand , "%s",operand);
	switch (indic) {

        case IMM_ADDRESSING:		/*its immed/no_src adressing method*/
        {
            /*can be a macro too */
            long l;
            if(!checkForExistingMacro(operand + 1, &l)) /*this is not a macro */
                l = strtol(operand +1, NULL , 10);
            encodeParamWord(l, ARE_ABSOLUTE, MAX_INDEX, MIN_INDEX);	/*its a numerical value*/
			break;
		}
        case STRAIGHT_ADDRESSING: /*its direct adressing (1)*/
        {
			long value;
			int type;
            if ((value = getExistingSymbol(operand, &type)) ==-1)
                secondPassError("undefined symbol specified ", operand)
			else {
                   encodeParamWord(value, (type == EXTERNAL) ? ARE_EXTERNAL : ARE_RELOCATABLE, MAX_INDEX, MIN_INDEX);	/*because of the syntax is ok at this point we can assume that there is enough space for the encoding*/
            }

            /*if we use an external label in this instruction */
            if(type == EXTERNAL && !secondpassErrorFlag)
                addToExternFile(operand , 100 + IC, name);

			break;
		}
        case BY_INDEX_ADRESSING:
		{
			int type;
            char token[MAX_LINE_SIZE];
			long value;
            sscanf(operand, "%[^[]s", token);   /*scan it until the [ sign, which surely exists */


            if ((value = getExistingSymbol(token, &type)) == -1)	/*a proper adress cannot be NULL*/
                secondPassError("undefined symbol specified ", token)
			else {
                encodeParamWord(value, (type == EXTERNAL) ? ARE_EXTERNAL : ARE_RELOCATABLE, MAX_INDEX, MIN_INDEX);	/*because of the syntax is ok at this point we can assume that there is enough space for the encoding*/

			}

            /*if we use an external label in this instruction, we should add it */
            if(type == EXTERNAL && !secondpassErrorFlag)    /*add just if there were no errors before */
                addToExternFile(token , IC + 100, name);

            incIC   /*updating the IC for the second word in memory */

            sscanf(strchr(operand, '[') + 1, "%[^]]s", token);  /*scanning the number inside the brackets */

            if(isNumberString(token))
                value  = strtol(token  , NULL, 10); /*this is a numerical value */
            else
                checkForExistingMacro(token , &value);

            if (!secondpassErrorFlag)
                encodeParamWord(value, ARE_ABSOLUTE, MAX_INDEX, MIN_INDEX);

			break;
		}
        case REG_ADDRESSING:	/*its a register, here we have to know if its src or dst */
		{
			if (issrc) {
                encodeParamWord(strtol(operand + 1, NULL, 10), ARE_ABSOLUTE, SRC_INDEX, END_SRC_INDEX);
			}
			else 
                encodeParamWord(strtol(operand + 1, NULL, 10), ARE_ABSOLUTE, DST_INDEX, MIN_INDEX);
			
			break;
		}

        default:
            break;
	}

    incIC   /*updating the IC */
	return 0;
}





