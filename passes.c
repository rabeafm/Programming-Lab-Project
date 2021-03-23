#include <stdio.h>
#include <string.h>
#include "assemble.h"
#include "data.h"
#include "symbtab.h"
#include "utils.h"

/** ------------------------------------------------------------*
 *  Implementation of first pass Function 						*
 *  Checks for Blanks & Comments & Entry to ignore, Labels to 	*
 *  add to Symbol Table, Data or Strings or Extern to Data Image* 
 *  Gives Warnings and Errors when needed and stops only after  *
 *  finishing calls other funtions to analyze statements or to 	*
 *  convert to binary on success.								*
 *  @param	statement		a statement from the file			*
 *  @param	statement_cnt   statement counter					*
 *  @param	CODE_IMAGE		Array of structs representing orders*
 *  @param	IC				Pointer to code counter				*
 *  @param	DATA_IMAGE		Array of unions representing operand*
 *  @param	DC				Pointer to data counter				*
 *  @param 	Tlinkptr		Pointer to head of symbol table		*
 *  @param	error_flag		Pointer to error flag for later stop*
 *  @return TRUE(1) if change was made or else FALSE(0)			*
 *--------------------------------------------------------------*/
int firstPass(char *statement,int statement_cnt,MachineOrder CODE_IMAGE[],int *IC,Operand DATA_IMAGE[],int *DC,Tlinkptr *head,int *error_flag){
	/**	  Label	  - 	Operator  -  Source Operand - Distination Operand - Fixes Seperate Comma - Fixes quick identification of too many operands */
	char label[32]="\0",word[32]="\0",srcoper[32]="\0",distoper[32]="\0",comma[32]="\0",tmi[32]="\0";
	int label_length = 0;
	int L=0;				/* Variable Defined in Algorithem used for counting extra Operands per statement  */

	/*	Checking for Blank or Comment Lines - Should Ignore Them  */
	sscanf(statement,"%s:",word);
	if(isBlank(word) || isComment(word)){
		return FALSE;
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
		return addDataToImage(statement_cnt,strstr(statement,".data")+5,DATA_IMAGE,DC,error_flag);
	}
	if(isString(word)){
		if(label_length)
			add_symbol(statement_cnt,label,DC,word,head,error_flag);
		return addStringToImage(statement_cnt,strstr(statement,".string")+7,DATA_IMAGE,DC,error_flag);
	}

	/* Ignore Enrty*/
	if(isEntry(word)){
		if(label_length)
			printf("*** Warning in line: %d, the label %s defined before .entry has no meaning and will be ignored. ***\n",statement_cnt,label);
		return FALSE;
	}
	/* Handle Extern - should check symbol table issues*/
	if(isExtern(word)){
		if(label_length){
			printf("*** Warning in line: %d, the label %s defined before .extern has no meaning and will be ignored. ***\n",statement_cnt,label);
			sscanf(statement,"%s %s %s",srcoper,word,label); /* Make the second the label */
		} else { 
			sscanf(statement,"%s %s",word,label); 
		}
		return add_symbol(statement_cnt,label,DC,word,head,error_flag);
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
		return FALSE;
	}

	/* lookup operator in operator list if doesnt exist report error*/
	if(isOperatorLegal(word)==-1){
		printf("*** Error in line: %d, illegal command %s check the spelling (commands must be lowercase)  ***\n",statement_cnt,word);
		*error_flag = 3;
		return FALSE;
	}

	/* Analayze operand structure and check and calculate number of words needed ->L */
	L=isStatementLegal(statement_cnt,word,srcoper,distoper,comma,error_flag);

	if(*error_flag!=3) {
		 /*  making sure srcoper and distoper have the needed values if needed*/
    	if(srcoper[strlen(srcoper)-1]==',')
			srcoper[strlen(srcoper)-1]='\0';
    	if(distoper[0]==',')
			strcpy(distoper,distoper+1);
    	if(strcmp(distoper,"\0")==0)
			strcpy(distoper,comma);
    	if(distoper[0]==',')
			strcpy(distoper,distoper+1);
    	if(strcmp(distoper,"\0")==0){
        	strcpy(distoper,srcoper);
		}
		makeFirstBinary(statement_cnt, CODE_IMAGE, *IC,word,srcoper,distoper,error_flag);
	}
	(*IC)+=L;	/*update IC=IC+L*/

	return TRUE;
}


/** ------------------------------------------------------------*
 *  Implementation of second pass Function 						*
 *  Adds to attributes of symbol in symbols table, calls a 		*
 *  function to continue filling Code Image with data			*
 *  	*
 *  convert to binary on success.								*
 *  @param	statement		a statement from the file			*
 *  @param	statement_cnt   statement counter					*
 *  @param	CODE_IMAGE		Array of structs representing orders*
 *  @param	IC				Pointer to code counter				*
 *  @param	DATA_IMAGE		Array of unions representing operand*
 *  @param	DC				Pointer to data counter				*
 *  @param 	Tlinkptr		Pointer to head of symbol table		*
 *  @param	error_flag		Pointer to error flag for later stop*
 *  @return TRUE(1) if change was made or else FALSE(0)			*
 *--------------------------------------------------------------*/
int secondPass(char *statement,int statement_cnt,int *IC,int *DC,Tlinkptr *head,Operand DATA_IMAGE[],MachineOrder CODE_IMAGE[],Tlinkptr *extern_head,int *error_flag){
	char word[32]="\0",srcoper[32]="\0",distoper[32]="\0",comma[32]="\0";
	Tlinkptr runner;
	
	sscanf(statement,"%s %s %s %s", word, srcoper, distoper,comma);

	/*	Checking for Blank or Comment Lines - Should Ignore Them  */
	if(isBlank(word) || isComment(word)){
		return FALSE;
	}
	/* if first word is Label if it is mark the label flag and put the data in the right variables */ 
	if(isLabel(word)){
		sscanf(statement+strlen(word),"%s %s %s %s", word, srcoper, distoper,comma);
	}
	
	/* if data / string / extern ignore */
	if(isData(word) || isString(word) || isExtern(word)){
		return FALSE;
	}

	if(isEntry(word)){
		printf("Entry %s operand %s\n",word,srcoper);
		runner=get_symbol(srcoper,head);
		if(runner){
			if((*runner).is_extern){
				printf("*** Error in line: %d - the symbol %s was already defined as external it cant be defined as entry ***\n",statement_cnt,srcoper);
				return FALSE;
			}
			(*runner).is_entry=1;
			return TRUE;
		} else {
			printf("*** Error in line: %d - the symbol %s was not found in the symbol table ***\n",statement_cnt,srcoper);
			(*error_flag)=3;
			return FALSE;
		}
	}
	/* bincode rest of operand infowords, based on addressing
	   every operand with symbol  find symbol value from table 
	   and if not in table give error.
	   if has external attribute add infoword address tolist of externals 
	   i can use IC and L .*/
		
	if(*error_flag!=3) {
		 /*  making sure srcoper and distoper have the needed values if needed*/
    	if(srcoper[strlen(srcoper)-1]==',')
			srcoper[strlen(srcoper)-1]='\0';
    	if(distoper[0]==',')
			strcpy(distoper,distoper+1);
    	if(strcmp(distoper,"\0")==0)
			strcpy(distoper,comma);
    	if(distoper[0]==',')
			strcpy(distoper,distoper+1);
    	if(strcmp(distoper,"\0")==0){
        	strcpy(distoper,srcoper);
		}
		makeSecondBinary(statement_cnt,CODE_IMAGE,IC,word,srcoper,distoper,head,extern_head,error_flag);
	}

	
	return 1;
}