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
#include "symbtab.h"
#include "utils.h"
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
 *  After the first pass, Make binary code of the first word &  *
 *  infowords in immediate and register direct addressing modes.*
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
 *  @return TRUE(1) if change was made, FALSE(0) if found Errors*
 *--------------------------------------------------------------*/
int makeFirstBinary(int statement_cnt, MachineOrder CODE_IMAGE[], int IC,char word[],char oper[],char distoper[],int *error_flag){
    int order,reg,src_flag=FALSE;       /* reg is used for passing the register code*/
                                        /* src_flag used for correcting next line for dist operand if operator recieves 2 operands*/
    IC-=CODE_BEGIN;
    
    /* getting order number from table */
    order=isOperatorLegal(word);
    /* Filling Data for All */
    CODE_IMAGE[IC].flag=OPERATOR_FLAG;
    CODE_IMAGE[IC].are=0xA;         /* Using Hexa for are A=Absolute C=Relocatble E=External */
    CODE_IMAGE[IC].optype.operator.opcode=OpTable[order].opcode;
    CODE_IMAGE[IC].optype.operator.funct=OpTable[order].funct;
    CODE_IMAGE[IC].optype.operator.dist_add=NO_ADD;
    CODE_IMAGE[IC].optype.operator.src_add=NO_ADD;
    /* Updating different cases based on operand */
    switch(order){
        case 4:             /* Special case 4 Errors*/ 
            if(oper[0]=='#'){
                printf("*** Error in line: %d, Operator %s cannot use immediate addresseing as first operand ***\n",statement_cnt,OpTable[order].key);
                *error_flag=ERROR;
                break;
            }
            if(strlen(oper)==2 && oper[0]=='r' && isdigit(oper[1]) && oper[1]>='0' && oper[1]<='7'){
                printf("*** Error in line: %d, Operator %s cannot use register direct addresseing as first operand ***\n",statement_cnt,OpTable[order].key);
                *error_flag=ERROR;
                break;
            }               /* Mutual for cases 0,1,2,3,4 */
        case 0: case 1: case 2: case 3:
            if(oper[0]=='%'){
                printf("*** Error in line: %d, Operator %s cannot use relative addresseing as first operand ***\n",statement_cnt,OpTable[order].key);
                *error_flag=ERROR;
                break;
            }
            if(oper[0]=='#'){
                CODE_IMAGE[IC].optype.operator.src_add=ADD_IMM;
                CODE_IMAGE[IC+1].flag=SRC_OPERAND_FLAG;
                CODE_IMAGE[IC+1].optype.src_oper.val.sign=atoi(oper+1);
                CODE_IMAGE[IC+1].are=0xA;
            }
            if(strlen(oper)==2 && oper[0]=='r' && isdigit(oper[1]) && oper[1]>='0' && oper[1]<='7'){
                switch(atoi(oper+1)){ 
                    case 0: reg=R0; break;    case 1: reg=R1; break;
                    case 2: reg=R2; break;    case 3: reg=R3; break;
                    case 4: reg=R4; break;    case 5: reg=R5; break;
                    case 6: reg=R6; break;    case 7: reg=R7; break;
                }
                CODE_IMAGE[IC].optype.operator.src_add=ADD_REG;
                CODE_IMAGE[IC+1].flag=SRC_OPERAND_FLAG;          
                CODE_IMAGE[IC+1].optype.src_oper.val.sign=reg;
                CODE_IMAGE[IC+1].are=0xA;
            }
            src_flag=TRUE;
        case 5: case 6: case 7: case 8: case 12:    
            if(distoper[0]=='#' && order!=1){       /* Mutual for cases 0,2,3,4,5,6,7,8,12 */
                printf("*** Error in line: %d, Operator %s cannot use immediate addresseing as second operand ***\n",statement_cnt,OpTable[order].key);
                *error_flag=ERROR;
                break;
            }
        case 13:                                    /* Mutual for cases 0,1,2,3,4,5,6,7,8,12,13 */
            if(distoper[0]=='%'){                   
                printf("*** Error in line: %d, Operator %s cannot use relative addresseing as second operand ***\n",statement_cnt,OpTable[order].key);
                *error_flag=ERROR;
                break;
            }
            if(strlen(distoper)==2 && distoper[0]=='r' && isdigit(distoper[1]) && distoper[1]>='0' && distoper[1]<='7'){
                switch(atoi(distoper+1)){ 
                    case 0: reg=R0; break;    case 1: reg=R1; break;
                    case 2: reg=R2; break;    case 3: reg=R3; break;
                    case 4: reg=R4; break;    case 5: reg=R5; break;
                    case 6: reg=R6; break;    case 7: reg=R7; break;
                } 
                CODE_IMAGE[IC].optype.operator.dist_add=ADD_REG;
                CODE_IMAGE[IC+src_flag+1].flag=DIST_OPERAND_FLAG;         
                CODE_IMAGE[IC+src_flag+1].optype.dist_oper.val.sign=reg;
                CODE_IMAGE[IC+src_flag+1].are=0xA;
            }
            if(distoper[0]=='#'){
                CODE_IMAGE[IC].optype.operator.dist_add=ADD_IMM; 
                CODE_IMAGE[IC+src_flag+1].flag=DIST_OPERAND_FLAG;
                CODE_IMAGE[IC+src_flag+1].optype.dist_oper.val.sign=atoi(distoper+1);
                CODE_IMAGE[IC+src_flag+1].are=0xA;
            }
    }
    if(*error_flag==ERROR)
        return FALSE;
    return TRUE;
}

