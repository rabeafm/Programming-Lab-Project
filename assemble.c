/*--------------------------------------------------------------*
 *						   Introduction		   					*
 * 	fix make file before giving project remove o and x folders	*
 *--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "files.h"

/** ------------------------------------------------------------*
 *  Recieves an array of string from command line handles them	*
 *  one by one calling the read file function and if read was 	*
 *  successful calls the write files function					*
 *--------------------------------------------------------------*/
int main(int argc, char *argv[]) {
	while (*++argv){
		if(readFile(*argv)){
			writeFiles(*argv);
		}
	}
	return 1;
}