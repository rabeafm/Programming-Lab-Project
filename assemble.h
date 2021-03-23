#ifndef _ASSEM_
#define _ASSEM_
/** ------------------------------------------------------------*
 * Main Definitions & Declarations file included in assemble.c  *
 * / files.c / passes.c                                     	*
 *--------------------------------------------------------------*/
#include "data.h"
#include "symbtab.h"

/* RAM size is 4096 i chose to use only 2000 */
/* from which 1800 for Code and 200 for data */
#define MAX_MACHINE_ORDERS 1800  
#define MAX_DATA_ORDERS 200
#define MAX_FILENAME_LENGTH 100
#define MAX_STATEMENT_LENGTH 82
#define MAX_WORD_LENGTH 32
#define MAX_DIGITS 5 /* Max Number of digits in 12 bits - used for adding data */
#define CODE_BEGIN 100
#define DATA_BEGIN 0

/*  files.c Functions   */
int readFile(char *);
int writeFiles(char *);

/*  passes.c Functions   */
int firstPass(char *,int, MachineOrder [], int *, Operand [], int *, Tlinkptr *,int *);
int secondPass(char *,int, MachineOrder [], int *, Operand [], int *, Tlinkptr *, Tlinkptr *, int *);

/*  binary.c Functions   */

/*      first pass       */
int addDataToImage(int, char *, Operand [],int *, int *);
int addStringToImage(int, char *, Operand [],int *, int *);
int makeFirstBinary(int, MachineOrder *, int, char *, char *, char *, int *);

/*      second pass      */
int makeSecondBinary(int ,MachineOrder [], int *, char *, char *, char *,Tlinkptr *,Tlinkptr *, int *);
int addBinaryData(MachineOrder [], int, Operand [], int);

#endif