/* fix make file befor giving project remove o and x folders*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dict.h"
#include "symbtab.h"
#include "passes.h"

/* General Constants and Definitions */
#define MAX_MACHINE_ORDERS 850  /* Based on RAM size 4096 and Each Machine Order is 48 bits aka 4 RAM units */
#define MAX_DATA_ORDERS 150     /* its 1024 from which 850 to machine and 150 for data */
#define MAX_FILENAME_LENGTH 100
#define MAX_STATEMENT_LENGTH 82


/* Code Image / Data Image / Symbol Table / ICF & DCF */
MachineOrder CODE_IMAGE[MAX_MACHINE_ORDERS];
Operand DATA_IMAGE[MAX_DATA_ORDERS];
Tlist SYMB_TABLE = { NULL };
static int ICF,DCF;

/* Function Declarations */
int readfile(char *);
int writefiles(char *);
void printTable(Tlinkptr);
void freeTable(Tlinkptr);

/* Assembler main runfile */
int main(int argc, char *argv[]) {
	while (*++argv){
		if(readfile(*argv)){
			printTable(SYMB_TABLE.head);
			writefiles(*argv);
		} 
	}
	freeTable(SYMB_TABLE.head);
	return 0;
}


/* Declaring Global Variables and Functions to be used */
int readfile(char *file){
	FILE *fd;
	char file_ext[MAX_FILENAME_LENGTH];
	char statement[MAX_STATEMENT_LENGTH];
	int statement_cnt=0;
	int error_flag=0;

	int IC=100,DC=0;

	strcpy(file_ext,"x/");			/*	remove the x directory*/
	strcat(file_ext,file);
	strcat(file_ext,".as");
	if(!(fd=fopen(file_ext,"r+"))){ /* writable problem accessing file*/
		printf("Cannot find or open the file %s .\n",file_ext);
		return NOCHANGE;
	}

	printf("The file %s was found, intiating first pass:\n",file_ext);
	while(fgets(statement,MAX_STATEMENT_LENGTH,fd)){
		statement_cnt++;
		firstpass(statement,statement_cnt,&IC,&DC,&(SYMB_TABLE.head),DATA_IMAGE,CODE_IMAGE,&error_flag);	
	}
	if(error_flag==3){
		printf("Errors were found in first pass of file %s, terminating proccess for this file.",file_ext);
		fclose(fd);
		return NOCHANGE;
	}
	ICF=IC;
	DCF=DC;
	update_data_symbols(ICF,&(SYMB_TABLE.head));	/*	Update in Symbol Table every data Symbol using +ICF */
	statement_cnt=0;

	rewind(fd);
	printf("First pass of %s finished successuly, intiating second pass:\n",file_ext);
	

	while(fgets(statement,MAX_STATEMENT_LENGTH,fd)){
		statement_cnt++;
		secondpass(statement,statement_cnt,&IC,&DC,&(SYMB_TABLE.head),DATA_IMAGE,CODE_IMAGE,&error_flag);	
	}
	fclose(fd);
	if(error_flag==3){
		printf("Errors were found in second pass of file %s, terminating proccess for this file.",file_ext);
		return NOCHANGE;
	}
	return CHANGED;
}

int writefiles(char *file){
	FILE *fd;
	char file_ext[MAX_FILENAME_LENGTH];
	char statement[MAX_STATEMENT_LENGTH]="\0";
	int statement_cnt=100;

	strcpy(file_ext,"x/");			/*	remove the x directory*/
	strcat(file_ext,file);
	strcat(file_ext,".ob");
	if(!(fd=fopen(file_ext,"w+"))){ /* writable problem accessing file*/
		printf("Cannot open the file %s for writing output, check if you have permission or disk space.\n",file_ext);
		return NOCHANGE;
	}
	sprintf(statement,"\t%d %d\n",ICF-statement_cnt,DCF);
	fputs(statement,fd);
	printf("The file %s opened, writing data:\n",file_ext);
	while (statement_cnt<ICF+DCF)
	{
		sprintf(statement,"%04d %X%X%X %X\n",statement_cnt,CODE_IMAGE[statement_cnt-100].operator.opcode,CODE_IMAGE[statement_cnt-100].operator.funct,
			CODE_IMAGE[statement_cnt-100].operator.src_add*4+CODE_IMAGE[statement_cnt-100].operator.dist_add,CODE_IMAGE[statement_cnt-100].operator.are);
		fputs(statement,fd);
		statement_cnt++;
	}	
	printf("write files\n"); 
	fclose(fd);
	return 1;
}

void printTable(Tlinkptr r){
	if(r){
		printf("Symbol: %s Value: %d Code: %d Data: %d entry: %d extern: %d Next: %s \n",(*r).symbol,(*r).value,(*r).is_code,(*r).is_data,(*r).is_entry,(*r).is_extern,(*((*r).next)).symbol);
		printTable((*r).next);
	}
}

void freeTable(Tlinkptr r){
	if(r){
		printf("There is %s\n",(*((*r).next)).symbol);
		freeTable((*r).next);
		free(r);
	}
}
