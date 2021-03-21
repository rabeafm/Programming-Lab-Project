#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "files.h"
#include "binary.h"
#include "dictionary.h"
#include "symbtab.h"


/** ------------------------------------------------------------*
 *  Code Image - Array [Struct{Union,are,flag}]					*
 * 	Data Image - Array [Union]		(Union for signed\unsigned)	*
 *  ICF/DCF Counters - Image and Data Final Counters			*
 *  Symbol Table - Linked List of Symbols/Labels				*
 *--------------------------------------------------------------*/
MachineOrder CODE_IMAGE[MAX_MACHINE_ORDERS];
Operand DATA_IMAGE[MAX_DATA_ORDERS];
Tlist SYMB_TABLE;
static int ICF,DCF;

/** ------------------------------------------------------------*
 *  Recieves a filename adds the extention .as & reads statement*
 *  by statement and calls first pass and second pass.			*
 *  @param filename without extention							*
 *  @return TRUE(1) if file read was successful or else FALSE(0)*
 *--------------------------------------------------------------*/
int readFile(char *file){
	FILE *fd;
	int IC=CODE_BEGIN,DC=DATA_BEGIN;
	char file_ext[MAX_FILENAME_LENGTH]="\0";
	char statement[MAX_STATEMENT_LENGTH]="\0";
	int statement_cnt=0; 				/* statements counter */
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
	if(error_flag==3){
		printf("*** Error - Errors were found in the first pass of the file %s, terminating proccess for this file. ***\n",file_ext);
		freeTable(SYMB_TABLE.head);
		fclose(fd);
		return FALSE;
	}

	/*	Updating final counters and Symbol Table links using +ICF */
	ICF=IC;
	DCF=DC;
	updateDataSymbols(ICF,&(SYMB_TABLE.head));
	
	printf("First pass of %s finished successuly, intiating second pass:\n",file_ext);
	/****************** Intializing for Second Pass *****************/
	rewind(fd);	
	statement_cnt=0;
	IC=0;
	DC=DATA_BEGIN;
	
	/****************** Second Pass *****************/
	while(fgets(statement,MAX_STATEMENT_LENGTH,fd)){
		statement_cnt++;
		secondPass(statement,statement_cnt,&IC,&DC,&(SYMB_TABLE.head),DATA_IMAGE,CODE_IMAGE,&error_flag);
	}

	/******* Adding Data Image to Code ImagebinarySecond Pass **********/
	IC=ICF-CODE_BEGIN;
	while(DC<DCF){
		addBinaryData(CODE_IMAGE,IC,DATA_IMAGE,DC);
		IC++;
		DC++;
	}

	fclose(fd);
	/* Error Termination after finishing first pass */
	if(error_flag==3){
		printf("*** Error - Errors were found in second pass of file %s, terminating proccess for this file. ***",file_ext);
		freeTable(SYMB_TABLE.head);
		return FALSE;
	}
	return TRUE;
}

/** ------------------------------------------------------------*
 *  Recieves a filename checks if data in CODE_IMAGE and Symbol *
 *  Table are suffecient to build .ob file, .ext & .ent file.	*  
 *  @param filename without extention							*
 *  @return TRUE(1) if file/files were written or else FALSE(0) *
 *--------------------------------------------------------------*/
int writeFiles(char *file){
	FILE *fd;
	char file_ext[MAX_FILENAME_LENGTH]="\0";
	char statement[MAX_STATEMENT_LENGTH]="\0";
	int statement_cnt=CODE_BEGIN;
	int i=0;

	/* If data is not accessable stop function */
	strcat(file_ext,file);
	strcat(file_ext,".ob");
	if(!(fd=fopen(file_ext,"w+"))){
		printf("*** Error - Cannot open the file %s for writing output, check if you have permission or disk space. ***\n",file_ext);
		return FALSE;
	}

	/************* Writing data from union based on  *****************/
	printf("The file %s opened, writing data:\n",file_ext);
	sprintf(statement,"\t%d %d\n",ICF-CODE_BEGIN,DCF);
	fputs(statement,fd);
	while(i<ICF+DCF-CODE_BEGIN){
		if(CODE_IMAGE[i].flag==1){ 								/* Operator flag*/
			sprintf(statement,"%04d %X%X%X %X\n",statement_cnt,CODE_IMAGE[i].optype.operator.opcode,CODE_IMAGE[i].optype.operator.funct,
				CODE_IMAGE[i].optype.operator.src_add*4+CODE_IMAGE[i].optype.operator.dist_add,CODE_IMAGE[i].are);
		} else {
			if(CODE_IMAGE[i].flag==2){ 							/* Source Operand flag*/
				if(CODE_IMAGE[i].are==0xC)
					sprintf(statement,"%04d %03X %c\n",statement_cnt,CODE_IMAGE[i].optype.src_oper.val.unsign,'R');
				else
					sprintf(statement,"%04d %03X %X\n",statement_cnt,CODE_IMAGE[i].optype.src_oper.val.unsign,CODE_IMAGE[i].are);
				} else { 										/* dist Operand flag*/
					if(CODE_IMAGE[i].are==0xC)
						sprintf(statement,"%04d %03X %c\n",statement_cnt,CODE_IMAGE[i].optype.dist_oper.val.unsign,'R');
					else
						sprintf(statement,"%04d %03X %X\n",statement_cnt,CODE_IMAGE[i].optype.dist_oper.val.unsign,CODE_IMAGE[i].are);
				}
		}
		statement_cnt++;
		i++;
		fputs(statement,fd);
	}
	printTable(SYMB_TABLE.head);	
	fclose(fd);
	freeTable(SYMB_TABLE.head);
	return TRUE;
}