assembler: assemble.o binary.o files.o passes.o symbtab.o utils.o
	gcc -g -ansi -Wall -pedantic o/assemble.o o/binary.o o/files.o o/passes.o o/symbtab.o o/utils.o -o assembler
assemble.o: assemble.c
	gcc -c -ansi -Wall -pedantic assemble.c -o o/assemble.o
binary.o: binary.c
	gcc -c -ansi -Wall -pedantic binary.c -o o/binary.o
files.o: files.c files.h
	gcc -c -ansi -Wall -pedantic files.c -o o/files.o
passes.o: passes.c
	gcc -c -ansi -Wall -pedantic passes.c -o o/passes.o
symbtab.o: symbtab.c symbtab.h
	gcc -c -ansi -Wall -pedantic symbtab.c -o o/symbtab.o
utils.o: utils.c utils.h
	gcc -c -ansi -Wall -pedantic utils.c -o o/utils.o