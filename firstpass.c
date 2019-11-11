/*Module handles the first pass operations and function calls */

#include "datamanagmentfirstpass.h"
#include "dataStructures.h"
#include "lineanalyzing.h"
#include "firstpass.h"
#include <string.h>


/*This global will hold the errors state of the first pass */
int firstpassErrorFlag;


/*first pass parser function
 * responsible for determine the command type
*/
LINE_STATE parserFunction(char* token,int * labeldecFlag, char * label) {


    switch (typetoIndex(token)) {   /*analyzing by command type */

        case CMD_MACRO:	/*This is a macro declaration*/
            analyzeMacroLine(*labeldecFlag);
            break;
        case CMD_STRING : /*string command */
            if (*labeldecFlag == 1 && label != NULL) 	/*Adding this label as data*/

                 firstpassErrorFlag |= 1 ^ pushToSymbolTable(label, DATA, DC);


            /*now we have to update the DC and coding the string in the program memory*/
           analyzeStringLine();

            break;
        case CMD_DATA:  /*data command */
            if (*labeldecFlag == 1 && label != NULL) {	/*Adding this label as data*/

                 firstpassErrorFlag |= 1 ^ pushToSymbolTable(label, DATA, DC);
            }
            /*now we have to update the DC and coding the string in the program memory*/
            analyzeDataLine();
            break;
        case CMD_ENTRY: /*we shouldnt take care of an entry command during the first pass */
            fpAnalyzeEntryLine(*labeldecFlag);
            break;
        case CMD_EXTERN:    /*external command */
        {
            char external_name[MAX_LINE_SIZE];

            memset(external_name, '\x00', MAX_LINE_SIZE);
            if (analyzeExternLine(external_name))
                 firstpassErrorFlag |= 1 ^ pushToSymbolTable(external_name, EXTERNAL, 0);	/* Push it to the symbol table with irrelevant value*/
            break;
        }
        default:
            if (isLabelDeclaration(token)) { /*This is a lable declaration*/
                stripLabel(token)   /*strips the label from the : sign */
                if (isValidLabel(token, 0)) {
                    if(*labeldecFlag == 1)  /*we already had a label in this line */
                        multipleLabelsError();
                    *labeldecFlag  = 1; /*set the labeldeclaration flag for this line*/
                    return NOT_FINISHED_THIS_LINE;
                }
                break;
            }

            else {		/*This is a code declaration */
                if (*labeldecFlag == 1 && label != NULL)
                     firstpassErrorFlag |= 1 ^ pushToSymbolTable(label, CODE, IC + 100);
                analyzeCodeLine(token);
            }
            break;

    }

    return FINISHED_THIS_LINE;  /*finished to parse this line  */

}


/*first pass handling function for managing the diffent operations */
int firstPass(FILE* fd) {


    char lineread[MAX_LINE_SIZE];
    char* token, * n, * tmptoken;
    int labeldecFlag = 0;
    firstpassErrorFlag = 0;	/*This global will hold the first pass error status*/
    IC = DC = 0; /*setting up the instruction counter */

    LC = 0;     /*zero the line counter */
    while (getLineCheckLen(lineread, MAX_LINE_SIZE, fd)) {

        if ( !(n = removeInitailSpaces(lineread)) || n[0] == ';') 	/*First we will check if this is a blank line/comment */
            continue;

        token = strtok(n, "\t\n \r");
        labeldecFlag = 0;
        if (!parserFunction(token, &labeldecFlag, NULL)) { /*We havent finish the parsing for this line*/
            tmptoken = strtok(NULL, "\t\n\r ");
            if(tmptoken != NULL){
                sscanf(tmptoken, "%s", tmptoken);
                (void)parserFunction(tmptoken, &labeldecFlag, token);
            }
            else
                noLineContent();    /*We dont have any content after the opening label */

        }

    }

    /*This is the end of the source file gave to us*/

    if (firstpassErrorFlag)

        /*We got some errors during the first pass , exiting */
        return 0; /*function failed*/


    /*now we have to update each data symbol within the symbol table by adding IC + 100 */
    updateEachDataSymbol();
    return 1;


}


