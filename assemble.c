		/*	Main function is after the description	*/

/*--------------------------------------------------------------*
 *		 		Introduction to the Assembler	   				*
 *--------------------------------------------------------------*
 * @author Rabea Fahoum											*
 * This project aims to immitates one of every computer	most 	*
 * common programs, which is an Assembler. This project was 	*
 * built as	a final homework for the academic course in the 	*
 * open university learned in the fall of 2021:					*
 * 																*
 *	 		# 20465 - Laboratory in Systems Programming #		* 
 * 																*
 * The Assembler (written in ANSI C90) is made for a special 	*
 * assembly language defined for the project. The assembler's 	*
 * role is to create a file of machine language from a file		*
 * written in the assembly language as the first step in a 		*
 * program cycle towards getting the code to run on a computer.	*
 * The next stages which are the linkage and loading are not 	*
 * part of this program.										*
 * 																*
 * # The project includes the following files:					*
 * 1- Source files of the program (.c & .h):					*
 * 		a.	assemble.c	- Main File								*
 * 		b.	assemble.h	- Main Functions Declaration			*
 * 		c.	binary.c	- Binary Translation Functions			*
 *		d.	data.h		- Data Structures						*
 * 		e.	files.c		- File Handling Functions				*
 * 		f.	passes.c	- First Pass & Second Pass Functions	*
 * 		g.	symtab.h	- Symbol Table Structure & Declaration  *
 * 		h.	symtab.h	- Symbol Table Functions				*
 * 		i.	utils.h		- Utilities Macros & Declarations		*
 * 		j.	utils.c		- Utilities Functions					*
 * 		k.	values.h	- Assembly Language Binary Values		*
 * 2- Runtime file for Ubuntu (Compiled and linked).			*
 * 3- Makefile (Compiled with gcc with the flags -Wall -ansi 	*
 * 	  -pedantic, with no messages or warning from the compiler).*
 * 4- Runtime Examples:											*
 *	a.	Input in Assembly, Output as a result of the using the	*
 * 		assembler on the input files, showing use of different	*
 *		orders and data structures.								*
 * 	b. 	Input in Assembly Displaying various error types (with	*
 * 		no output files) and screenshots showing the errors.	*
 *																*
 *--------------------------------------------------------------*/

#include <stdio.h>
#include "assemble.h"

/** ------------------------------------------------------------*
 *  Recieves an array of string from the command line handles 	*
 *  them one by one calling the read file function and if read 	*
 *  was successful calls the write files function				*
 *--------------------------------------------------------------*/
int main(int argc, char *argv[]) {
	while (*++argv){
		if(readFile(*argv)){
			writeFiles(*argv);
		}
	}
	return 1;
}