#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symbtab.h"
#include "utils.h"

/** ------------------------------------------------------------*
 *  Recieves a label, checks if legal and adds to symbol table.	*
 *  															*
 *  @param	statement_cnt   statement counter					*
 *  @param	label			label								*
 *  @param	DC				Pointer to data counter				*
 *  @param	word			Symbol type defining word 			*
 *  @param 	head			Pointer to head of symbol table		*
 *  @param	error_flag		Pointer to error flag for later stop*
 *  @return TRUE(1) if label was added or else FALSE(0)			*
 *--------------------------------------------------------------*/
int addSymbol(int statement_cnt,char *label, int *DC, char *word, Tlinkptr *head, int *error_flag){
	Tlinkptr runner = *head;
	int label_len = strlen(label);

	/* Fixes Label end */
	if(label[label_len-1]==':') {
		label[label_len-1]='\0';
		label_len--;
	}

	/* Checks if Label is legal */
	if(!isLabelLegal(statement_cnt,label)){
		(*error_flag)=ERROR;
		return FALSE;
	}	
	
	/* if Table is empty add label to head */
	if(!(*head)){
		*head = (Tlinkptr) malloc(sizeof(Tlink));
		strcpy((**head).symbol,label); 			/* add label to symbol table */
		if(isData(word) || isString(word)){	/* if data with value DC and attribute data */
			(**head).value=(*DC);
			(**head).is_data=TRUE; 
		}
		if(isExtern(word)){			/* if extern with value DC and attribute extern */
			(**head).value=0;
			(**head).is_extern=TRUE;
		}
		if(!strcmp(word,"code")){		/* if code with value DC and attribute code */
			(**head).value=(*DC);
			(**head).is_code=TRUE; 
		}
		return TRUE;
	}
	if(strcmp((*runner).symbol,label)==0 && !isExtern(word)){
		printf("*** Error in line: %d, Symbol %s is already in symbol table *** \n",statement_cnt,label);
		(*error_flag)=ERROR;
		return FALSE;
	}
	while((*runner).next){
		runner = (*runner).next;
		if(strcmp((*runner).symbol,label)==0){
			printf("*** Error in line: %d, Symbol %s is already in symbol table",statement_cnt,label);
			(*error_flag)=ERROR;
			if(isExtern(word) && (*runner).is_extern==FALSE)	/* if symbol in list without external attribute report error */
				printf(", and doesnt have attribute external");
			printf(" *** \n");
			return FALSE;
		}
	}
	(*runner).next = (Tlinkptr) malloc(sizeof(Tlink));
	strcpy((*((*runner).next)).symbol,label);		/* add label to symbol table */
	if(isData(word) || isString(word)){			/* if data with value DC and attribute data */
		(*((*runner).next)).value=(*DC);
		(*((*runner).next)).is_data=TRUE; 
	}
	if(isExtern(word)){						/* if extern with value DC and attribute extern */
		(*((*runner).next)).value=0;
		(*((*runner).next)).is_extern=TRUE;
	}
	if(!strcmp(word,"code")){			/* if code with value DC and attribute code */
		(*((*runner).next)).value=(*DC);
		(*((*runner).next)).is_code=TRUE; 
	}
	return TRUE;	
}

/** ------------------------------------------------------------*
 *  Recieves a pointer to a link in a linked list and frees 	*
 *  allocated memory for it and all following links.			*
 *  															*
 *  @param 	runner			Pointer to head of symbol table		*
 *--------------------------------------------------------------*/
void freeTable(Tlinkptr runner){
	Tlinkptr tmp;
	while (runner){
       tmp = runner;
       runner = runner->next;
       free(tmp);
    }
}

/** ------------------------------------------------------------*
 *  Recieves a label and a pointer to a link in a linked list  	*
 *  and returns pointer to the link with the label name or NULL.*
 *  															*
 *  @param	label			label								*
 *  @param 	head			Pointer to head of symbol table		*
 *  @return pointer to link if found or NULL					*
 *--------------------------------------------------------------*/
Tlinkptr getSymbol(char label[],Tlinkptr *head){
	Tlinkptr runner = *head;
	while(runner){
		if(strcmp((*runner).symbol,label)==0)
			return runner;
		runner = (*runner).next;
	}
	return NULL;
}

/** ------------------------------------------------------------*
 *  Recieves code final count and updates data labels values 	*
 *  															*
 *  @param 	ICF				Machine Code Counter				*
 *  @param 	head			Pointer to head of symbol table		*
 *--------------------------------------------------------------*/
void updateDataSymbols(int ICF,Tlinkptr *head){
	Tlinkptr runner = *head;
	while(runner){
		if((*runner).is_data==1)
			(*runner).value+=ICF;
		runner = (*runner).next;
	}
}

/** ------------------------------------------------------------*
 *  Recieves an extern label and extern table head, and adds to *
 *  Extern Table.												*
 *  															*
 *  @param	label			label								*
 *  @param	DC				Data counter value					*
 *  @param 	head			Pointer to head of extern table		*
 *  @param	error_flag		Pointer to error flag for later stop*
 *  @return TRUE(1) 											*
 *--------------------------------------------------------------*/
int addExternSymbol(char *label, int DC, Tlinkptr *head, int *error_flag){
	Tlinkptr runner = *head;

	/* Fixes Label end */
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
		(**head).is_extern=TRUE;
		return TRUE;
	}
	while((*runner).next){
		runner = (*runner).next;
	}
	(*runner).next = (Tlinkptr) malloc(sizeof(Tlink));
	strcpy((*((*runner).next)).symbol,label);
	(*((*runner).next)).value=DC;
	(*((*runner).next)).is_extern=TRUE;
	return TRUE;	
}

/** ------------------------------------------------------------*
 *  Recieves a pointer to a link (Tlink) in a linked list and 	*
 *  printsit and all following links (Used in development).		*
 *  															*
 *  @param 	r			Pointer to head of symbol table			*
 *--------------------------------------------------------------*/
void printTable(Tlinkptr r){
	if(r){
		printf("Symbol: %s Value: %d Code: %d Data: %d entry: %d extern: %d Next: %s \n",(*r).symbol,(*r).value,(*r).is_code,(*r).is_data,(*r).is_entry,(*r).is_extern,(*((*r).next)).symbol);
		printTable(r->next);
	}
}