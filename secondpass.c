#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "passes.h"
#include "symbtab.h"

int secondpass(char *statement,int statement_cnt,int *IC,int *DC,Tlinkptr *head,Operand DATA_IMAGE[],MachineOrder CODE_IMAGE[],int *error_flag){
	char word[32]="\0",srcoper[32]="\0",distoper[32]="\0",comma[32]="\0";
	Tlinkptr runner;
	
	sscanf(statement,"%s %s %s %s", word, srcoper, distoper,comma);

	/*	Checking for Blank or Comment Lines - Should Ignore Them  */
	if(isBlank(word) || isComment(word)){
		return NOCHANGE;
	}
	/* if first word is Label if it is mark the label flag and put the data in the right variables */ 
	if(isLabel(word)){
		sscanf(statement+strlen(word),"%s %s %s %s", word, srcoper, distoper,comma);
	}
	
	/* if data / string / extern ignore */
	if(isData(word) || isString(word) || isExtern(word)){
		return NOCHANGE;
	}
	if(isEntry(word)){
		printf("Entry %s operand %s\n",word,srcoper);
		runner=get_symbol(srcoper,head);
		if(runner){
			(*runner).is_entry=1;
			return CHANGED;
		} else {
			printf("*** Error in line: %d - the symbol %s was not found in the symbol table ***\n",statement_cnt,srcoper);
			(*error_flag)=3;
			return NOCHANGE;
		}
	}
	/* bincode rest of operand infowords, based on addressing
	   every operand with symbol  find symbol value from table 
	   and if not in table give error.
	   if has external attribute add infoword address tolist of externals 
	   i can use IC and L .*/
	makeSecondBinary(statement_cnt,CODE_IMAGE,DATA_IMAGE,*IC,word,srcoper,distoper,comma,head,error_flag);


	
	return 1;
}


/* Implementation of second process Functions */
