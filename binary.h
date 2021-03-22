#include <stdio.h>
#include <stdlib.h>
#include "dictionary.h"
#include "symbtab.h"



int add_data_to_image(int, char *, Operand [],int *, int *);
int add_string_to_image(int, char *, Operand [],int *, int *);
int analayzeStatement(int ,char *,char *,char *,char *,int *);
int makeFirstBinary(MachineOrder *, int, int, int, char *, char *, char *, char *, int *);
int makeSecondBinary(int ,MachineOrder [],Operand [], int *, char *, char *, char *, char *,Tlinkptr *,Tlinkptr *, int *);
int addBinaryData(MachineOrder [],int, Operand [], int);
int lookupDict(char []);



