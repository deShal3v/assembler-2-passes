/*Module to interact with the symbol table  , contains functions, and declarations of variables */

#include "dataStructures.h"
#include "errorshandling.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern int IC ,DC;

/*Type constains a symbol node in the linked list
 should be private because its contains internal implementation*/
typedef struct symbolnode {

    SYMBOL symbol;
    struct symbolnode * next;
}SYMBOLNODE;


/*static ptrs for the symbol table */
static SYMBOLNODE* head = NULL;
static SYMBOLNODE* last = NULL;

/*checks for an existing symbol with the specified name */
int checkForExistingSym(char* name) {

    SYMBOLNODE* tmp = head;

    while (tmp != NULL) {
        if (strcmp(tmp->symbol.name, name) == 0) {	/*There is an exisitng one*/
            extern int LC;
            raiseSymError(name)
            return 1;
        }

        tmp = tmp->next;
    }

    return 0;
}

long getExistingSymbol(char* name, int * type) {


	SYMBOLNODE* tmp = head;

	while (tmp != NULL) {
		if (strcmp(tmp->symbol.name, name) == 0) {	/*There is an exisitng one*/
			if(type != NULL)
				*type = tmp->symbol.type;	/*updating its type */
			return tmp->symbol.value;

		}
		tmp = tmp->next;
	}

    return -1;
}

/*checks for an existing macro in the symbol table */
int checkForExistingMacro(char* name, long * status) {


	SYMBOLNODE* tmp = head;

	while (tmp != NULL) {
		if (strcmp(tmp->symbol.name, name) == 0 && tmp->symbol.type  == MACRO) {	/*There is an exisitng one*/
            if(status !=NULL)
                *status = tmp ->symbol.value;
			return 1;
		}

		tmp = tmp->next;
	}

	return 0;
}

/*Pushes a new symbol to the table*/
int pushToSymbolTable(char* name, SYMBOL_TYPE type, long value) {

	SYMBOLNODE* newsym;
	/*Checking is there is already a sym with this name*/
    if(checkForExistingSym(name))
        return 0;

    /*Lets generate a new symbol first*/


    newsym = (SYMBOLNODE*)malloc(sizeof(SYMBOLNODE));
    strcpy(newsym->symbol.name, name);
    newsym->symbol.type = type;
    newsym->symbol.value = value;


    newsym->next = NULL;

    if (head == NULL) {
        head = newsym;
        last = newsym;
    }
    else
        last->next = newsym;
    last = newsym;

    return 1;
} 

/*Iterating over the table and updating each data symbol value in the end of the first pass*/
void updateEachDataSymbol(){

    SYMBOLNODE* tmp = head;

    while (tmp != NULL) {

        if(tmp ->symbol.type == DATA)
            tmp->symbol.value += (IC + CODING_REGION_START);
        tmp  = tmp ->next;
    }
}
/*frees the symbol table */
void freeSymbolTable(){
    SYMBOLNODE *  tmp, * current = head;

    while(current != NULL){
        tmp = current ->next;
        free(current);  /*frees the current node */
        current = tmp;
    }

    head  = NULL;
}




