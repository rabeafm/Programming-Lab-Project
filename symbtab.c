#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "symbtab.h"

#define MAX_RESERVED_WORDS 30
#define MAX_LENGTH_OF_RESERVED_WORDS 7

char RES_WORDS[MAX_RESERVED_WORDS][MAX_LENGTH_OF_RESERVED_WORDS] = {
	"mov", "cmp", "add", "sub",
	"lea", "clr", "not", "inc",
	"dec", "jmp", "bne", "jsr",
	"red", "prn", "rts", "stop",
	"r0", "r1", "r2", "r3",
	"r4", "r5", "r6", "r7",
	".data",".string",".entry",".extern"
};

/* if symbol is not legal label report error */
int isLabelLegal(int statement_cnt, char *label){
	int i=1;
	if(!isalpha(label[0])){
		printf("*** Error in line: %d, first character of label: %c is not a capital or small letter ***\n",statement_cnt,label[0]);
		return FALSE;
	}
	while(i<strlen(label)){
		if(!isalnum(label[i])){
			printf("*** Error in line: %d, the character of label: %c is not a capital or small letter or a digit ***\n",statement_cnt,label[i]);
			return FALSE;
		}
		i++;
	}
	i=0;
	while(i<MAX_RESERVED_WORDS){
		if(strcmp(label,RES_WORDS[i])==0){
			printf("*** Error in line: %d, the label %s is a reserved word ***\n",statement_cnt,label);
			return FALSE;
		}	
		i++;
	}
	return TRUE;
}

int add_symbol(int statement_cnt,char *label, int *DC, char *word, Tlinkptr *head, int *error_flag){
	Tlinkptr runner = *head;
	int label_len = strlen(label);
	if(label[label_len-1]==':') {
		label[label_len-1]='\0';
		label_len--;
	}

	if(!isLabelLegal(statement_cnt,label)){
		(*error_flag)=3;
		return 0;
	}
		
	
	/* if Table is empty add label to head */
	if(!(*head)){
		*head = (Tlinkptr) malloc(sizeof(Tlink));
		strcpy((**head).symbol,label);
		if(!strcmp(word,".data") || !strcmp(word,".string")){/*  add operand symbol to symbol table with value DC and attribute data*/
			(**head).value=(*DC);
			(**head).is_data=1; 
		}
		if(!strcmp(word,".extern")){/*  add operand symbol to symbol table with value 0 and attribute external*/
			(**head).value=0;
			(**head).is_extern=1;
		}
		if(!strcmp(word,"code")){
			(**head).value=(*DC);
			(**head).is_code=1; 
		}
		printf("Added to symbol table: %s code: %d data: %d extern: %d value: %d \n",(*head)->symbol,(*head)->is_code,(*head)->is_data,(*head)->is_extern, (*head)->value);
		return 1;
	}
	if(strcmp((*runner).symbol,label)==0){
		printf("\t\t\t\t\t\t\t Symbol %s is already in symbol table", label);
		(*error_flag)=3;
		if(!strcmp(word,".extern") && (*runner).is_extern==0)
			printf(", and doesnt have attribute external");
		printf("\n");
		/* if symbol in list without external attribute report error */
		return 0;
	}
	while((*runner).next){
		runner = (*runner).next;
		if(strcmp((*runner).symbol,label)==0){
			printf("\t\t\t\t\t\t\t Symbol %s is already in symbol table", label);
			(*error_flag)=3;
			if(!strcmp(word,".extern") && (*runner).is_extern==0)
			printf(", and doesnt have attribute external");
			printf("\n");
			return 0;
		}
	}
	(*runner).next = (Tlinkptr) malloc(sizeof(Tlink));
	strcpy((*((*runner).next)).symbol,label);
	if(!strcmp(word,".data") || !strcmp(word,".string")){
		(*((*runner).next)).value=(*DC);
		(*((*runner).next)).is_data=1; 
	}
	if(!strcmp(word,".extern")){
		(*((*runner).next)).value=0;
		(*((*runner).next)).is_extern=1;
	}
	if(!strcmp(word,"code")){
		(*((*runner).next)).value=(*DC);
		(*((*runner).next)).is_code=1; 
	}
	printf("Added to symbol table: %s code: %d data: %d extern: %d value: %d \n",((*runner).next)->symbol,((*runner).next)->is_code,((*runner).next)->is_data,((*runner).next)->is_extern, ((*runner).next)->value);
	return 1;	
}

Tlinkptr get_symbol(char symbol[],Tlinkptr *head){
	Tlinkptr runner = *head;
	while((*runner).next){
		if(strcmp((*runner).symbol,symbol)==0)
			return runner;
		runner = (*runner).next;
	}
	return NULL;
}

void update_data_symbols(int ICF,Tlinkptr *head){
	Tlinkptr runner = *head;
	while((*runner).next){
		if((*runner).is_data==1)
			(*runner).value+=ICF;
		runner = (*runner).next;
	}
}
