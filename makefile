assembler: assemble.o binary.o files.o passes.o symbtab.o utils.o
	gcc -g -ansi -Wall -pedantic assemble.o binary.o files.o passes.o symbtab.o utils.o -o assembler
assemble.o: assemble.c assemble.h
	gcc -c -ansi -Wall -pedantic assemble.c -o assemble.o
binary.o: binary.c assemble.h data.h symbtab.h utils.h values.h
	gcc -c -ansi -Wall -pedantic binary.c -o binary.o
files.o: files.c assemble.h data.h symbtab.h utils.h
	gcc -c -ansi -Wall -pedantic files.c -o files.o
passes.o: passes.c assemble.h data.h symbtab.h utils.h
	gcc -c -ansi -Wall -pedantic passes.c -o passes.o
symbtab.o: symbtab.c symbtab.h utils.h
	gcc -c -ansi -Wall -pedantic symbtab.c -o symbtab.o
utils.o: utils.c utils.h
	gcc -c -ansi -Wall -pedantic utils.c -o utils.o