#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "passes.h"
#include "symbtab.h"
#include "dict.h"

#define MAX_DIGITS 5

#define NUMBER_OF_ORDERS 16
OperatorDict OpTable[]={
	/* Order   Dis Src	Funct  OpCode	  Order   Dis Src  Funct  OpCode	 Order   Dis Src  Funct  OpCode */
	{  "mov" , /*{31 , 310 ,*/ FMOV , MOV }, {  "cmp" , /*{310 , 310 ,*/ FCMP , CMP }, {  "add" , /*{31 , 310 ,*/ FADD , ADD },
	{  "sub" , /*{31 , 310 ,*/ FSUB , SUB }, {  "lea" , /*{31 , 1 , */FLEA , LEA }, {  "clr" , /*{31 , 0 ,*/ FCLR , CLR },
	{  "not" , /*{31 , 0 ,*/ FNOT , NOT }, {  "inc" , /*{31 , 0 , */FINC , INC }, {  "dec" , /*{31 , 0 , */FDEC , DEC },
	{  "jmp" , /*{21 , 0 ,*/ FJMP , JMP }, {  "bne" , /*{21 , 0 , */FBNE , BNE }, {  "jsr" , /*{21 , 0 ,*/ FJSR , JSR },
	{  "red" , /*{31 , 0 ,*/ FRED , RED }, {  "prn" , /*{310 , 0 , */FPRN , PRN }, {  "rts" , /*{0 , 0 ,*/ FRTS , RTS },
	{  "stop", /*{0 , 0 ,*/ FSTOP, STOP }
};
/* Declaring Global Variables and Functions to be used */
int LookupinDict(char word[]){
	int i;
	for(i=0;i<NUMBER_OF_ORDERS;i++)
		if(strcmp(OpTable[i].key,word)==0)
			return i;
	return -1;
}

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
    return TRUE;
}

int analayzeStatement(int statement_cnt,char *word,char *srcoper,char *distoper,char *comma,int *error_flag){
    int commacnt=0;
    char *fix;
    switch(LookupinDict(word)){
        case 14: case 15:
            if(strcmp(srcoper,"\0")!=0 || strcmp(distoper,"\0")!=0 || strcmp(comma,"\0")!=0 ){
                printf("*** Error in line: %d, order %s doesn't recieve operands ***\n",statement_cnt,word);
                *error_flag=3;
                return 0;
            }
            return 1;
        case 5: case 6: case 7: case 8: case 9: 
        case 10: case 11: case 12: case 13:
            if(strcmp(distoper,"\0")!=0 || strcmp(comma,"\0")!=0){
                printf("*** Error in line: %d, order %s recieves only one operand ***\n",statement_cnt,word);
                *error_flag=3;
                return 0;
            }
            if(!isOperandLegal(statement_cnt,srcoper,strlen(srcoper)-1)){
                *error_flag=3;
                return 0;
            }
            return 2;
        case 0: case 1: case 2: case 3: case 4:
            if(strcmp(srcoper,"\0")==0 || strcmp(distoper,"\0")==0 || (strcmp(comma,"\0")==0 && distoper[0]==',' && strlen(comma)==1 )){
                fix=strchr(srcoper,',');
                if(strcmp(fix,"\0")!=0){
                    strcpy(distoper,fix+1);
                    *(fix+1)='\0';
                } else {
                    printf("*** Error in line: %d, order %s recieves 2 operands split by comma, one or more operands are missing ***\n",statement_cnt,word);
                    *error_flag=3;
                    return 0;
                }
            }
            if(srcoper[strlen(srcoper)-1]==','){
                commacnt++;
                if(!isOperandLegal(statement_cnt,srcoper,strlen(srcoper)-2)){
                    *error_flag=3;
                    return 0;
                }
            } else {
                if(!isOperandLegal(statement_cnt,srcoper,strlen(srcoper)-1)){
                    *error_flag=3;
                    return 0;
                }
            }
            if(distoper[0]==','){
                commacnt++;
                if(strlen(distoper)>1){
                    if(!isOperandLegal(statement_cnt,distoper+1,strlen(distoper)-1)){
                        *error_flag=3;
                        return 0;
                    }
                } else {
                    if(!isOperandLegal(statement_cnt,comma,strlen(comma)-1)){
                        *error_flag=3;
                        return 0;
                    }
                }
            } else {
                if(!isOperandLegal(statement_cnt,distoper,strlen(distoper)-1)){
                    *error_flag=3;
                    return 0;
                }
                if(strcmp(comma,"\0")!=0){
                    printf("*** Error in line: %d, too many operands ***\n",statement_cnt);
                    *error_flag=3;
                    return 0;
                }
            }
            if(commacnt>1 || commacnt==0){
                printf("*** Error in line: %d, too many commas between operands ***\n",statement_cnt);
                *error_flag=3;
                return 0;
            }    
            return 3;
    }
   return 0;
}

