#include <stdio.h>
#include <string.h>
#include "assemble.h"
#include "data.h"
#include "symbtab.h"
#include "utils.h"

/** ------------------------------------------------------------*
 *  Code Image - Array [Struct{Union,are,flag}]					*
 * 	Data Image - Array [Union]		(Union for signed\unsigned)	*
 *  ICF/DCF Counters - Image and Data Final Counters			*
 *  Symbol Table - Linked List of Symbols/Labels				*
 *  Externals Table - Linked List of external Symbols/Labels	*
 *--------------------------------------------------------------*/
MachineOrder CODE_IMAGE[MAX_MACHINE_ORDERS];
Operand DATA_IMAGE[MAX_DATA_ORDERS];
Tlist SYMB_TABLE;
Tlist EXTERN_TABLE;
static int ICF,DCF;

/** ------------------------------------------------------------*
 *  Recieves a filename adds the extention .as & reads statement*
 *  by statement and calls first pass, accordingly updates data *
 *  symbols, calls second pass and adds data to Code Image.		*
 * 																*
 *  @param file		 without extention							*
 *  @return TRUE(1) if file read and coding the data into the 	*
 * 			code image was succefull or else FALSE(0). 			*
 *--------------------------------------------------------------*/
int readFile(char *file){
	FILE *fd;
	int IC=CODE_BEGIN,DC=DATA_BEGIN;
	char file_ext[MAX_FILENAME_LENGTH]="\0";
	char statement[MAX_STATEMENT_LENGTH]="\0";
	int statement_cnt=0; 					/* statements counter */
	int error_flag=0;
	
	/* If data is not accessable stop function */
	strcpy(file_ext,file);
	strcat(file_ext,".as");
	if(!(fd=fopen(file_ext,"r+"))){
		printf("*** Error - Cannot find or open the file %s. ***\n",file_ext);
		return FALSE;
	}

	/****************** First Pass *****************/
	printf("The file %s was found, intiating first pass proccess:\n",file_ext);
	while(fgets(statement,MAX_STATEMENT_LENGTH,fd)){
		statement_cnt++;
		firstPass(statement,statement_cnt,CODE_IMAGE,&IC,DATA_IMAGE,&DC,&(SYMB_TABLE.head),&error_flag);	
	}

	/* Error Termination after finishing first pass */
	if(error_flag==ERROR){
		printf("*** Error - Errors were found in the first pass of the file %s, terminating proccess for this file. ***\n",file_ext);
		freeTable(SYMB_TABLE.head);
		fclose(fd);
		return FALSE;
	}

	/*	Updating final counters and Symbol Table links using +ICF */
	ICF=IC;
	DCF=DC;
	updateDataSymbols(ICF,&(SYMB_TABLE.head));
	
	/****************** Intializing for Second Pass *****************/
	printf("First pass of %s finished successuly, intiating second pass:\n",file_ext);
	rewind(fd);	
	statement_cnt=0;		/* Variables Init */
	IC=0;				/* Works as an index as well */
	DC=DATA_BEGIN;
	
	/****************** Second Pass *****************/
	while(fgets(statement,MAX_STATEMENT_LENGTH,fd)){
		statement_cnt++;
		secondPass(statement,statement_cnt,&IC,&DC,&(SYMB_TABLE.head),DATA_IMAGE,CODE_IMAGE,&(EXTERN_TABLE.head),&error_flag);
	}
	fclose(fd);

	/******* Adding Data Image to Code Image **********/
	addBinaryData(CODE_IMAGE,ICF-CODE_BEGIN,DATA_IMAGE,DCF);
	
	/* Error Termination after finishing second pass */
	if(error_flag==ERROR){
		printf("*** Error - Errors were found in second pass of file %s, terminating proccess for this file. ***",file_ext);
		freeTable(SYMB_TABLE.head);
		return FALSE;
	}
	return TRUE;
}

/** ------------------------------------------------------------*
 *  Recieves a filename checks if data in CODE_IMAGE and Symbol *
 *  Table are suffecient to build .ob, .ext & .ent files. if so *
 *  writes files, if not sufficient for .ent or/and .ext because*
 *  no entries or externals then only builds .ob file and only	*
 *  one of .ent or .ext	or neither.								*
 *																*
 *  @param file		 without extention							*
 *  @return TRUE(1) if file/files were written or else FALSE(0) *
 *--------------------------------------------------------------*/
