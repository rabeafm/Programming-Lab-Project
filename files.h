#include <stdio.h>
#include <stdlib.h>
#include "dictionary.h"
#include "symbtab.h"

/** ------------------------------------------------------------*
 *  File Manipulation Definitions           					*
 *--------------------------------------------------------------*/
#define MAX_MACHINE_ORDERS 1800  /* Based on RAM size 4096 and Each Machine Order is 18 bits aka 2 RAM units */
#define MAX_DATA_ORDERS 200     /* its 2000 from which 1800 to machine and 200 for data */
#define MAX_FILENAME_LENGTH 100
#define MAX_STATEMENT_LENGTH 82
#define CODE_BEGIN 100
#define DATA_BEGIN 0

int readFile(char *);
int writeFiles(char *);
int firstPass(char *,int,MachineOrder [],int *,Operand [],int *,Tlinkptr *,int *);
int secondPass(char *,int,int *,int *,Tlinkptr *,Operand [],MachineOrder [],int *);
