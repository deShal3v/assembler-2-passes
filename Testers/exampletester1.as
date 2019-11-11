
; Tester code 1
; should create the output files 
.define sz			 =  	 	 	 	 2	
STR: .string "   a"
DATAARR: .data 2,2,2,3
CODE:  mov r1,r3
	sub r0,DATAARR[sz]
	mov #3, r2 
	mov #2 , r1
	cmp r2 , W1[1]
	inc r1
	dec r2
	mov r2 , W1[2]
.extern W1
.entry STR
