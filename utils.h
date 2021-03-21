#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define MAX_DIGITS 5
#define NUMBER_OF_ORDERS 16

int isOperandLegal(int, char *, int);
int isDataLegal(int, char *, int);
int isStringLegal(int, char *);
int isBlank(char *);
int isComment(char *);
int isLabel(char *);
int isData(char *);
int isString(char *);
int isEntry(char *);
int isExtern(char *);