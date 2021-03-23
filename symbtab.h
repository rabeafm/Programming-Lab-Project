#ifndef _SYMBTAB_
#define _SYMBTAB_

/*--------------------------------------------------------------*
 *		 Table link pointer / Table link / Table list		   	*
 * Table list - used for symbol (Label) table and extern table. *
 * Table link - holds the values for every symbol (Label).		*
 * Table link pointer - points to a Table link					*
 * Used in: symbtab.c											*
 *--------------------------------------------------------------*/

typedef struct tlink *Tlinkptr;

typedef struct {
	Tlinkptr head;
} Tablelist;

typedef struct tlink {
	char symbol[32];
	unsigned int value : 28;
	unsigned int is_code : 1;
	unsigned int is_data : 1;
	unsigned int is_entry : 1;
	unsigned int is_extern : 1;
	Tlinkptr next;
} Tlink;

/*	Symbol Table Functions */
int addSymbol(int,char *,int *,char *, Tlinkptr *, int *);
void freeTable(Tlinkptr);
Tlinkptr getSymbol(char *,Tlinkptr *);
void updateDataSymbols(int ,Tlinkptr *);
int addExternSymbol(char *,int ,Tlinkptr *, int *);

/*	not used in running but used for writing program  */
void printTable(Tlinkptr);

#endif