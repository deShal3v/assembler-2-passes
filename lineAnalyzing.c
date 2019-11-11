/*Module responsible of all the line anallyizing and parsing operations
*including of detecting syntax errors and exporting them if needed
*/

#include "dataStructures.h"
#include "lineanalyzing.h"
#include "instructionsset.h"
#include "datamanagmentfirstpass.h"
#include "moduleanalyzing.h"
#include "errorshandling.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

/*macro detecting whether or not we've got too many arguments for instruction */
#define CHECK_TOO_MANY_ARGS(expected){\
    char * externours;\
    if (!isspacestring(externours = strtok(NULL, ""))) {  /*we have externous text after the end of command */\
        if(!(isanotherparam(externours)))\
           raiseParserError("Externous text after end of instruction or command")\
        else\
            fprintf(stderr, "%s:%d:""\x1b[31m" "error" "\x1b[0m" ": Too many arguments for instruction, expected %d\n", getCurModuleName(),  LC , expected);\
            firstpassErrorFlag = 1;\
     }\
}



extern int firstpassErrorFlag;  /*externing the error flag for the first pass */
int LC; /*specifies the line number */

/*get a line from the source assembly file ,and check for length
 * aiming for the first pass , where we should check the line size */
int getLineCheckLen(char * linebuffer , int max_size_allowed , FILE * fd){

    int c, i = 0, linetoobig = 0;   /*linetoobig is a flag indicates if we long line , above the max_size_allowed */
    LC++;
    memset(linebuffer, '\x00', (size_t)max_size_allowed);
    while((c = fgetc(fd))!= EOF && c !='\n'){
        if(i <= max_size_allowed)   /*we are still in the allowed range */
            linebuffer[i] = (char)c;
        else if(!linetoobig){
            raiseParserError("Line size is too big, Line should contain at most 80 characters")
            linetoobig =1;
        }
        i++;
    }

    if(c == EOF)    /*we've reached the end-of-file */
        return 0;
    return 1;

}

/*checks if a number is in the num_of_bits  represantation range or not
 *return zero if not out of range , otherwise return value is non-zero
*/
int outofrange(long l, size_t num_of_bits) {

    return (l < (signed int) (-1 * (1 << (num_of_bits -1))) || l > (signed int)(1 << (num_of_bits -1)));    /*because of the assembler supports negative  numbers (in the two complement method) , we need to set min/max that we can  hold in 12 bits */
}



/*multiple label declarations in the same line */
void multipleLabelsError(){

    raiseParserError("multiple labels declared ")
}

void noLineContent(){
    raiseParserError("no content found after an opening label")
}

/*removes initail spaces from a string, returns a pointer to the new string */
char* removeInitailSpaces(char* buf) {

	while (isspace(*buf))
		buf++;

	if (*buf == '\x00')
		return NULL;
	return buf;

}

/*Determines if externous text in the end of expression is another parameter or not*/
int isanotherparam(char * token){

   token = removeInitailSpaces(token);

   if(token != NULL && *token ==',')
       return 1;
   return 0;

}

/*checks if a string contains just spaces
 * return non-zero value if it contains just spaces characters
 * otherwise the return value is zero */
int isspacestring(char* string) {

	int i;
	i = 0;
	if (string == NULL)
		return 1;

	while (string[i] != '\x00' && string[i] != EOF) {
		if (!isspace(string[i]))
			return 0;
		i++;
	}

	return 1;
}

/*Analyzing a macro line */
void analyzeMacroLine(int labelInThisLine) {


    char* tmpval, * value, macro_name[MAX_LINE_SIZE];
	long val;
    int macro_error =0;

    if(labelInThisLine)
        raiseParserError("label declaration is not allowed in a macro-declaration line")

    /*Parsing the macro name and value */
    tmpval = strtok(NULL ,"=");
    sscanf(tmpval , "%s", macro_name);
    value = strtok(NULL, " \t");


    if (isspacestring(macro_name) || (value && isspacestring(value))){
        raiseParserError((macro_name == NULL) ? "No macro body specified" : "No macro value specified")
        macro_error = 1;
        if(strpbrk(tmpval, " \t"))
            raiseParserError("macro declaration should be specified using a '=' sign")
    }
    if (!isspacestring(strtok(NULL, "\t "))){
        raiseParserError("Too many arguments for macro declaration")
        macro_error =1;
    }

    if (value && !isNumberString(value)){ /*if we have a value to check */
        raiseParserError("Macro value is not a valid number")
        macro_error =1;
    }
    if(value && outofrange( val = strtol(value, NULL, 10), TOTAL_NUM_OF_BITS))
            raiseWarning("Macro value might be out of range when will be in use");

    if(!isValidLabel(macro_name, 1))   /*checks the label/macro syntax */
        macro_error = 1;

    if(macro_error) /*we had errors until now with this macro declaration */
        return;
    /*Adding the macro to the symbols table*/

     firstpassErrorFlag |=  1 ^ pushToSymbolTable(macro_name, MACRO, val);

 }

