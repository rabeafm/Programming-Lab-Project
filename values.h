#ifndef _DICT_
#define _DICT_

/*----------------------------------------------------------------*
 * Orders Opcodes / Functs / Address Modes / Registers Dictionary *
 * Operator Dictionary is used to hold in the data related to its *
 * translation to binary code.									  *
 * Used in: binary.c										  *
 *----------------------------------------------------------------*/

typedef enum {								
	MOV = 0,  CMP = 1, ADD = 2, SUB = 2,  LEA = 4,	/*			Two Operands			*/		
	CLR = 5,  NOT = 5, INC = 5, DEC = 5, 			/* One Operand - SRC Operand = 0	*/
	JMP = 9,  BNE = 9, JSR = 9, RED = 12, PRN = 13, 
	RTS = 14, STOP = 15								/*		SRC & Dis Operand = 0		*/
} OpCodeValues;

typedef enum {
	FMOV = 0,  FCMP = 0,  FADD = 10, FSUB = 11,  FLEA = 0,
	FCLR = 10, FNOT = 11, FINC = 12, FDEC = 13,
	FJMP = 10, FBNE = 11, FJSR = 12,
	FRED = 0,  FPRN = 0,  FRTS = 0,  FSTOP = 0
} FunctsValues;

typedef enum {
	NO_ADD =0,
	ADD_IMM = 0,	/* 	Immediate 	*/
	ADD_DIR = 1,	/* 	Direct 		*/
	ADD_REL = 2,	/*	Relative 	*/
	ADD_REG = 3		/* 	Register 	*/
} AddressModesValues;

typedef enum {
	R0 = 1,  R1 = 2,  R2 = 4,  R3 = 8,
	R4 = 16, R5 = 32, R6 = 64, R7 = 128
} RegistersValues;

#endif