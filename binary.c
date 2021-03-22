/** ------------------------------------------------------------*
 * This file is the most important file, because the functions  *
 * here are responsible for the creation of the Machine Code.    *
 *--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "assemble.h"
#include "data.h"
#include "utils.h"
#include "symbtab.h"
#include "values.h"

/** ------------------------------------------------------------*
 * The OpTable is an array of structures used for easier data   *
 * insertion relying on the operator number, when its elements  *
 * are an Order, a Funct and an OpCode.                         *
 *--------------------------------------------------------------*/
OperatorDict OpTable[]={  /*Order  Funct OpCode */
	{ "mov", FMOV, MOV }, { "cmp", FCMP, CMP }, { "add", FADD, ADD }, { "sub", FSUB, SUB },
    { "lea", FLEA, LEA }, { "clr", FCLR, CLR },	{ "not", FNOT, NOT }, { "inc", FINC, INC },
    { "dec", FDEC, DEC }, { "jmp", FJMP, JMP }, { "bne", FBNE, BNE }, { "jsr", FJSR, JSR },
	{ "red", FRED, RED }, { "prn", FPRN, PRN }, { "rts", FRTS, RTS }, { "stop", FSTOP, STOP}
};

/******************** first pass functions *********************/
/** ------------------------------------------------------------*
 *  Receives operand/operands as a string of numbers checks if  *
 *  the operands statement is legal, if so divides them every   *
 *  operand on its own and checks if its legal,if so adds to    * 
 *  Data Image.                                                 *
 *  Gives Errors when needed and signals the error flag ERROR(3)*
 *                                                              *
 *  @param	statement_cnt   Statement counter					*
 *  @param	operands		String of numbers                   *
 *  @param	DATA_IMAGE		Array of unions - operands image    *
 *  @param	DC				Pointer to data counter+(# operands)*
 *  @param	error_flag		Pointer to error flag for later stop*
 *  @return TRUE(1) if change was made or else FALSE(0)			*
 *--------------------------------------------------------------*/
int addDataToImage(int statement_cnt,char *operands,Operand DATA_IMAGE[],int *DC,int *error_flag){
   	int i,j,n;                          /* n is used as a backward length index (from end to start) */
    int no_sign_flag=FALSE,no_space_flag=FALSE,no_comma_flag=FALSE;  /* no sign/space/comma allowed if TRUE */
    char param[MAX_DIGITS];
    char nums[MAX_STATEMENT_LENGTH];

    /*  Trimming Begining and End of operand/operands */
    strcpy(nums,operands+strspn(operands," \t"));
    for (n=strlen(nums)-1; n>=0 ; n--)
        if(nums[n]!=' ' && nums[n]!='\t' && nums[n]!='\n')
            break;
    nums[n+1] = '\0';

    /*  check if data statement is legal    */
    if(isDataLegal(statement_cnt,nums,n)){
        for(i=0,j=0;i<=n+1;i++){
            if(nums[i]=='-' || nums[i]=='+'){   
                if(no_sign_flag==TRUE){               /* Checks for two following signs or a sign after a digit */
                    printf("*** Error in line: %d, you cant use multiple signs consequently or between digits ***\n",statement_cnt);
                    return FALSE;
                }
                no_space_flag=TRUE;
                no_sign_flag=TRUE;
                param[j]=nums[i];
                param[j+1]='\0';
                j++;
            } else {
                if(isdigit(nums[i])){               /* Checks for spaces between digits or between a sign and a digit */
                    if(no_space_flag==TRUE && (nums[i-1]==' ' || nums[i-1]=='\t')){
                        printf("*** Error in line: %d, you cant use spaces and \\t between digits of the same number or between a sign and a digit ***\n",statement_cnt);
                        return FALSE;
                    }
                    no_comma_flag=FALSE;
                    no_space_flag=TRUE;
                    no_sign_flag=TRUE;
                    param[j]=nums[i];
                    param[j+1]='\0';
                    j++;
                } else {
                    if(nums[i]==',' || nums[i]=='\0'){  /* Checks for two following commas */
                        if(no_comma_flag==TRUE)
                            printf("*** Error in line: %d, you cant use multiple commas consequently there must be digits between commas ***\n",statement_cnt);
                        no_comma_flag=TRUE;
                        no_space_flag=FALSE;
                        no_sign_flag=FALSE;
                        j=0;
                        DATA_IMAGE[(*DC)].val.sign=atoi(param);
                        (*DC)++;
                    }
                }
            }
        }
        return TRUE;
    }
    return FALSE;
}

