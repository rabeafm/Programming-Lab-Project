#include <stdio.h>
#include <stdlib.h>
#include "passes.h"

/* Include header files */


/* Declaring Global Variables and Functions to be used */

int secondpass(char *line,int *IC,int *DC){
	/*char label[32],word[32];
	sscanf(line,"%s",word);*/
	/*if(isLabel(word)){
		sscanf(line,"%s %s",label ,word);
	}
	if(isData(word) || isString(word) || isExtern(word)){
		printf("Data / String / Extern \n");
		return 0;
	}
	if(isEntry(word)){
		printf("Entry \n"); */
		/* add to symbol table entry attribute to symbol
		   if symbol not in table give errror */
		/*return 0;
	}*/
	/* bincode rest of operand infowords, based on addressing
	   every operand with symbol  find symbol value from table 
	   and if not in table give error.
	   if has external attribute add infoword address tolist of externals 
	   i can use IC and L .*/
	return 1;
}


/* Implementation of second process Functions */
