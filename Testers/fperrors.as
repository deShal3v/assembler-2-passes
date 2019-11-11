
;This is the errors tester for the first pass , line is too big  ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddf	ddddd

mov r2 , r3 externous
mov r2 , #2

;macro with too big value
.define macrotoobig = 1444444443334443343443

mov #macro_not_defined , LIST[2]
.define macro_not_defined =  4
invallid arg1_might_be_invalid , arg2 
#this is an invalid comment 

STRING_WITHOUT_BRACKETS:	.string abcdefghijk"


3ompare: cmp #4096, r2
 ;should be out of range value, cannot be represted in 12 bits  
	
	bne W 	
	;shouldnt complain on this in the first pass
clr #-4096 
;value is out of range again 

.data 6, -9 , 4096 , 16384
;outofrange too, but now the range is of 14 bits instead of 12 as we had earlier 

;macro withour a '=' sign 	
.define macro 4


.define macro ==4

;multiple declarations of a symbol
.define SYM = 4
SYM: .string "abcdefg"

;Invlalid label to macro declaration
LABELFORMACRO: .define macro3 = 100

stop

;checking Two labels in a row 

Label1: Label2: mov r1 ,#2

;I chose label for extern to be ok
LABELfor3xtern: 	.extern WORd

.data -3 , ++56  

.data 0

.string "abcde


add #2 ,#4

;Label for an .entry command 

L9: .data 1
LABELFORENTRY: .entry L9


;end of source file 