int writeFiles(char *file){
	FILE *fd;
	Tlinkptr runner;
	char file_ext[MAX_FILENAME_LENGTH]="\0";
	char statement[MAX_STATEMENT_LENGTH]="\0";
	int statement_cnt=CODE_BEGIN;
	int i=0,entry_flag=0;

	/* If file is not accessable stop function */
	strcat(file_ext,file);
	strcat(file_ext,".ob");
	if(!(fd=fopen(file_ext,"w+"))){
		printf("*** Error - Cannot open the file %s for writing output, check if you have permission or disk space. ***\n",file_ext);
		return FALSE;
	}

	/************* Writing data to .ob file from code image based on type  *****************/
	printf("The file %s opened, writing data.\n",file_ext);
	sprintf(statement,"\t%d %d\n",ICF-CODE_BEGIN,DCF);
	fputs(statement,fd);
	while(i<ICF+DCF-CODE_BEGIN){
		switch (CODE_IMAGE[i].flag){
			case OPERATOR_FLAG:					/* Operator flag*/
				sprintf(statement,"%04d %X%X%X %X\n",statement_cnt,CODE_IMAGE[i].optype.operator.opcode,CODE_IMAGE[i].optype.operator.funct,
					CODE_IMAGE[i].optype.operator.src_add*4+CODE_IMAGE[i].optype.operator.dist_add,CODE_IMAGE[i].are);
				break;
			case SRC_OPERAND_FLAG:			/* Source Operand flag */
				if(CODE_IMAGE[i].are==0xC)
					sprintf(statement,"%04d %03X %c\n",statement_cnt,CODE_IMAGE[i].optype.src_oper.val.unsign,'R');
				else
					sprintf(statement,"%04d %03X %X\n",statement_cnt,CODE_IMAGE[i].optype.src_oper.val.unsign,CODE_IMAGE[i].are);
				break;
			case DIST_OPERAND_FLAG:
				if(CODE_IMAGE[i].are==0xC)	/* dist Operand flag */
					sprintf(statement,"%04d %03X %c\n",statement_cnt,CODE_IMAGE[i].optype.dist_oper.val.unsign,'R');
				else
					sprintf(statement,"%04d %03X %X\n",statement_cnt,CODE_IMAGE[i].optype.dist_oper.val.unsign,CODE_IMAGE[i].are);
				break;
			case INFO_FLAG:					/* information flag */
					sprintf(statement,"%04d %03X %X\n",statement_cnt,CODE_IMAGE[i].optype.information.val.unsign,CODE_IMAGE[i].are);
				break;
		}
		i++;
		statement_cnt++;
		fputs(statement,fd);
	}
	fclose(fd);

	/************* Writing data to .ext file from extern table if externals exist  *****************/
	if(EXTERN_TABLE.head){
		strcpy(file_ext,file);
		strcat(file_ext,".ext");
		if(!(fd=fopen(file_ext,"w+"))){
			printf("*** Error - Cannot open the file %s for writing output, check if you have permission or disk space. ***\n",file_ext);
			return FALSE;
		}
		printf("The file %s opened, writing data.\n",file_ext);
		runner=EXTERN_TABLE.head;
		while(runner){
			sprintf(statement,"%s %04d\n",runner->symbol,CODE_BEGIN+runner->value);
			runner=runner->next;
			fputs(statement,fd);
		}
	}
	fclose(fd);
	
	/************* Writing data to .ent file from symbol table if entries exist  *****************/
	strcpy(file_ext,file);
	strcat(file_ext,".ent");
	runner=SYMB_TABLE.head;
	while(runner){
		if((*runner).is_entry){
			if(entry_flag==0){
				if(!(fd=fopen(file_ext,"w+"))){
					printf("*** Error - Cannot open the file %s for writing output, check if you have permission or disk space. ***\n",file_ext);
					return FALSE;
				}
				entry_flag=1;
				printf("The file %s opened, writing data.\n",file_ext);
			}
			sprintf(statement,"%s %04d\n",runner->symbol,runner->value);
			fputs(statement,fd);
		}
		runner = runner->next;
	}
	fclose(fd);

	freeTable(EXTERN_TABLE.head);
	freeTable(SYMB_TABLE.head);
	return TRUE;
}