/* bincode first word and every infoword in meyade immediate addressing*/
int makeFirstBinary(MachineOrder CODE_IMAGE[], int statement_cnt,int IC,int L,char word[],char srcoper[],char distoper[],char comma[],int *error_flag){
    int order;
    IC-=100;
    /*  making sure srcoper and distoper have the needed values if needed*/
    if(srcoper[strlen(srcoper)-1]==',')
        srcoper[strlen(srcoper)-1]='\0';
    if(distoper[0]==',')
        distoper=distoper+1;
    if(strcmp(distoper,"\0")==0)
        distoper=comma;
    if(distoper[0]==',')
        distoper=distoper+1;

    /*  getting command from table */
    order=LookupinDict(word);
    /* works for all */
    CODE_IMAGE[IC].operator.are=0xA;
    CODE_IMAGE[IC].operator.opcode=OpTable[order].opcode;
    CODE_IMAGE[IC].operator.funct=OpTable[order].funct;
    switch(order){
        case 14: case 15:
            CODE_IMAGE[IC].operator.dist_add=NO_ADD;                    /*No Break here*/
        case 13:
            if(srcoper[0]=='#'){
                CODE_IMAGE[IC].operator.dist_add=ADD_IMM;               
                CODE_IMAGE[IC+1].dist_oper.val.sign=atoi(srcoper+1);
                CODE_IMAGE[IC+1].operator.are=0xA;                      /*No Break here*/
            }
        case 5: case 6: case 7: case 8: case 9: 
        case 10: case 11: case 12: 
            CODE_IMAGE[IC].operator.src_add=NO_ADD;    
            break;
        case 1:
            if(distoper[0]=='#'){
                CODE_IMAGE[IC].operator.dist_add=ADD_IMM;               /*No Break here*/
                CODE_IMAGE[IC+2].dist_oper.val.sign=atoi(distoper+1);
                CODE_IMAGE[IC+2].operator.are=0xA;
            }

        case 0: case 2: case 3: case 4:
            if(srcoper[0]=='#'){
                CODE_IMAGE[IC].operator.src_add=ADD_IMM;
                CODE_IMAGE[IC+1].src_oper.val.sign=atoi(srcoper+1);
                CODE_IMAGE[IC+1].operator.are=0xA;
            }
            break;
    }
	return 0;
}

int makeSecondBinary(int statement_cnt,MachineOrder CODE_IMAGE[],Operand DATA_IMAGE[], int IC,char word[],char srcoper[],char distoper[],char comma[],Tlinkptr *head,int *error_flag){
    Tlinkptr runner;
    int order;
    /*  making sure srcoper and distoper have the needed values if needed*/
    if(srcoper[strlen(srcoper)-1]==',')
        srcoper[strlen(srcoper)-1]='\0';
    if(distoper[0]==',')
        distoper=distoper+1;
    if(strcmp(distoper,"\0")==0)
        distoper=comma;
    if(distoper[0]==',')
        distoper=distoper+1;

    /*  getting command from table */
    order=LookupinDict(word);
    switch(order){
        case 13:
            runner=get_symbol(srcoper,head);
            if(runner){
                printf("%s %d %d", (*runner).symbol,(*runner).value,IC);
                CODE_IMAGE[IC].operator.dist_add=ADD_DIR;               
                CODE_IMAGE[IC+1].dist_oper.val.unsign=(*runner).value;
                CODE_IMAGE[IC+1].operator.are=0xA;                      /*No Break here*/
            }
        /*case 5: case 6: case 7: case 8: case 9: 
        case 10: case 11: case 12: 
            CODE_IMAGE[IC].operator.src_add=NO_ADD;    
            break;
        case 1:
            if(distoper[0]=='#'){
                CODE_IMAGE[IC].operator.dist_add=ADD_IMM;               /*No Break here*/
               /* CODE_IMAGE[IC+2].dist_oper.val.sign=atoi(distoper+1);
                CODE_IMAGE[IC+2].operator.are=0xA;
            }

        case 0: case 2: case 3: case 4:
            if(srcoper[0]=='#'){
                CODE_IMAGE[IC].operator.src_add=ADD_IMM;
                CODE_IMAGE[IC+1].src_oper.val.sign=atoi(srcoper+1);
                CODE_IMAGE[IC+1].operator.are=0xA;
            }
            break;*/
    }
	return 0;
}


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
                        DATA_IMAGE[(*DC)].val.sign=atoi(param);
                        printf("DC: %d data image: %d atoi param: %d  param: %s\n",(*DC),DATA_IMAGE[(*DC)].val.sign,atoi(param),param);
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
            DATA_IMAGE[(*DC)].val.unsign=*pi;
            printf("DC: %d data image: %c param: %c\n",(*DC),DATA_IMAGE[(*DC)].val.unsign,'\0');
            (*DC)++;
            pi++;
        }
        DATA_IMAGE[(*DC)].val.unsign='\0';
        printf("DC: %d data image: %c param: %c\n",(*DC),DATA_IMAGE[(*DC)].val.unsign,'\0');
        (*DC)++;
        return CHANGED;
    }
    return NOCHANGE;  
}