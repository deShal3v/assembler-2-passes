#include "dataManagment.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


/*neccesary operations to set a new source file handling */
#define ADJUSTTONEW_SOURCEFILE(sizeinuse, counter){\
    if(counter == 0) /*this is a new source file */ \
        sizeinuse = 0;\
}

/*this number will hold the default increase rate for reallloc calls for the code/data dynamic arrays */
#define  INCREASEFACTOR 1.5

/*represent a number in two complement method by the number of bits
 using the known formula of 2**n -(-value) if negative*/
#define TWO_COMPLEMENT_REPRESENTATION(value , nofbits) ((value) >= 0)?(value):(int)(pow(2, nofbits) -(-(value)));


int DC, IC;  /* the counters */
extern char* filenamenoext;

static memword* datasection;
static memword* codesection;


/*Gets two bits value from the counter word at index a */
int getTwoBItsValue(int a, int counter, memword * section){

    int mask = (1 << (a +1 )) | (1<< a);
    int res = (section[counter].value &mask )>> a;
    return res;
}

/*Get two bits value of code */
int getTwoBItsValueCode(int a , int counter){

    return getTwoBItsValue(a , counter, codesection);
}

/*Get two bits value of data */
int getTwoBItsValueData(int a , int counter){

    return getTwoBItsValue(a , counter , datasection);
}


/*Allocator, allocate by an efficient increase factor 1.5 */
memword * Alloc(size_t sizetoencode, memword * section, int * sizeinuse, int counter){

    int sizegiven = *(int *)sizeinuse;

    if (*sizeinuse - counter * (signed int)sizeof(memword) <= (int)sizetoencode  || !*sizeinuse) { /*we need to allocate more space */
         *sizeinuse = (!*sizeinuse) ? sizetoencode : sizeof(memword) * ceil((INCREASEFACTOR * (*sizeinuse/sizeof(memword))));
        if (!sizegiven) /*we dont have any previous memory in use */
            section = (memword*)calloc(sizetoencode/ sizeof(memword),sizeof(memword));

        else{ /*extend the memory with reallocn , and zero the new part */

            *sizeinuse = (sizetoencode + sizegiven > *sizeinuse)?sizetoencode +sizegiven:*sizeinuse;
            section = (memword*)realloc((void *)section, (size_t)*sizeinuse);
            if(!section)
                return NULL;
            memset((void *)&(section[sizegiven/sizeof(memword)]), '\x00', *sizeinuse - sizegiven);
        }
    }

    return section; /*return a pointer to the new section */
}

/*Allocate sufficient data space */
memword * dataAlloc(size_t sizetoencode) {

    static int sizeinuse; /*Each time we will increase the size of the arrray and we must know how much space do we have now*/
    ADJUSTTONEW_SOURCEFILE(sizeinuse, DC);
    datasection = Alloc(sizetoencode * sizeof(memword), datasection, &sizeinuse, DC);
    return datasection;
}

/*Allocate sufficient code space */
memword * codeAlloc(size_t sizetoencode) {

    static int sizeinuse; /*Each time we will increase the size of the arrray and we must know how much space do we have now*/
    ADJUSTTONEW_SOURCEFILE(sizeinuse, IC);
    codesection = Alloc(sizetoencode * sizeof(memword), codesection, &sizeinuse, IC);
    return codesection;
}


/*encodes a code word into memory */
int encodeByWord(long value, memword * word, int are, int specindexstart, int specindexend) {

    int i, mask;
    int tmp = 0;
    /*representing the value to encode int two's complement method */
    int u = (int)TWO_COMPLEMENT_REPRESENTATION(value, specindexstart - specindexend + 1);
    mask= 1 << (specindexstart - specindexend);
    for (i = specindexstart; i >= specindexend; i--,mask >>= 1) {   /*iterating over each bit */
        tmp = (u & mask) << (specindexend);
        word-> value |= tmp;    /*adding the current bit */

    }
    /*now its time to encode the are field*/
    if (are != -1)	/*if its not encolded yet from another operand sharing the same word in memory*/
        word-> value |= are;

    return 1;
}

