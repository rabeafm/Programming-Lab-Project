#ifndef _UTILS_
#define _UTILS_

#define MAX_RESERVED_WORDS 28 
#define NUMBER_OF_ORDERS 16
#define FALSE 0
#define TRUE 1
#define UPDATE 2
#define ERROR 3

int isStatementLegal(int,char *,char *,char *,char *,int *);
int isLabelLegal(int, char *);
int isOperatorLegal(char *);
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

#endif