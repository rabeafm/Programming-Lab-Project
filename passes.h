#include <stdio.h>
#include <stdlib.h>
#include "symbtab.h"

int firstpass(char *,int,int *,int *,Tlinkptr *);
int secondpass(char *,int *,int *);

int isBlank(char *);
int isComment(char *);
int isLabel(char *);
int isData(char *);
int isString(char *);
int isEntry(char *);
int isExtern(char *);


