#ifndef _DATA_
#define _DATA_
/*--------------------------------------------------------------*
 *			   Operator / Operand / Machine Order		   		*
 * Operator, Operand and Machine Order are the main structures  *
 * that hold the code image and the data image.					*
 * Used Mainly in: assemble.c, files.c & passes.c				*
 *--------------------------------------------------------------*/
/*1- Abstraction of data structures: splitting data structures access and implementation*/


typedef struct operator {
	unsigned int dist_add: 2;
	unsigned int src_add: 2;
	unsigned int funct: 4;
	unsigned int opcode: 4;
} Operator;

typedef struct operand {
	union {
		unsigned int unsign: 12;
		signed int sign: 12;
	} val;
} Operand;

typedef struct machineorder {
	union {
		Operator operator;
		Operand  src_oper;
		Operand  dist_oper;
	} optype;
	unsigned int are: 4;
	unsigned int flag: 2;
} MachineOrder;

typedef struct operatordict {
	char key[5];
	unsigned int funct: 4;
	unsigned int opcode: 4;
} OperatorDict;

#endif