/******************** second pass functions *********************/
/** ------------------------------------------------------------*
 *  After the second pass, Make binary code of the operands in  *
 *  case of direct or relational addressing modes.              *
 *  Gives Errors when needed and signals the error flag ERROR(3)*
 *                                                              *
 *  @param	statement_cnt   statement counter					*
 *  @param	CODE_IMAGE		Array of structs representing orders*
 *  @param	IC				Pointer to code counter				*
 *  @param	word		    string with assembly word           *
 *  @param	oper			string with src operand if theres 2 *
 *                          operands or dist operand if theres 1*
 *  @param 	distoper		string with dist operand if theres 2*
 *  @param  head            head of symbols table (linkedlist)  *
 *  @param  extern_head     head of externs table (linkedlist)  *
 *  @param	error_flag		Pointer to error flag for later stop*
 *  @return TRUE(1) if change was made, FALSE(0) if found Errors*
 *--------------------------------------------------------------*/
int makeSecondBinary(int statement_cnt,MachineOrder CODE_IMAGE[], int *IC,char word[],char oper[],char distoper[],Tlinkptr *head,Tlinkptr *extern_head,int *error_flag){
    int order,operand_cnt=0;    /* Operand Count */
    Tlinkptr link;              /* a link in the symbols linked list */
        
    /* getting command from table */
    order=isOperatorLegal(word);
    switch(order){
        case 0: case 1: case 2: case 3: case 4:     /* label case with direct addressing for source operand */
            if(!((strlen(oper)==2 && oper[0]=='r' && isdigit(oper[1]) && oper[1]>='0' && oper[1]<='7') || oper[0]=='#' || oper[0]=='%')){
                link = getSymbol(oper,head);                
                if(link){
                    CODE_IMAGE[(*IC)].optype.operator.src_add=ADD_DIR;
                    CODE_IMAGE[(*IC)+1].flag=SRC_OPERAND_FLAG;          
                    CODE_IMAGE[(*IC)+1].optype.src_oper.val.unsign=(*link).value;
                    if ((*link).is_extern){
                        addExternSymbol(oper,(*IC)+1,extern_head,error_flag);   /* add to external table */
                        CODE_IMAGE[(*IC)+1].are=0xE;    /*External */
                    }
                    else
                        CODE_IMAGE[(*IC)+1].are=0xC;    /*Relocatble */
                } else {
                    printf("*** Error in line: %d, Symbol %s given as first operand wasn't found in symbol table ***\n",statement_cnt,oper);
                    *error_flag=ERROR;
                }
            }
            operand_cnt++;
        case 5: case 6: case 7: case 8: case 9:
        case 10: case 11: case 12: case 13:         /* label case with direct addressing for destination operand */
            if(!((strlen(distoper)==2 && distoper[0]=='r' && isdigit(distoper[1]) && distoper[1]>='0' && distoper[1]<='7') || distoper[0]=='#' || distoper[0]=='%')){
                link = getSymbol(distoper,head);                
                if(link){
                    CODE_IMAGE[(*IC)].optype.operator.dist_add=ADD_DIR;
                    CODE_IMAGE[(*IC)+operand_cnt+1].flag=DIST_OPERAND_FLAG;          
                    CODE_IMAGE[(*IC)+operand_cnt+1].optype.dist_oper.val.unsign=(*link).value;
                    if ((*link).is_extern){
                        addExternSymbol(distoper,(*IC)+operand_cnt+1,extern_head,error_flag);  /* add to external table */
                        CODE_IMAGE[(*IC)+operand_cnt+1].are=0xE;        /*External */
                    } else
                        CODE_IMAGE[(*IC)+operand_cnt+1].are=0xC;        /*Relocatble */
                } else {
                    printf("*** Error in line: %d, Symbol %s given as second operand wasn't found in symbol table ***\n",statement_cnt,oper);
                    *error_flag=ERROR;
                }
            }
            if((distoper[0]=='%') && (order==9 || order==10 || order==11)){  /* label case with relative addressing for destination operand */
                link = getSymbol(distoper+1,head);                
                if(link && (*link).is_extern==FALSE){
                    CODE_IMAGE[(*IC)].optype.operator.dist_add=ADD_REL;
                    CODE_IMAGE[(*IC)+1].flag=DIST_OPERAND_FLAG;          
                    CODE_IMAGE[(*IC)+1].optype.dist_oper.val.sign=(*link).value-((*IC)+101);
                    CODE_IMAGE[(*IC)+1].are=0xA;                    /*Absolute */
                } else {
                    printf("*** Error in line: %d, Symbol %s given as operand wasnt found in symbol table ***\n",statement_cnt,oper);
                    *error_flag=ERROR;
                }
            }
            operand_cnt++;
    }
    if(*error_flag==ERROR)
        return FALSE;
    *IC=*IC+1+operand_cnt;
    return TRUE;
}

/** ------------------------------------------------------------*
 *  Transfers data from Data Image to the end of Code Image.    *
 *                                                              *
 *  @param	CODE_IMAGE		Array of structs representing orders*
 *  @param	IC				Code counter(counter-100) for index *
 *  @param	DATA_IMAGE		Array of unions - operands image    *
 *  @param	DCF 		    Total Code Counter                  *
 *  @return TRUE(1) if change was made, FALSE(0) if found Errors*
 *--------------------------------------------------------------*/
int addBinaryData(MachineOrder CODE_IMAGE[], int IC, Operand DATA_IMAGE[], int DCF){
    int d=0;
    while(d<DCF){
    	CODE_IMAGE[IC].are=0xA;
    	CODE_IMAGE[IC].flag=INFO_FLAG;
    	CODE_IMAGE[IC].optype.information.val.sign=DATA_IMAGE[d].val.sign;  
		IC++;
		d++;
	}
    return TRUE;
}