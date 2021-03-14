assembler: assemble.o
	gcc -g -ansi -Wall -pedantic assemble.o -o assembler


assemble.o: assemble.c
	gcc -c -ansi -Wall -pedantic assemble.c -o assemble.o
