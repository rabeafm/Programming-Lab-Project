#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symbtab.h"
#include "utils.h"

int addExternSymbol(char *label, int DC, Tlinkptr *head, int *error_flag){
	Tlinkptr runner = *head;

	int label_len = strlen(label);
	if(label[label_len-1]==':') {
		label[label_len-1]='\0';
		label_len--;
	}
		
	/* if Table is empty add label to head */
	if(!(*head)){
		*head = (Tlinkptr) malloc(sizeof(Tlink));
		strcpy((**head).symbol,label);
		(**head).value=DC;
		(**head).is_extern=1;
		printf("Added to Extern table: %s code: %d data: %d extern: %d value: %d \n",(*head)->symbol,(*head)->is_code,(*head)->is_data,(*head)->is_extern, (*head)->value);
		return 1;
	}
	while((*runner).next){
		runner = (*runner).next;
	}
	(*runner).next = (Tlinkptr) malloc(sizeof(Tlink));
	strcpy((*((*runner).next)).symbol,label);
	(*((*runner).next)).value=DC;
	(*((*runner).next)).is_extern=1;
	printf("Added to Extern table: %s code: %d data: %d extern: %d value: %d \n",((*runner).next)->symbol,((*runner).next)->is_code,((*runner).next)->is_data,((*runner).next)->is_extern, ((*runner).next)->value);
	return 1;	
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
	while(runner){
		if(strcmp((*runner).symbol,symbol)==0)
			return runner;
		runner = (*runner).next;
	}
	return NULL;

}


/*	Updating final counters and Symbol Table links using +ICF */
void updateDataSymbols(int ICF,Tlinkptr *head){
	Tlinkptr runner = *head;
	while(runner){
		if((*runner).is_data==1)
			(*runner).value+=ICF;
		runner = (*runner).next;
	}
}

void printTable(Tlinkptr r){
	if(r){
		printf("Symbol: %s Value: %d Code: %d Data: %d entry: %d extern: %d Next: %s \n",(*r).symbol,(*r).value,(*r).is_code,(*r).is_data,(*r).is_entry,(*r).is_extern,(*((*r).next)).symbol);
		printTable(r->next);
	}
}

void freeTable(Tlinkptr runner){
	Tlinkptr tmp;
	while (runner){
       tmp = runner;
       runner = runner->next;
       free(tmp);
    }
}