/** ------------------------------------------------------------*
 *  Receives operands/operands as a string of characters checks *
 *  if operands statement is legal, if so adds to Data Image.   *
 *  Gives Errors when needed and signals the error flag ERROR(3)*
 *                                                              *
 *  @param	statement_cnt   Statement counter					*
 *  @param	operands		String of characters                *
 *  @param	DATA_IMAGE		Array of unions - operands image    *
 *  @param	DC				Pointer to data counter	+ (str len) *
 *  @param	error_flag		Pointer to error flag for later stop*
 *  @return TRUE(1) if change was made or else FALSE(0)			*
 *--------------------------------------------------------------*/
int addStringToImage(int statement_cnt,char *operands,Operand DATA_IMAGE[],int *DC, int *error_flag){
    char *pi;           /* Pointer Index */ 
    char *last_char;    
    if(isStringLegal(statement_cnt,operands)){
        pi=strchr(operands,'\"')+1;
        last_char=strrchr(operands,'\"');
        while(pi<last_char){
            DATA_IMAGE[(*DC)].val.unsign=*pi;
            (*DC)++;
            pi++;
        }
        DATA_IMAGE[(*DC)].val.unsign='\0';
        (*DC)++;
        return TRUE;
    }
    return FALSE;  
}

/** ------------------------------------------------------------*
 *  Make binary code of the first word and every infoword in    *
 *  immediate and register direct addressing modes.             *
 *  Gives Errors when needed and signals the error flag ERROR(3)*
 *                                                              *
 *  @param	statement_cnt   statement counter					*
 *  @param	CODE_IMAGE		Array of structs representing orders*
 *  @param	IC				Pointer to code counter				*
 *  @param	word		    string with assembly word           *
 *  @param	oper			string with src operand if theres 2 *
 *                          operands or dist operand if theres 1*
 *  @param 	distoper		string with dist operand if theres 2*
 *  @param	error_flag		Pointer to error flag for later stop*
 *  @return TRUE(1) if change was made or else FALSE(0)			*
 *--------------------------------------------------------------*/
int makeFirstBinary(int statement_cnt, MachineOrder CODE_IMAGE[], int IC,char word[],char oper[],char distoper[],int *error_flag){
    int order,num,reg,cflag=0;
    IC-=CODE_BEGIN;

   

    /*  getting command from table */
    order=isOperatorLegal(word);
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
                    case 0: reg=R0; break;    case 1: reg=R1; break;
                    case 2: reg=R2; break;    case 3: reg=R3; break;
                    case 4: reg=R4; break;    case 5: reg=R5; break;
                    case 6: reg=R6; break;    case 7: reg=R7; break;
                    default:      break;
                }          
                CODE_IMAGE[IC+1].optype.src_oper.val.sign=reg;
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
                    case 0: reg=R0; break;    case 1: reg=R1; break;
                    case 2: reg=R2; break;    case 3: reg=R3; break;
                    case 4: reg=R4; break;    case 5: reg=R5; break;
                    case 6: reg=R6; break;    case 7: reg=R7; break;
                    default:      break;
                }          
                CODE_IMAGE[IC+cflag+1].optype.dist_oper.val.sign=reg;
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
int makeSecondBinary(int statement_cnt,MachineOrder CODE_IMAGE[],Operand DATA_IMAGE[], int *IC,char word[],char oper[],char distoper[],char comma[],Tlinkptr *head,Tlinkptr *xhead,int *error_flag){
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
    order=isOperatorLegal(word);
    switch(order){
        case 0: case 1: case 2: case 3: case 4:
            if(!((strlen(oper)==2 && oper[0]=='r' && isdigit(oper[1]) && oper[1]>='0' && oper[1]<='7') || oper[0]=='#' || oper[0]=='%')){
                runner = get_symbol(oper,head);                
                if(runner){
                    CODE_IMAGE[(*IC)].optype.operator.src_add=ADD_DIR;
                    CODE_IMAGE[(*IC)+1].flag=2;          
                    CODE_IMAGE[(*IC)+1].optype.src_oper.val.sign=(*runner).value;
                    if ((*runner).is_extern){
                        addExternSymbol(oper,(*IC)+1,xhead,error_flag);
                        CODE_IMAGE[(*IC)+1].are=0xE;
                    }
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
                    if ((*runner).is_extern){
                        addExternSymbol(distoper,(*IC)+cflag+1,xhead,error_flag);
                        CODE_IMAGE[(*IC)+cflag+1].are=0xE;
                    } else
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
    return TRUE;
}




