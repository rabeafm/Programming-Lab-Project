#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

/***************** A list of reserved words ********************/
char *RESERVED_WORDS[] = {
	"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc",
	"dec", "jmp", "bne", "jsr",	"red", "prn", "rts", "stop",
	"r0", "r1", "r2", "r3",	"r4", "r5", "r6", "r7",
	".data",".string",".entry",".extern"
};

/** ------------------------------------------------------------*
 *  Recieves an order statement split to its components and a   * 
 *  statement count and an error flag checks if the statement   *
 *  structure is legal (Report Errors).                         *
 *                                                              *
 *  @param statement_cnt statements count						*
 *  @param word 		 operator          						*
 *  @param srcoper 		 source operand          				*
 *  @param distoper 	 destination operand          			*
 *  @param comma 		 destination operand if comma has spaces*
 *                       from both sides          				*
 *  @param	error_flag		Pointer to error flag for later stop*
 *  @return Number of elements in statement (number of machine  *
 *          orders needed) or else FALSE(0)		                *
 *--------------------------------------------------------------*/
int isStatementLegal(int statement_cnt,char *word,char *srcoper,char *distoper,char *comma,int *error_flag){
    int commacnt=0;     /* Comma counter */
    char *fix;          /* used for string fixes */

    switch(isOperatorLegal(word)){
        case 14: case 15:       
            if(strcmp(srcoper,"\0")!=0 || strcmp(distoper,"\0")!=0 || strcmp(comma,"\0")!=0 ){
                printf("*** Error in line: %d, order %s doesn't recieve operands ***\n",statement_cnt,word);
                *error_flag=ERROR;
                return FALSE;
            }
            return 1;           /* only an operator */
        case 5: case 6: case 7: case 8: case 9: 
        case 10: case 11: case 12: case 13:
            if(strcmp(distoper,"\0")!=0 || strcmp(comma,"\0")!=0){
                printf("*** Error in line: %d, order %s recieves only one operand ***\n",statement_cnt,word);
                *error_flag=ERROR;
                return FALSE;
            }
            if(!isOperandLegal(statement_cnt,srcoper,strlen(srcoper)-1)){
                *error_flag=ERROR;
                return FALSE;
            }
            return 2;           /* an operator and an operand */
        case 0: case 1: case 2: case 3: case 4:
            if(strcmp(srcoper,"\0")==0 || strcmp(distoper,"\0")==0 || (strcmp(comma,"\0")==0 && distoper[0]==',' && strlen(comma)==1 )){
                fix=strchr(srcoper,',');
                if(strcmp(fix,"\0")!=0){
                    strcpy(distoper,fix+1);
                    *(fix+1)='\0';
                } else {
                    printf("*** Error in line: %d, order %s recieves 2 operands split by comma, one or more operands are missing ***\n",statement_cnt,word);
                    *error_flag=ERROR;
                    return FALSE;
                }
            }
            if(srcoper[strlen(srcoper)-1]==','){
                commacnt++;
                if(!isOperandLegal(statement_cnt,srcoper,strlen(srcoper)-2)){
                    *error_flag=ERROR;
                    return FALSE;
                }
            } else {
                if(!isOperandLegal(statement_cnt,srcoper,strlen(srcoper)-1)){
                    *error_flag=ERROR;
                    return FALSE;
                }
            }
            if(distoper[0]==','){
                commacnt++;
                if(strlen(distoper)>1){
                    if(!isOperandLegal(statement_cnt,distoper+1,strlen(distoper)-1)){
                        *error_flag=ERROR;
                        return FALSE;
                    }
                } else {
                    if(!isOperandLegal(statement_cnt,comma,strlen(comma)-1)){
                        *error_flag=ERROR;
                        return FALSE;
                    }
                }
            } else {
                if(!isOperandLegal(statement_cnt,distoper,strlen(distoper)-1)){
                    *error_flag=ERROR;
                    return FALSE;
                }
                if(strcmp(comma,"\0")!=0){
                    printf("*** Error in line: %d, too many operands ***\n",statement_cnt);
                    *error_flag=ERROR;
                    return FALSE;
                }
            }
            if(commacnt>1 || commacnt==0){
                printf("*** Error in line: %d, too many commas between operands ***\n",statement_cnt);
                *error_flag=ERROR;
                return FALSE;
            }    
            return 3;           /* an operator and two operands */
    }
   return FALSE;
}

