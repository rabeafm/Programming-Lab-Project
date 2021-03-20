#include <stdio.h>
#include <stdlib.h>
#include "symbtab.h"
#include "dict.h"

int firstpass(char *,int,int *,int *,Tlinkptr *,Operand [],MachineOrder [],int *);
int secondpass(char *,int *,int *);
int add_data_to_image(int, char *, Operand [],int *, int *);
int add_string_to_image(int, char *, Operand [],int *, int *);
int analayzeStatement(int ,char *,char *,char *,char *,int *);
int makeFirstBinary(MachineOrder *, int, int, int, char *, char *, char *, char *, int *);


int LookupinDict(char []);
int isBlank(char *);
int isComment(char *);
int isLabel(char *);
int isData(char *);
int isString(char *);
int isEntry(char *);
int isExtern(char *);