/*Analyzes an entry line during the first pass , checking syntax only */
void fpAnalyzeEntryLine(int islabel){

    char * operand;
    if(islabel)
        raiseWarning("Shouldnt assign a label to an .entry command")
    if(isspacestring(operand = strtok(NULL , ",")))
        raiseParserError("missing an argument for .entry command")
    else
        CHECK_TOO_MANY_ARGS(1)

}

/*searches for the command in the commands table
 * return the command index if found (value between 0 - 4) , otherwise return 5 */
CMD_TYPE typetoIndex(char* token) {

	int i;
	extern const char* expectedtyped[];


	for (i = 0; i < INVALID; i++) {

		if(strcmp(token , expectedtyped[i] ) ==0)	/*We have a match*/
            return (CMD_TYPE)i;
	}

	return INVALID;
}

/*Analyzing a .string declaration and checking for syntax errors*/
void analyzeStringLine() {

    char * stringparam, * lst_bracket, *initial ,  stringtoencode[MAX_LINE_SIZE];
    int i = 0;
    int onlyspaces = 1, numofbrac = 0;
    stringparam = strtok(NULL , "");
    stringparam = removeInitailSpaces(stringparam);

    if(!*stringparam){  /*no string specified */
        raiseParserError("No string to encode specified")
        return;
    }


    /*extract the string from the command , the string can contain " , and every other characrer */

    if(*stringparam == '"'){
        numofbrac++;
        stringparam++;
    }
    else {
        raiseParserError("string must start with a \" sign ")
        return;
    }
    initial = lst_bracket = stringparam;

    memset(stringtoencode, '\x00', MAX_LINE_SIZE);
    while(*stringparam){

        if(*stringparam == '"'){
            onlyspaces = 0;
            numofbrac++;
            lst_bracket = stringparam;  /*this is the location of the last quotes for now */
        }

        else if(!isspace(*stringparam))   /*non space after " sign */
            onlyspaces++;
        stringtoencode[i] = *stringparam;   /*adding the char to the new buffer */
        i++;


        stringparam++;

    }


    if(onlyspaces && numofbrac >1){ /*we already had two brackets , but we have some plain text (non white characters) without a quotes after them */
        raiseParserError("Externous text after end of string ")
        return;
    }
    if(numofbrac < 2){  /* no closing brackets were found */
          raiseParserError("No closing quotes were found in a .string command ")
          return;
    }
    else{
     stringtoencode[lst_bracket - initial] = '\x00';    /*set the null terminator at the end of the string we want (instead of the last ") */
    }
	/*Encoding the data if all went fine */

    if(firstpassErrorFlag)  /*if we had previous erros we shouldnt encode anything */
		return;
    encodeString(stringtoencode);   /*Encode the string if we dont have any errors until now */

}


/*Analyzing a .data line */
void analyzeDataLine() {

	long values[MAX_LINE_SIZE];	/*This will hold the values for encoding*/
	int i;
	char* token;
	long l;
	i = 0;


    while ((token = strtok(NULL, ","))){

		l = strtol(token, NULL, 10);
        if (!isNumberString(token = removeInitailSpaces(token))) {	/*We got one which is not a long type*/
            char oname[MAX_LINE_SIZE];
            sscanf(token , "%s", oname);
            if (!checkForExistingMacro(oname, &l)) {
                raiseParserError("Object is not a proper number for .data command")
				break;
			}
		}

        else if (outofrange(l, TOTAL_NUM_OF_BITS)) {
            raiseParserError("Constant value is out of range , cannot be stored in a memory word")
			break;
		}
		values[i] = l;
		i++;
    }
	
	if (!i)
        raiseParserError("No data supplied to .data command")
    if (firstpassErrorFlag) /*if we had errors until now we shouldnt encode anything */
		return;
	/*Time to encode this in the memory*/
	encodeData(values, i);
}


