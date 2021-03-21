#include <stdio.h>
#include <ctype.h>
#include "dictionary.h"
#include "utils.h"
#include "symbtab.h"

/* Operations with coded values of opcode and funct for easier use */
OperatorDict OpTable[]={
	/*Order  Funct OpCode	Order  Funct OpCode	  Order  Funct OpCode   Order  Funct OpCode */
	{ "mov", FMOV, MOV }, { "cmp", FCMP, CMP }, { "add", FADD, ADD }, { "sub", FSUB, SUB },
    { "lea", FLEA, LEA }, { "clr", FCLR, CLR },	{ "not", FNOT, NOT }, { "inc", FINC, INC },
    { "dec", FDEC, DEC }, { "jmp", FJMP, JMP }, { "bne", FBNE, BNE }, { "jsr", FJSR, JSR },
	{ "red", FRED, RED }, { "prn", FPRN, PRN }, { "rts", FRTS, RTS }, { "stop", FSTOP, STOP}
};

/** ------------------------------------------------------------*
 *  Recieves a word, checks if it exists in assembly language   *
 *  dictionary, and returns its number in the OpTable     		*
 *  @param word 		word									*
 *  @return Index in OpTable Dictionary or else -1				*
 *--------------------------------------------------------------*/
int lookupDict(char word[]){
	int i;
	for(i=0;i<NUMBER_OF_ORDERS;i++)
		if(strcmp(OpTable[i].key,word)==0)
			return i;
	return -1;
}



