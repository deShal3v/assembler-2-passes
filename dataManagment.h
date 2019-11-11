
#ifndef DATAMANAGMENT_H
#define DATAMANAGMENT_H


    typedef struct {    /*represents a word in memory */
        unsigned int value : 14;
    }memword;

    #define TOTAL_NUM_OF_BITS 14
    #define TOTAL_NUM_OF_BITS_WITH_ARE 12

    /*declaring some adressing methods using enum*/
    typedef enum {ERROR_ADRESSING = -1,NO_PARAM   = -1 ,IMM_ADDRESSING, STRAIGHT_ADDRESSING, BY_INDEX_ADRESSING , REG_ADDRESSING} adressing_method;
    enum{SRC_ADRESSING_OFFSET = 4 , DST_ADRESSING_OFFSET = 2};	/*the offsets inside the buffer we will read*/
    enum {ARE_ABSOLUTE, ARE_EXTERNAL , ARE_RELOCATABLE};
    enum { MAX_INDEX = 13, MIN_INDEX = 2, SRC_INDEX =7 , DST_INDEX  = 4, END_SRC_INDEX = 5};	/*holds the operands indexes inside the object file*/
    /*this will hold a potential flag indicated if we should allocate more memory for our coding process
      for the first pass the flag will be set
      for the second pass we dont have to handle these things
     */
    enum{ ALLOCATE_FLAG_UNSET, ALLOCATE_FLAG_SET} ;

    /*Get the current instruction word value by index and counter*/
    int getTwoBItsValueCode(int, int);

    /*Get the current instruction data value by index */
    int getTwoBItsValueData(int , int);


    extern int IC, DC;  /*extern the instruction and data counters whereever wee need an access to the data structures */



#endif /* DATAMANAGMENTFIRS */


