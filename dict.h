
#ifndef _DICT_
#define _DICT_

/*--------------------------------------------------------------*
 *			   Operator / Operand / Machine Order		   		*
 *--------------------------------------------------------------*/
typedef struct operator {
	unsigned int dist_add: 2;
	unsigned int src_add: 2;
	unsigned int funct: 4;
	unsigned int opcode: 4;
	unsigned int are :4;
} Operator;

typedef struct operand {
	union {
		unsigned int uval: 12;
		signed int sval: 12;
	} val;
	unsigned int are: 4;	/* .data .string .extern*/
} Operand;

typedef struct machineorder {
	Operator operator;
	Operand  src_data_oper;	/* The content can be code or data - union word {	data_word *data;	code_word *code; } word; data or code flag*/
	Operand  dist_oper;
} MachineOrder;

typedef struct operatordict {
	char key[5];
	Operator op;
} OperatorDict;

/*----------------------------------------------------------------*
 * Orders Opcodes / Functs / Address Modes / Registers Dictionary *
 *----------------------------------------------------------------*/
typedef enum {
	/*	First Group	- Two Operands - Z-Flag in PSW	 */
	MOV = 0,  CMP = 1, ADD = 2, SUB = 2,  LEA = 4,
	/*	Second Group - One Operand - SRC Operand = 0 */
	CLR = 5,  NOT = 5, INC = 5, DEC = 5,  
	JMP = 9,  BNE = 9, JSR = 9, 
	RED = 12, PRN = 13, 
	/*	Third Group	- SRC & Dis Operand = 0			 */
	RTS = 14, STOP = 15
} OpCodeDict;

typedef enum {
	FMOV = 0,  FCMP = 0,  FADD = 10, FSUB = 11,  FLEA = 0,
	FCLR = 10, FNOT = 11, FINC = 12, FDEC = 13,
	FJMP = 10, FBNE = 11, FJSR = 12,
	FRED = 0,  FPRN = 0,  FRTS = 0,  FSTOP = 0
} FunctsDict;

typedef enum {
	AIMM = 0,	/* 	Immediate 	*/
	ADIR = 1,	/* 	Direct 		*/
	AREL = 2,	/*	Relative 	*/
	AREG = 3	/* 	Register 	*/
} AddrModes;

typedef enum {
	R0 = 1,  R1 = 2,  R2 = 4,  R3 = 8,
	R4 = 16, R5 = 32, R6 = 64, R7 = 128
} RegDict;

/*----------------------------------------------*
 *							*
 *----------------------------------------------*/



#endif






