#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "passes.h"
#include "symbtab.h"
#include "dict.h"

OperatorDict OpTable[]={
	/* Order     Funct  OpCode		Order     Funct  OpCode		 Order     Funct  OpCode */
	{  "mov" , {0 , 0 , FMOV , MOV }}, {  "cmp" , {0 , 0 , FCMP , CMP }}, {  "add" , {0 , 0 , FADD , ADD }},
	{  "sub" , {0 , 0 , FSUB , SUB }}, {  "lea" , {0 , 0 , FLEA , LEA }}, {  "clr" , {0 , 0 , FCLR , CLR }},
	{  "not" , {0 , 0 , FNOT , NOT }}, {  "inc" , {0 , 0 , FINC , INC }}, {  "dec" , {0 , 0 , FDEC , DEC }},
	{  "jmp" , {0 , 0 , FJMP , JMP }}, {  "bne" , {0 , 0 , FBNE , BNE }}, {  "jsr" , {0 , 0 , FJSR , JSR }},
	{  "red" , {0 , 0 , FRED , RED }}, {  "prn" , {0 , 0 , FPRN , PRN }}, {  "rts" , {0 , 0 , FRTS , RTS }},
	{  "stop", {0 , 0 , FSTOP, STOP }}
};
/* Declaring Global Variables and Functions to be used */
int Lookup(word){
	int i;
	for(i=0;i<16;i++)
		return 0;

	return 1;
}
int firstbincode(){
	return 0;
}

/* Implementation of first process Functions */
int firstpass(char *statement,int statement_cnt,int *IC,int *DC,Tlinkptr *head){
	char label[32]="\0",word[32]="\0",srcoper[10]="\0",trgoper[10]="\0";
	int label_flag = 0;
	int error_flag = 0;
	int L=0;
	
	/*	Checking for Blank or Comment Lines - Should Ignore Them  */
	sscanf(statement,"%s:",word);
	if(isBlank(word) || isComment(word)){
		return NOCHANGE;
	}

	/* Check if the first word is a Label if it is mark the label flag and put the data in the right variables */ 
	if(isLabel(word)){
		label_flag=1;			/* Turn On Symbol Definition */
		L=sscanf(statement,"%s %s %s %s", label, word, srcoper, trgoper)-1; /* Correct Input and update L for possible later use */
	} else {
		L=sscanf(statement,"%s %s %s", word, srcoper, trgoper);
	}
	
	/* Check if it is a data or string storage instruction */ 
	/* Identify data type code in data image and DC+(length of data) */
	if(isData(word)) {
		if(label_flag==1)
			add_symbol(statement_cnt,label,DC,word,head,&error_flag);
		if(strstr(statement,","))
			(*DC)++;
		(*DC)++;
		return CHANGED;
	}
	if(isString(word)){
		if(label_flag==1)
			add_symbol(statement_cnt,label,DC,word,head,&error_flag);
		(*DC)+=(strlen(srcoper)-1);
		return CHANGED;
	}


	
	if(isEntry(word)){
		printf("Entry\n");
		return 0;
	}
	if(isExtern(word)){
		printf("Extern\n");
		sscanf(statement,"%s %s",word,label); /* Make label second */
		add_symbol(statement_cnt,label,DC,word,head,&error_flag);
		return 0;
	}
	/*		Then it is an order statement		*/
	if(label_flag==1){
		add_symbol(statement_cnt,label,IC,"code",head,&error_flag);
	}
	/*if(!Lookup(word)){
		printf("Operator %s doesnt exist check spelling\n",word);
		error_flag = 3;
		return 0;
	}*/
/* lookup operator in operator list if doesnt exist report error
		Analayze operand structure and check and calculate number of words needed ->L */

	firstbincode(*IC,L,word,srcoper,trgoper);/* bincode first word and every infoword in meyade immediate addressing*/
											   /*add to machine code with IC and L*/
	(*IC)+=L;	/*update IC=IC+L*/

	printf("\t\t\t\t\t %d \t %s \t %s \t %s \n",L,word,srcoper,trgoper);
	if (error_flag==3)
		return 3;
	return 0;
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