/*Encodes a code word into memory*/
int encodeCodeWord(long value, int are , int specindexstart, int specindexend, int allocateflag, int wordstoencode){


    if(allocateflag && !(codesection = codeAlloc((size_t) wordstoencode)))
        return 0;
    encodeByWord(value , &codesection[IC], are , specindexstart, specindexend);

    return 1;
}



/*Encodes data array in the data section of the program*/
void encodeData(long values[], int size) {

	int i;

    if (!(datasection = dataAlloc((size_t)size)))
		return; /*couldnt allocate the appropriate memory*/
	i = DC;
	while (i < DC + size ) {
		
        encodeByWord(values[i -DC], &datasection[i], -1, MAX_INDEX, 0);
		i++;
	}

	DC = i;

}

/*Encodes a parameter additional word of instruction */
int encodeParamWord(long value, int are , int specindexstart, int specindexend){

    int res =  encodeCodeWord(value , are , specindexstart, specindexend , ALLOCATE_FLAG_UNSET, 0);
    return res;
}

/*encodes string to the memory , and placing a NULL byte */
void encodeString(char * stringtoencode) {
	char* c;

	if (!(datasection = dataAlloc(strlen(stringtoencode) + 1)))
		return; /*couldnt allocate the appropriate memory*/
	
	c = stringtoencode;
    while (*c != '\x00') {  /*while we havnt reach the null terminator */
        encodeByWord(*c , &datasection[DC], -1 , MAX_INDEX, 0);
        DC++;   /*updating the data counter */
		c++;
	}

    /*now we have to encode the NULL byte (termination) */
    encodeByWord(0 , &datasection[DC], -1 , MAX_INDEX, 0);
    DC++;

}


/*Calculate the L param by the adressing methods were in use */
int calculateL(adressing_method addrdst , adressing_method addrsrc){
    int L =0;
    switch (addrsrc) {  /*adding  the src additional number of words  */

        case NO_PARAM:
            break;
        case BY_INDEX_ADRESSING:
            L+=2;
            break;

        case REG_ADDRESSING:
            if(addrdst != REG_ADDRESSING)
                L +=1;
            break;
        default:
            L+=1;
            break;
    }

    switch (addrdst) {  /*adding  the dst additional number of words  */

        case NO_PARAM:
            break;
        case BY_INDEX_ADRESSING:
            L+=2;
            break;
        default:
            L++;
            break;
     }


    return L;
}


/*coding an instruction's first word during the first pass, returns L*/
int codingInstructionWord(char opcode , adressing_method addrdst , adressing_method addrsrc ) {

	int L = 1;


	/*now we have to calculate the L param*/
    L += calculateL(addrdst, addrsrc);

    encodeCodeWord(opcode , ARE_ABSOLUTE, 9, 6, ALLOCATE_FLAG_SET, L);    /*encoding the opcode to the memory */
    if(addrsrc == NO_PARAM)
        addrsrc = 0; /* zero value for encoding */
    if(addrdst == NO_PARAM)
        addrdst = 0;

    encodeCodeWord(addrsrc, ARE_ABSOLUTE, 5,4, ALLOCATE_FLAG_UNSET, 0);  /*we dont have to allocate mmlore word for the instruction , just hardcode the params */
    encodeCodeWord(addrdst, ARE_ABSOLUTE, 3, 2, ALLOCATE_FLAG_UNSET, 0);

	IC += L;
	return L;
}

/*Frees the code and data structures were in use */
void freeCodeDataRegions(){
    free(codesection);  /*frees the code array */
    free(datasection);  /*frees the data array */

    /*updates the pointers for the next time we will use this module
     * maybe in another source file */

    datasection = NULL;
    codesection = NULL;
}



