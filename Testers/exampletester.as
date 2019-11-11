;This tester is valid and should create output files
.extern L1
.define macro1 =	1

mov r1 , L1[macro1]
lea L1 , L2[macro1]

prn #-2
prn	#+2

sub L1[1], L2
not r2

clr r7

LOOP:	mov L2[2], r7
	inc r7
	mov r7, L2[2]
	cmp r2 , #2
	bne LOOP
	

L2: .data 4,4,5,-22

.entry L2

;end of source file