/*Analyzes an .extern line */
int analyzeExternLine(char * name_of_label) {

	char* name;
	int errorcode;
	errorcode = 1;
    name = strtok(NULL, "\t \r\n"); /*extracting the 1st parameter */
	if (isspacestring(name)) {
        raiseParserError("No parameter was given to .extern")
		errorcode = 0;
	}
    if (!isspacestring(strtok(NULL, "\t "))) {
        raiseParserError("Too many args for .extern command")
		errorcode = 0;
	}

    strncpy(name_of_label, name, strlen(name));

	return errorcode;
} 




/*checks if a string contains the : sign specifies a label*/
char * isLabelDeclaration(char* token) {
	/*Function checks for a label declaration*/
	char* tmp;
	if (!(tmp = strchr(token, ':')))
        return NULL;
	
	return tmp;
}

/*checks if a label is valid */
int isValidLabel(char* token, int isMacro) {

    int n;
    /*first check for simple syntax */
    /*as mentioned i the idtruction's papers
     * a valid lable need to meet these confinements:
     * starts in an alphabetic letter
     * after that there is a serie of number and letters
     * Max size is 31 */
    char actualLable[MAX_LINE_SIZE];
    char * al;
    int counter = 0, nonalpha_flag = 0;

    const int max_label_size = 31;
    if(!sscanf(token , "%s", actualLable))
        return 0;
    if(!isalpha(actualLable[0]))    /*a label must start in a letter */
        raiseParserError((!isMacro)?"Invalid label format , Label must starts in a letter": "Invalid macro name format , Label must starts in a letter")

    al = actualLable;
    while(*al){
        if(!isalnum(*al) && !nonalpha_flag){
            raiseParserError((!isMacro)?"Non alphanumeric characrer is not allowed in a label name": "Non alphanumeric characrer is not allowed in a macro name")
            nonalpha_flag = 1;  /*set the flag for preveting the assembler from display this error again */
        }
        counter++;
        al++;
    }

    if(counter > max_label_size)
        raiseParserError((!isMacro)?"Label name is too long, expected at most 31 chars":"Macro name is too long, expected at most 31 chars")



    /*checks if its a name of instruction */
    if(isNameOfInstruction(token)){
        raiseParserError((!isMacro)?"Invalid Label name specified , name of insruction was in use ":"Invalid macro name specified , name of insruction was in use ")
        return 0;
    }

    if(isNameOfCommand(token)){  /*its a name of instruction , Invalid name of course */
        raiseParserError((!isMacro)?"Invalid label name , instruction name cannot be used as a label name":"Invalid macro name , instruction name cannot be used as a label name");
        return 0;
    }

    /*now lets check if its a name of a register */
    if(strlen(token) == 2 && *token == 'r' && isdigit(token[1]) && (n =(int)strtol(&token[1], NULL , 10)) >= 0 && n < NUM_OF_REGISTERS){
        raiseParserError((!isMacro)?"Invalid label name , register name cannot be used as a label name":"Invalid macro name , register name cannot be used as a label name");
        return 0;
    }
	
	return 1;
}

/*checks if a string is a proper number */
int isNumberString(char* string) {
	
    char* c;
    c = string;
    if(*c == '-' || *c ==  '+') /*if we have a sign prefix */
        c++;
    while (isdigit(*c)) {
        c++;
    }
    if (isspace(*c) || *c == '\x00')
        return 1;
    return 0;
}

/*checks if a string is a register */
int isRegister(char* token) {
	long n;
	if (*token != 'r')
		return 0;
	token++;
    return (isNumberString(token) && (n = strtol(token, NULL , 10)) < NUM_OF_REGISTERS && n >= 0);
}





/*Get the adressing method of the input string
 * return a value specify the adressing method was in use */
