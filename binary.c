#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "passes.h"
#include "symbtab.h"
#include "dict.h"

/* add data to data image and DC+(length of data) */

int isDataLegal(int statement_cnt, char *operands){
    return 1;}

int add_data_to_image(int statement_cnt,char *operands,Operand DATA_IMAGE[],int *DC,int *error_flag){
   	int i=0,j=0;
    int op_length;
    char param[20];
    char nums[strlen(operands)];

    strcpy(nums,operands+strspn(operands," \t"));
    if(!isdigit(nums[i]) && nums[i]!='-' && nums[i]!='+'){
        printf("*** Error in line: %d, data must begin with a digit or +/- character followed by a digit ***\n",statement_cnt,nums[i]);
        return NOCHANGE;
    } else { 
        if(!isdigit(nums[i]) && nums[i+1]==','){
            printf("*** Error in line: %d, data must begin with a digit or +/- character followed by a digit ***\n",statement_cnt,nums[i]);
        } else {
            param[j]=nums[i];
        }
    }
    
    for(i=1,j=1;i<strlen(nums);i++){
        if(isdigit(nums[i])){
            param[j]=nums[i];
            param[j+1]='\0';
            j++;
        } 
        if(nums[i]=='-' || nums[i]=='+'){
            param[j]=nums[i];
            param[j+1]='\0';
            j++;
        }
        
        
        
         && nums[i]!='-' && nums[i]!='+')
            nums[i]!=',' && nums[i]!='\0'){
            if( isdigit(nums[i]) || ((nums[i]=='-' || nums[i]=='+') && (nums[i-1]==','))){
                param[j]=nums[i];
                param[j+1]='\0';
                j++;
            } else {
                printf("%s \n",param);
                printf("%d \n",atoi(param));
                j=0;
            }
        }
    }

    
    return NOCHANGE;
/*

printf("%s",operands);
    printf("%s",nums);
    for(i=0,j=0;i<strlen(nums);i++){

        printf("%c\n",nums[i]);
    
    }
    
    /*    if(nums[i]!=',' && nums[i]!='\0'){
                param[j]=nums[i];
                param[j+1]='\0';
                j++;
            } else {
                printf("%s\n",param);
                printf("%d\n",atoi(param));
                j=0;
            }
        }
    
    /*if(isDataLegal(statement_cnt,nums)){}
    /*sscanf(nums+j," %d ",&param);
    printf("%d \n",param);
    printf("operands: %s\n\n",nums);
    /*if(isDataLegal(statement_cnt,operands)){
        op_length=strlen(operands);
        while (i<op_length)
        {   
            sscanf(operands+i,"%d",&param);
            printf("\n %d \n",param);
            printf("%d",strcspn(operands+i,","));
            i++;
        }}


    sscanf(operands,"%d",&param);
    printf("\n\n\n  %d \n\n\n",param);
    pi=strchr(operands,',')+1;
    printf("\n\n\n %c %d \n\n\n",*pi ,param);
    pi=strchr(pi,',');
    printf("\n\n\n %c %d \n\n\n",*pi ,param);
    if(isDataLegal(statement_cnt,operands)){
        sscanf(operands,"%d",&param);
       /* DATA_IMAGE[(*DC)].val=param;
        (*DC)++;*/
       /* pi=strrchr(pi,',');
        printf("\n\n\n %c %d \n\n\n",*pi ,param);
        /*last_char=strrchr(pi,',');
        while(pi<last_char){
            sscanf(pi,"%d",&param);
            printf("\n\n\n %d \n\n\n",param);
            DATA_IMAGE[(*DC)].val=param;
            printf("\n\n\n %d \n\n\n",DATA_IMAGE[(*DC)].val);
            (*DC)++;
            pi=strchr(pi,',');
            pi++;
            pi++;
        }
        sscanf(last_char,"%d",&param);
        printf("\n\n\n %d \n\n\n",param);
        DATA_IMAGE[(*DC)].val=param;
        printf("\n\n\n %d \n\n\n",DATA_IMAGE[(*DC)].val);
        (*DC)++;
        return CHANGED;
    }*/  
}


/*************************************************************
	DATA_IMAGE[0]=27;
	*************************************************************/
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
            DATA_IMAGE[(*DC)].val=*pi;
            (*DC)++;
            pi++;
        }
        DATA_IMAGE[(*DC)].val='\0';
        (*DC)++;
        return CHANGED;
    }
    return NOCHANGE;  
}