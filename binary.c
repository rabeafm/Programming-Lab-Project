#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "passes.h"
#include "symbtab.h"
#include "dict.h"

#define MAX_DIGITS 5

/* add data to data image and DC+(length of data) */

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
    if(!isdigit(nums[length])){
        printf("*** Error in line: %d, data must end with a digit ***\n",statement_cnt);
        return FALSE;
    }
    for(i=0;i<=length;i++){
        if(!isdigit(nums[i]) && nums[i]!='-' && nums[i]!='+' && nums[i]!=',' && nums[i]!=' ' && nums[i]!='\t'){
            printf("*** Error in line: %d, illegal character -%c- was used, you can only use digits,commas,spaces,\\t and +/- ***\n",statement_cnt,nums[i]);
            return FALSE;
        }
    }
    return TRUE;
}

int add_data_to_image(int statement_cnt,char *operands,Operand DATA_IMAGE[],int *DC,int *error_flag){
   	int i,j;
    int sign_flag=0,space_flag=0,comma_flag=0,length;
    char param[MAX_DIGITS];
    char nums[80];

    /*  Trimming Begining and end operand */
    strcpy(nums,operands+strspn(operands," \t"));
    for (length=strlen(nums)-1; length>=0 ; length--)
        if(nums[length]!=' ' && nums[length]!='\t' && nums[length]!='\n')
            break;
    nums[length+1] = '\0';

    if(isDataLegal(statement_cnt,nums,length)){
        for(i=0,j=0;i<=length+1;i++){
            if(nums[i]=='-' || nums[i]=='+'){
                if(sign_flag==1){
                    printf("*** Error in line: %d, you cant use multiple signs consequently or between digits ***\n",statement_cnt);
                    return NOCHANGE;
                }
                space_flag=1;
                sign_flag=1;
                param[j]=nums[i];
                param[j+1]='\0';
                j++;
            } else {
                if(isdigit(nums[i])){
                    if(space_flag==1 && (nums[i-1]==' ' || nums[i-1]=='\t')){
                        printf("*** Error in line: %d, you cant use spaces and \\t between digits of the same number or between a sign and a digit ***\n",statement_cnt);
                        return NOCHANGE;
                    }
                    comma_flag=0;
                    space_flag=1;
                    sign_flag=1;
                    param[j]=nums[i];
                    param[j+1]='\0';
                    j++;
                } else {
                    if(nums[i]==',' || nums[i]=='\0'){
                        if(comma_flag==1)
                            printf("*** Error in line: %d, you cant use multiple commas consequently there must be digits between commas ***\n",statement_cnt);
                        comma_flag=1;
                        space_flag=0;
                        sign_flag=0;
                        j=0;
                        DATA_IMAGE[(*DC)].val.sval=atoi(param);
                        printf("DC: %d data image: %d atoi param: %d  param: %s\n",(*DC),DATA_IMAGE[(*DC)].val.sval,atoi(param),param);
                        (*DC)++;
                    }
                }
            }
        }
        return CHANGED;
    }
    return NOCHANGE;
}

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
        if((*last_char) != ' ' && (*last_char) != '\t' && (*last_char) != '\n'){
            printf("*** Error in line: %d, writing after string end is illegal ***\n",statement_cnt);
            return FALSE;
        }
        last_char++;
    }
    return TRUE;
}

/* add string to data image and DC+(length of string) */
int add_string_to_image(int statement_cnt,char *operands,Operand DATA_IMAGE[],int *DC, int *error_flag){
    char *pi;
    char *last_char;
    if(isStringLegal(statement_cnt,operands)){
        pi=strchr(operands,'\"')+1;
        last_char=strrchr(operands,'\"');
        while(pi<last_char){
            DATA_IMAGE[(*DC)].val.uval=*pi;
            printf("DC: %d data image: %c param: %c\n",(*DC),DATA_IMAGE[(*DC)].val.uval,'\0');
            (*DC)++;
            pi++;
        }
        DATA_IMAGE[(*DC)].val.uval='\0';
        printf("DC: %d data image: %c param: %c\n",(*DC),DATA_IMAGE[(*DC)].val.uval,'\0');
        (*DC)++;
        return CHANGED;
    }
    return NOCHANGE;  
}