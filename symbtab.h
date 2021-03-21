#ifndef _SYMBTAB_
#define _SYMBTAB_

#define NOCHANGE 0
#define CHANGED 1
#define UPDATE 2
#define ERROR 3
#define FALSE 0
#define TRUE 1

/* pointer to table link */
typedef struct tlink *Tlinkptr;

typedef struct tlink
{
	char symbol[32];
	unsigned int value : 28;
	unsigned int is_code : 1;
	unsigned int is_data : 1;
	unsigned int is_entry : 1;
	unsigned int is_extern : 1;
	Tlinkptr next;
} Tlink;

typedef struct {
	Tlinkptr head;
} Tlist;

void updateDataSymbols(int ,Tlinkptr *);

int add_symbol(int,char *,int *,char *, Tlinkptr *, int *);

Tlinkptr get_symbol(char *,Tlinkptr *);
void printTable(Tlinkptr);
void freeTable(Tlinkptr);

#endif
