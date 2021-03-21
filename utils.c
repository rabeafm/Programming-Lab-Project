#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "binary.h"
#include "utils.h"
#include "dictionary.h"

/** ------------------------------------------------------------*
 *  Recieves a statements count, an operand string and strings  *
 *  length, checks if the operand is legal.                 	*
 *  @param statement_cnt statements count						*
 *  @param operand 		 operand        						*
 *  @param length	 	 operand length							*
 *  @return TRUE(1) if string is legal or else FALSE(0)			*
 *--------------------------------------------------------------*/
int isOperandLegal(int statement_cnt, char *operand,int length){
    int i=0;
    if(!isalnum(operand[i]) && operand[i]!='#' && operand[i]!='%' && operand[i]!='-' && operand[i]!='+'){
        printf("*** Error in line: %d, illegal first character in operand ***\n",statement_cnt);
                return FALSE;
    }
    for(i=1;i<length;i++){
        if(!isalnum(operand[i]) && operand[i]!='-' && operand[i]!='+'){
            printf("*** Error in line: %d, illegal character -%c- was used in operand name ***\n",statement_cnt,operand[i]);
            return FALSE;
        }
    }
    if(lookupDict(operand)!=-1 || strcmp(operand,".data")==0 || strcmp(operand,".string")==0
       || strcmp(operand,".entry")==0 || strcmp(operand,".extern")==0){
        printf("*** Error in line: %d, the operand %s is a reserved word ***\n",statement_cnt,operand);
		return FALSE;
    }
    return TRUE;
}

/** ------------------------------------------------------------*
 *  Recieves a statements count, a string of numbers split with	*
 *  ,	and strings length checks if string of number is legal.	*
 *  @param statement_cnt statements count						*
 *  @param nums 		 string of numbers						*
 *  @param length	 	 strings length							*
 *  @return TRUE(1) if string is legal or else FALSE(0)			*
 *--------------------------------------------------------------*/
int isDataLegal(int statement_cnt, char *nums,int length){
    int i=0;
    if(!isdigit(nums[i]) && nums[i]!='-' && nums[i]!='+'){
        printf("*** Error in line: %d, data must begin with a digit or +/- character followed by a digit ***\n",statement_cnt);
        return FALSE;
    }
    if((nums[i]=='-' || nums[i]=='+') && !isdigit(nums[i+1])){
        printf("*** Error in line: %d, data must begin with a digit or +/- character followed by a digit ***\n",statement_cnt);
        return FALSE;
    }
    if(!isdigit(nums[length])){ /* && nums[length]!=13 -- Correction for windows files */
        printf("*** Error in line: %d, data must end with a digit *** \n",statement_cnt);
        return FALSE;
    }
    for(i=0;i<length;i++){
        if(!isdigit(nums[i]) && nums[i]!='-' && nums[i]!='+' && nums[i]!=',' && nums[i]!=' ' && nums[i]!='\t'){
            printf("*** Error in line: %d - illegal character %c was used, you can only use digits,commas,spaces,\\t and +/- ***\n",statement_cnt,nums[i]);
            return FALSE;
        }
    }
    return TRUE;
}

/** ------------------------------------------------------------*
 *  Recieves a statements count, a string of operand/operands	*
 *  checks if string is legal.									*
 *  @param statement_cnt statements count						*
 *  @param operands 	 string                 				*
 *  @return TRUE(1) if string is legal or else FALSE(0)			*
 *--------------------------------------------------------------*/
int isStringLegal(int statement_cnt, char *operands){
	int op_length;
    char *pi;
    char *last_char;

    op_length=strlen(operands);
    pi=operands+strspn(operands," \t");
    if(*pi!='\"'){
   		printf("*** Error in line: %d, strings must begin with \" character ***\n",statement_cnt);
        return FALSE;
    }

    last_char=strrchr(operands,'\"');
    if(last_char==pi){
        printf("*** Error in line: %d, strings must end with \" character ***\n",statement_cnt);
        return FALSE;
    }

    last_char++;
    op_length=strlen(operands);
    while(last_char<&(operands[op_length])){
        if((*last_char) != ' ' && (*last_char) != '\t' && (*last_char) != '\n'){ /* && (*last_char) != 13 - Correction for windows files */
            printf("*** Error in line: %d, writing after string end is illegal ***\n",statement_cnt);
            return FALSE;
        }
        last_char++;
    }
    return TRUE;
}

/** ------------------------------------------------------------*
 *  Recieves a statement/word, checks if blank.					*
 *  @param word 		word or statement						*
 *  @return TRUE(1) if blank or else FALSE(0)					*
 *--------------------------------------------------------------*/
int isBlank(char *word){
	if(word[0]=='\0')
		return TRUE;
	return FALSE;
}

/** ------------------------------------------------------------*
 *  Recieves a statement/word, checks if comment (starts with ;)*
 *  @param word 		word or statement						*
 *  @return TRUE(1) if comment or else FALSE(0)					*
 *--------------------------------------------------------------*/
int isComment(char *word){
	if(word[0]==';')
		return TRUE;
	return FALSE;
}

/** ------------------------------------------------------------*
 *  Recieves a word, checks if label (ends with :)				*
 *  @param word 		word									*
 *  @return TRUE(1) if label or else FALSE(0)					*
 *--------------------------------------------------------------*/
int isLabel(char *word){
	if(word[strlen(word)-1]==':')
		return TRUE;
	return FALSE;
}

/** ------------------------------------------------------------*
 *  Recieves a word, checks if data (has ".data")				*
 *  @param word 		word									*
 *  @return TRUE(1) if data or else FALSE(0)					*
 *--------------------------------------------------------------*/
int isData(char *word){
	if(strcmp(word,".data")==0)
		return TRUE;
	return FALSE;
}

/** ------------------------------------------------------------*
 *  Recieves a word, checks if string (has ".string")			*
 *  @param word 		word									*
 *  @return TRUE(1) if string or else FALSE(0)					*
 *--------------------------------------------------------------*/
int isString(char *word){
	if(strcmp(word,".string")==0)
		return TRUE;
	return FALSE;
}

/** ------------------------------------------------------------*
 *  Recieves a word, checks if entry (has ".entry")				*
 *  @param word 		word									*
 *  @return TRUE(1) if entry or else FALSE(0)					*
 *--------------------------------------------------------------*/
int isEntry(char *word){
	if(strcmp(word,".entry")==0)
		return TRUE;
	return FALSE;
}

/** ------------------------------------------------------------*
 *  Recieves a word, checks if extern (has ".extern")			*
 *  @param word 		word									*
 *  @return TRUE(1) if extern or else FALSE(0)					*
 *--------------------------------------------------------------*/
int isExtern(char *word){
	if(strcmp(word,".extern")==0)
		return TRUE;
	return FALSE;
}