adressing_method getAdressingMethod(char * token) {

	/*This function calculates the L param as described in  the course notebook*/
	long n;
	
	char * index;
    if ((index = strchr(token, '#'))) {
		/*This is a first-type command , which*/
		if (isNumberString(token + 1))
            if ((n = strtol(token + 1, NULL, 10)) && !outofrange( n, TOTAL_NUM_OF_BITS_WITH_ARE))
				return IMM_ADDRESSING;
			else
                raiseParserError("Constant value is out of range")
		else {
			long value;
            char macro_name[MAX_LINE_SIZE];

            sscanf(index + 1, "%s", macro_name);
			/*Its not a number , so we have to check if its a macro */
            if (!checkForExistingMacro(macro_name, &value))
                raiseParserError("Undefined macro name specified")
			else
				return IMM_ADDRESSING;
		}



	}
    else if ((index = strchr(token, '['))) {    /*it contains the char '[' */
		char tmpbuffer[MAX_LINE_SIZE];
		char* closingbrackets;
		memset(tmpbuffer, '\x00', MAX_LINE_SIZE);
		if (!(closingbrackets = strchr(index, ']')) || (closingbrackets < index))
            raiseParserError("Missing brackets in expression")
        else{
            strncpy(tmpbuffer, index + 1 , (size_t)(closingbrackets - index) -1 );
            if(!isNumberString(tmpbuffer) && !checkForExistingMacro(tmpbuffer, NULL))
                raiseParserError("Invalid macro name specified");
         }
        * index = '\x00';
        if (isValidLabel(token, 0) && (closingbrackets > index))
			return BY_INDEX_ADRESSING;

	}
	else if (isRegister(token))
		return REG_ADDRESSING;
    else if (isValidLabel(token, 0))
		return STRAIGHT_ADDRESSING;
	
	return ERROR_ADRESSING;
	
}




/*Analyzes a code line
*calculates the L param
*encode the first word of the instrcution in memory
*/
int analyzeCodeLine(char* instruction) {
	

	/*Now we have to analyze this line and see if the asressing methods were ok*/
    int  n;
	unsigned int numofparams;
	char opcode;
	adressing_method addrm;
    char* src, * dst;

    if(!validateInstruction(instruction, &numofparams, & opcode)){
         raiseParserError("Invalid command or instruction name specified")
         return 0;
    }
    switch (numofparams) {  /* checking all possible number of arguments */

        case NO_PARAMS:
            CHECK_TOO_MANY_ARGS(NO_PARAMS)

            if(!firstpassErrorFlag)
                codingInstructionWord(opcode, NO_PARAM, NO_PARAM);
            break;

        case ONE_PARAM:

            dst = strtok(NULL, " \t");
            n = isspacestring(dst);

            if(n)
                raiseParserError("Missing an argument for instruction")

            CHECK_TOO_MANY_ARGS(ONE_PARAM)  /*checks for too many args */
            addrm = getAdressingMethod(dst);
            if(!isValidArgumentByAddrMethod(getDstAdressing(opcode) , addrm ))
                raiseAdressingMethodError("Invalid adressing mehtod for instruction", addrm)


            if ( addrm!= ERROR_ADRESSING && !firstpassErrorFlag)
                return codingInstructionWord(opcode,addrm , NO_PARAM);
            break;
        case TWO_PARAMS:

            src = strtok(NULL, ",");
            dst  = strtok(NULL, " \t");

            CHECK_TOO_MANY_ARGS(TWO_PARAMS);

            if (src  == NULL  || dst ==  NULL  || isspacestring(dst) || isspacestring(src)) /*src or dst havnt specified */
                raiseParserError("Wrong amount of parametes for instruction")

            else {
                adressing_method addrsrc, addrdst;

                /*get the src , dst adressing methods */
                addrdst = getAdressingMethod(dst);
                addrsrc = getAdressingMethod(src);

                if(!isValidArgumentByAddrMethod(getDstAdressing(opcode) , addrdst ))
                    raiseAdressingMethodError("Invalid adressing mehtod for instruction ", addrsrc)
                if(!isValidArgumentByAddrMethod(getSrcAdressing(opcode), addrsrc ))
                    raiseAdressingMethodError("Invalid adressing mehtod for instruction ", addrsrc)
                if (!firstpassErrorFlag &&  addrdst != ERROR_ADRESSING && addrsrc != ERROR_ADRESSING)
                    return codingInstructionWord(opcode,addrdst  , addrsrc );   /*if we had previous error we shouldnt encode in memory */

            }
            break;
        }
        return 1;

}