int analayzeStatement(int statement_cnt,char *word,char *srcoper,char *distoper,char *comma,int *error_flag){
    int commacnt=0;
    char *fix;
    switch(lookupDict(word)){
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
int makeFirstBinary(MachineOrder CODE_IMAGE[], int statement_cnt,int IC,int L,char word[],char oper[],char distoper[],char comma[],int *error_flag){
    int order,num,R,cflag=0;
    IC-=100;
    /*  making sure srcoper and distoper have the needed values if needed*/
    if(oper[strlen(oper)-1]==',')
        oper[strlen(oper)-1]='\0';
    if(distoper[0]==',')
        distoper=distoper+1;
    if(strcmp(distoper,"\0")==0)
        distoper=comma;
    if(distoper[0]==',')
        distoper=distoper+1;
    if(strcmp(distoper,"\0")==0)
        distoper=oper;

    /*  getting command from table */
    order=lookupDict(word);
    /* works for all */
    CODE_IMAGE[IC].flag=1;
    CODE_IMAGE[IC].are=0xA;
    CODE_IMAGE[IC].optype.operator.opcode=OpTable[order].opcode;
    CODE_IMAGE[IC].optype.operator.funct=OpTable[order].funct;
    CODE_IMAGE[IC].optype.operator.dist_add=NO_ADD;
    CODE_IMAGE[IC].optype.operator.src_add=NO_ADD;    
    switch(order){
        case 4:
            if(oper[0]=='#'){
                printf("*** Error in line: %d, Operator -%s- cannot use immediate addresseing as first operand ***\n",statement_cnt,OpTable[order].key);
                *error_flag=3;
                break;
            }
            if(strlen(oper)==2 && oper[0]=='r' && isdigit(oper[1]) && oper[1]>='0' && oper[1]<='7'){
                printf("*** Error in line: %d, Operator -%s- cannot use register direct addresseing as first operand ***\n",statement_cnt,OpTable[order].key);
                *error_flag=3;
                break;
            }
        case 0: case 1: case 2: case 3:
            if(oper[0]=='%'){
                printf("*** Error in line: %d, Operator -%s- cannot use relative addresseing as first operand ***\n",statement_cnt,OpTable[order].key);
                *error_flag=3;
                break;
            }
            if(oper[0]=='#'){
                CODE_IMAGE[IC].optype.operator.src_add=ADD_IMM;
                CODE_IMAGE[IC+1].flag=2;
                CODE_IMAGE[IC+1].optype.src_oper.val.sign=atoi(oper+1);
                CODE_IMAGE[IC+1].are=0xA;
            }
            if(strlen(oper)==2 && oper[0]=='r' && isdigit(oper[1]) && oper[1]>='0' && oper[1]<='7'){
                CODE_IMAGE[IC].optype.operator.src_add=ADD_REG;
                CODE_IMAGE[IC+1].flag=2;
                num=atoi(oper+1);
                switch (num) { 
                    case 0: R=R0; break;    case 1: R=R1; break;
                    case 2: R=R2; break;    case 3: R=R3; break;
                    case 4: R=R4; break;    case 5: R=R5; break;
                    case 6: R=R6; break;    case 7: R=R7; break;
                    default:      break;
                }          
                CODE_IMAGE[IC+1].optype.src_oper.val.sign=R;
                CODE_IMAGE[IC+1].are=0xA;
            }
            cflag=1;
        case 5: case 6: case 7: case 8: case 12:
            if(distoper[0]=='#' && order!=1){
                printf("*** Error in line: %d, Operator -%s- cannot use immediate addresseing as second operand ***\n",statement_cnt,OpTable[order].key);
                *error_flag=3;
                break;
            }
        case 13:
            if(distoper[0]=='%'){
                printf("*** Error in line: %d, Operator -%s- cannot use relative addresseing as second operand ***\n",statement_cnt,OpTable[order].key);
                *error_flag=3;
                break;
            }
            if(strlen(distoper)==2 && distoper[0]=='r' && isdigit(distoper[1]) && distoper[1]>='0' && distoper[1]<='7'){
                CODE_IMAGE[IC].optype.operator.dist_add=ADD_REG;
                CODE_IMAGE[IC+cflag+1].flag=3;
                num=atoi(distoper+1);
                switch (num) { 
                    case 0: R=R0; break;    case 1: R=R1; break;
                    case 2: R=R2; break;    case 3: R=R3; break;
                    case 4: R=R4; break;    case 5: R=R5; break;
                    case 6: R=R6; break;    case 7: R=R7; break;
                    default:      break;
                }          
                CODE_IMAGE[IC+cflag+1].optype.dist_oper.val.sign=R;
                CODE_IMAGE[IC+cflag+1].are=0xA;
            }
            if(distoper[0]=='#'){
                CODE_IMAGE[IC].optype.operator.dist_add=ADD_IMM; 
                CODE_IMAGE[IC+cflag+1].flag=3;
                CODE_IMAGE[IC+cflag+1].optype.dist_oper.val.sign=atoi(distoper+1);
                CODE_IMAGE[IC+cflag+1].are=0xA;                      /*No Break here*/
            }
            break;
    }
	return 0;
}

int makeSecondBinary(int statement_cnt,MachineOrder CODE_IMAGE[],Operand DATA_IMAGE[], int *IC,char word[],char oper[],char distoper[],char comma[],Tlinkptr *head,int *error_flag){
    int order,cflag=0;
    Tlinkptr runner=NULL;
    
    /*  making sure srcoper and distoper have the needed values if needed*/
    if(oper[strlen(oper)-1]==',')
        oper[strlen(oper)-1]='\0';
    if(distoper[0]==',')
        distoper=distoper+1;
    if(strcmp(distoper,"\0")==0)
        distoper=comma;
    if(distoper[0]==',')
        distoper=distoper+1;
    if(strcmp(distoper,"\0")==0)
        distoper=oper;
    
    /*printf("%d \n",*IC);
      getting command from table */
    order=lookupDict(word);
    switch(order){
        case 0: case 1: case 2: case 3: case 4:
            if(!((strlen(oper)==2 && oper[0]=='r' && isdigit(oper[1]) && oper[1]>='0' && oper[1]<='7') || oper[0]=='#' || oper[0]=='%')){
                runner = get_symbol(oper,head);                
                if(runner){
                    CODE_IMAGE[(*IC)].optype.operator.src_add=ADD_DIR;
                    CODE_IMAGE[(*IC)+1].flag=2;          
                    CODE_IMAGE[(*IC)+1].optype.src_oper.val.sign=(*runner).value;
                    if ((*runner).is_extern)
                        CODE_IMAGE[(*IC)+1].are=0xE;
                    else
                        CODE_IMAGE[(*IC)+1].are=0xC;
                } else {
                    printf("*** Error in line: %d, Symbol %s given as first operand wasnt found in symbol table ***\n",statement_cnt,oper);
                    *error_flag=3;
                }
            }
            cflag++;
        case 5: case 6: case 7: case 8: case 9:
        case 10: case 11: case 12: case 13:
            if(!((strlen(distoper)==2 && distoper[0]=='r' && isdigit(distoper[1]) && distoper[1]>='0' && distoper[1]<='7') || distoper[0]=='#' || distoper[0]=='%')){
                runner = get_symbol(distoper,head);                
                if(runner){
                    CODE_IMAGE[(*IC)].optype.operator.dist_add=ADD_DIR;
                    CODE_IMAGE[(*IC)+cflag+1].flag=3;          
                    CODE_IMAGE[(*IC)+cflag+1].optype.dist_oper.val.unsign=(*runner).value;
                    if ((*runner).is_extern)
                        CODE_IMAGE[(*IC)+cflag+1].are=0xE;
                    else
                        CODE_IMAGE[(*IC)+cflag+1].are=0xC;
                } else {
                    printf("*** Error in line: %d, Symbol %s given as second operand wasnt found in symbol table ***\n",statement_cnt,oper);
                    *error_flag=3;
                }
            }
            if((distoper[0]=='%') && (order==9 || order==10 || order==11)){
                runner = get_symbol(distoper+1,head);                
                if(runner){
                    CODE_IMAGE[(*IC)].optype.operator.dist_add=ADD_REL;
                    CODE_IMAGE[(*IC)+1].flag=3;          
                    CODE_IMAGE[(*IC)+1].optype.dist_oper.val.sign=(*runner).value-((*IC)+101);
                    CODE_IMAGE[(*IC)+1].are=0xA;
                } else {
                    printf("*** Error in line: %d, Symbol %s given as operand wasnt found in symbol table ***\n",statement_cnt,oper);
                    *error_flag=3;
                }
            }
            cflag++;
    }
    *IC=*IC+1+cflag;
    return 0;
}

/* add data to data image and DC+(length of data) */
int addBinaryData(MachineOrder CODE_IMAGE[], int IC,Operand DATA_IMAGE[],int DC){
    CODE_IMAGE[IC].are=0xA;
    CODE_IMAGE[IC].flag=2;
    CODE_IMAGE[IC].optype.src_oper.val.sign=DATA_IMAGE[DC].val.sign;  
    return CHANGED;
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