/** ------------------------------------------------------------*
 *  Recieves a statements count and a label string checks if the*
 *  label (Report Errors).                                     	*
 *                                                              *
 *  @param statement_cnt statements count						*
 *  @param label 		 label          						*
 *  @return TRUE(1) if string is legal or else FALSE(0)			*
 *--------------------------------------------------------------*/
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
		if(strcmp(label,RESERVED_WORDS[i])==0){
			printf("*** Error in line: %d, the label %s is a reserved word ***\n",statement_cnt,label);
			return FALSE;
		}	
		i++;
	}
	return TRUE;
}

/** ------------------------------------------------------------*
 *  Recieves a word, checks if it exists in assembly language   *
 *  dictionary, and returns its number in the OpTable     		*
 *                                                              *
 *  @param word 		word									*
 *  @return Index in OpTable Dictionary or else -1				*
 *--------------------------------------------------------------*/
int isOperatorLegal(char word[]){
	int i;
	for(i=0;i<NUMBER_OF_ORDERS;i++)
		if(strcmp(RESERVED_WORDS[i],word)==0)
			return i;
	return -1;
}

/** ------------------------------------------------------------*
 *  Recieves a statements count, an operand string and strings  *
 *  length, checks if the operand is legal (Report Errors)     	*
 *                                                              *
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
    if(isOperatorLegal(operand)!=-1 || strcmp(operand,".data")==0 || strcmp(operand,".string")==0
       || strcmp(operand,".entry")==0 || strcmp(operand,".extern")==0){
        printf("*** Error in line: %d, the operand %s is a reserved word ***\n",statement_cnt,operand);
		return FALSE;
    }
    return TRUE;
}

/** ------------------------------------------------------------*
 *  Recieves a statements count, a string of numbers split with	*
 *  , and strings length checks if string of number is legal    *
 * (Report Errors).                                         	*
 *                                                              *
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
 *  checks if string is legal (Report Errors).					*
 *                                                              *
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
 *  Recieves a statement, checks if is blank.				    *
 *                                                              *
 *  @param word 		word or statement						*
 *  @return TRUE(1) if blank or else FALSE(0)					*
 *--------------------------------------------------------------*/
int isBlank(char *word){
	if(word[0]=='\0')
		return TRUE;
	return FALSE;
}

/** ------------------------------------------------------------*
 *  Recieves a statement, checks if is a comment (starts with ;)*
 *                                                              *
 *  @param word 		word or statement						*
 *  @return TRUE(1) if comment or else FALSE(0)					*
 *--------------------------------------------------------------*/
int isComment(char *word){
	if(word[0]==';')
		return TRUE;
	return FALSE;
}

/** ------------------------------------------------------------*
 *  Recieves a word, checks if is label (ends with :)			*
 *                                                              *
 *  @param word 		word									*
 *  @return TRUE(1) if label or else FALSE(0)					*
 *--------------------------------------------------------------*/
int isLabel(char *word){
	if(word[strlen(word)-1]==':')
		return TRUE;
	return FALSE;
}

/** ------------------------------------------------------------*
 *  Recieves a word, checks if is data (has ".data")			*
 *                                                              *
 *  @param word 		word									*
 *  @return TRUE(1) if data or else FALSE(0)					*
 *--------------------------------------------------------------*/
int isData(char *word){
	if(strcmp(word,".data")==0)
		return TRUE;
	return FALSE;
}

/** ------------------------------------------------------------*
 *  Recieves a word, checks if is string (has ".string")		*
 *                                                              *
 *  @param word 		word									*
 *  @return TRUE(1) if string or else FALSE(0)					*
 *--------------------------------------------------------------*/
int isString(char *word){
	if(strcmp(word,".string")==0)
		return TRUE;
	return FALSE;
}

/** ------------------------------------------------------------*
 *  Recieves a word, checks if is entry (has ".entry")			*
 *                                                              *
 *  @param word 		word									*
 *  @return TRUE(1) if entry or else FALSE(0)					*
 *--------------------------------------------------------------*/
int isEntry(char *word){
	if(strcmp(word,".entry")==0)
		return TRUE;
	return FALSE;
}

/** ------------------------------------------------------------*
 *  Recieves a word, checks if is extern (has ".extern")		*
 *                                                              *
 *  @param word 		word									*
 *  @return TRUE(1) if extern or else FALSE(0)					*
 *--------------------------------------------------------------*/
int isExtern(char *word){
	if(strcmp(word,".extern")==0)
		return TRUE;
	return FALSE;
}