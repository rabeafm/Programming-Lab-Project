assembler: assemble.o firstpass.o secondpass.o symbtab.o binary.o
	gcc -g -ansi -Wall -pedantic o/assemble.o o/firstpass.o o/secondpass.o o/symbtab.o o/binary.o -o assembler
assemble.o: assemble.c 
	gcc -c -ansi -Wall -pedantic assemble.c -o o/assemble.o
firstpass.o: firstpass.c passes.h
	gcc -c -ansi -Wall -pedantic firstpass.c -o o/firstpass.o
secondpass.o: secondpass.c passes.h
	gcc -c -ansi -Wall -pedantic secondpass.c -o o/secondpass.o
symbtab.o: symbtab.c symbtab.h
	gcc -c -ansi -Wall -pedantic symbtab.c -o o/symbtab.o
binary.o: binary.c passes.h
	gcc -c -ansi -Wall -pedantic binary.c -o o/binary.o
