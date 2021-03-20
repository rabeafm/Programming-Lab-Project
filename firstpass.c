#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "passes.h"
#include "symbtab.h"
#include "dict.h"




/* Implementation of first process Functions */
int firstpass(char *statement,int statement_cnt,int *IC,int *DC,Tlinkptr *head,Operand DATA_IMAGE[],MachineOrder CODE_IMAGE[],int *error_flag){
	char label[32]="\0",word[32]="\0",srcoper[32]="\0",distoper[32]="\0",comma[32]="\0",tmi[32]="\0";
	int label_length = 0;
	int L=0;
	
	/*	Checking for Blank or Comment Lines - Should Ignore Them  */
	sscanf(statement,"%s:",word);
	if(isBlank(word) || isComment(word)){
		return NOCHANGE;
	}

	/* Check if the first word is a Label if it is mark the label flag and put the data in the right variables */ 
	if(isLabel(word)){
		sscanf(statement,"%s %s %s %s %s %s", label, word, srcoper, distoper,comma,tmi);
		label_length=strlen(label);			/* Turn On label flag */
	} else {
		sscanf(statement,"%s %s %s %s %s", word, srcoper, distoper,comma,tmi);
	}
	
	/* Check if it is a data or string storage instruction and call add to data image */ 
	if(isData(word)) {
		if(label_length)
			add_symbol(statement_cnt,label,DC,word,head,error_flag);
		return add_data_to_image(statement_cnt,strstr(statement,".data")+5,DATA_IMAGE,DC,error_flag);
	}
	if(isString(word)){
		if(label_length)
			add_symbol(statement_cnt,label,DC,word,head,error_flag);
		return add_string_to_image(statement_cnt,strstr(statement,".string")+7,DATA_IMAGE,DC,error_flag);
	}

	/* Ignore Enrty*/
	if(isEntry(word)){
		if(label_length)
			printf("*** Warning in line: %d, the label %s defined before .entry has no meaning and will be ignored. ***\n",statement_cnt,label);
		return NOCHANGE;
	}

	/* Handle Extern - should check symbol table issues*/
	if(isExtern(word)){
		if(label_length){
			printf("*** Warning in line: %d, the label %s defined before .extern has no meaning and will be ignored. ***\n",statement_cnt,label);
			sscanf(statement,"%s %s %s",srcoper,word,label); /* Make the second the label */
		} else { 
			sscanf(statement,"%s %s",word,label); 
		}
		add_symbol(statement_cnt,label,DC,word,head,error_flag);
		return 0;
	}

	/*		Then it is an order statement		*/
	if(label_length){
		add_symbol(statement_cnt,label,IC,"code",head,error_flag);
	}


	printf("%s --- %s --- %s --- %s --- %s --- %s \n",label,word,srcoper,distoper,comma ,tmi);
	/*	too many operands warning */
	if(strcmp(tmi,"\0")!=0){
		printf("*** Error in line: %d, too many operands (Maximum number of operands is two) ***\n",statement_cnt);
		*error_flag = 3;
		return NOCHANGE;
	}

	/* lookup operator in operator list if doesnt exist report error*/
	if(LookupinDict(word)==-1){
		printf("*** Error in line: %d, illegal command %s check the spelling (commands must be lowercase)  ***\n",statement_cnt,word);
		*error_flag = 3;
		return NOCHANGE;
	}
	/* Analayze operand structure and check and calculate number of words needed ->L */
	L=analayzeStatement(statement_cnt,word,srcoper,distoper,comma,error_flag);

	if(*error_flag!=3) 
		makeFirstBinary(CODE_IMAGE, statement_cnt,*IC,L,word,srcoper,distoper,comma,error_flag);
	(*IC)+=L;	/*update IC=IC+L*/
	return CHANGED;
}

int isBlank(char *word){
	if(word[0]=='\0')
		return TRUE;
	return FALSE;
}

int isComment(char *word){
	if(word[0]==';')
		return TRUE;
	return FALSE;
}

int isLabel(char *word){
	if(word[strlen(word)-1]==':')
		return TRUE;
	return FALSE;
}

int isData(char *word){
	if(strcmp(word,".data")==0)
		return TRUE;
	return FALSE;
}

int isString(char *word){
	if(strcmp(word,".string")==0)
		return TRUE;
	return FALSE;
}

int isEntry(char *word){
	if(strcmp(word,".entry")==0)
		return TRUE;
	return FALSE;
}

int isExtern(char *word){
	if(strcmp(word,".extern")==0)
		return TRUE;
	return FALSE;
}

/* 4 statement types (statements are seperated by '\n' 
whitespace ' ' or '\t' or none
comments   first char is ';'
guiding    memory allocation,init variables no machine orders for runtime
	in begining maybe there is a label (optional)
	name of guidance and parameters (starts with . then small letters)
		1-.data Z numbers with 1, -5 \t,+2  ,... (no 2, no , in  
			begining or end) for (data image) then DC++
			if there is a label the the label=DC before ++
			and label is added to symbol table
		2-.string get one parameter (legal string) by ascii and added
			to data image by order each to a word and then '\0'
			DC++ accordingly if there is a label the the label=DC
			before ++ and label is added to symbol table
		3-.entry get one parameter a defined label name in this file
			allowing for assembly code in other files to use it
			as operand.
			if label is before entry in same line ignore & warn
		4-.extern gets on parameter a name of undefined label in this
			file anouncing label is in other file and that this
			file is using the label
			if label is before extern in same line ignore & warn
			label cant be entry and extern in same file
order	   machine orders for runtime made of order name and operands
		has optional label, operation and operands
		if label then send to symbol table and its value is address of 			first word of the order in code image
		seperation is by ' ' or '\t' and between operands with ',' and 	
		maybe ' ' or '\t'	*/

/*	Labels: in the begining of order statements or .data or .string starts
		with alpha then alpha or digits ends with : no spaces before
		(only in definition) which is not part of it (max length 31 chars)
		cant be defined twice (hello and Hello are different)
		special words cant be labels such as r0 or add, R3 and ADD are ok
		label with .data or .string gets DC value
		label with order gets IC value
	Number: optional (+\-)
	String: mahrouzet ascii with ""



*/
