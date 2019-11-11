assembler: main.o  dataStructures.o  lineAnalyzing.o  moduleAnalyzing.o secondpass.o firstPass.o outputfiles.o dataManagment.o instructionsSet.o 
	gcc -g -Wall -ansi -pedantic main.o  secondpass.o dataStructures.o moduleAnalyzing.o  firstPass.o dataManagment.o lineAnalyzing.o outputfiles.o instructionsSet.o -o assembler -lm

main.o: main.c	 moduleanalyzing.h
	gcc -Wall -ansi -pedantic -c  main.c -o main.o


lineAnalyzing.o:lineAnalyzing.c dataStructures.h lineanalyzing.h moduleanalyzing.h  instructionsset.h datamanagmentfirstpass.h errorshandling.h
	gcc -Wall -ansi -pedantic -c lineAnalyzing.c -o lineAnalyzing.o


dataStructures.o: dataStructures.c dataStructures.h  errorshandling.h
	gcc -Wall -ansi -pedantic -c  dataStructures.c -o dataStructures.o


dataManagment.o: dataManagment.c dataManagment.h 
	gcc -Wall -ansi -pedantic -c  dataManagment.c -lm -o dataManagment.o


secondpass.o: secondpass.c lineanalyzing.h dataStructures.h outputfiles.h datamanagmentsecondpass.h errorshandling.h 
	gcc -Wall -ansi -pedantic -c  secondpass.c -o secondpass.o


outputfiles.o: outputfiles.c dataManagment.h outputfiles.h
	gcc -Wall -ansi -pedantic -c  outputfiles.c -o outputfiles.o


instructionsSet.o:  instructionsSet.c instructionsset.h
	gcc -Wall -ansi -pedantic -c  instructionsSet.c -o instructionsSet.o


firstPass.o: firstpass.c datamanagmentfirstpass.h dataStructures.h  firstpass.h  lineanalyzing.h
	gcc -Wall -ansi -pedantic -c  firstpass.c -o firstPass.o

moduleAnalyzing.o: moduleAnalyzing.c moduleanalyzing.h firstpass.h secondpass.h outputfiles.h
	gcc -Wall -ansi -pedantic -c moduleAnalyzing.c -o moduleAnalyzing.o
