#ifndef _DATA_
#define _DATA_

/*--------------------------------------------------------------*
 *			   Operator / Operand / Machine Order		   		*
 * Operator, Operand and Machine Order are the main structures  *
 * that hold the code image and the data image.					*
 * Used Mainly in: assemble.c, binary.c, files.c & passes.c		*
 *--------------------------------------------------------------*/

#define INFO_FLAG 0
#define OPERATOR_FLAG 1
#define SRC_OPERAND_FLAG 2
#define DIST_OPERAND_FLAG 3

typedef struct operator {			/* Operator Structure */
	unsigned int dist_add: 2;
	unsigned int src_add: 2;
	unsigned int funct: 4;
	unsigned int opcode: 4;
} Operator;

typedef struct operand {			/* Operand Structure also used for Data */
	union {
		unsigned int unsign: 12;
		signed int sign: 12;
	} val;
} Operand;

typedef struct machineorder {		/* Machine Order - can be operator/operand/infoword */
	union {
		Operator operator;
		Operand  src_oper;
		Operand  dist_oper;
		Operand  information;
	} optype;
	unsigned int are: 4;			
	unsigned int flag: 2;		 /* a flag to define if its Operator/SrcOper/DistOper/Information */
} MachineOrder;

typedef struct operatordict {	/* used for creating the binary OpTable(Dictionary) for data filling by index */
	char key[5];
	unsigned int funct: 4;
	unsigned int opcode: 4;
} OperatorDict;

#endif