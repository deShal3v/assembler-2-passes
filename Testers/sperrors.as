;This file demonstrates errors detection and handling during the second pass 


;Labels are not exist
mov r1 , NOTEXIST[2]

.entry NOTEXIST

.entry NON  

;entry for non initailized (in current source file) label 

.extern L1

.entry L1

;Invalid entry argument (macro), not a  label
.define macro = 8
.entry macro

