/*
 * PSEUDOCODE - pccc.c (pseudocode c compiler)
 * authors: thomas huetter 1120239, mario preishuber 1120643
 */
#include "scanner.h"
int main(int argc, char *argv[]){
	Token t;
	char *file = "./pccc.c";
	initScanner(file);
	/* scanner test */ 
	while ( hasMoreTokens() ) {	
		t = getNextToken();
		printToken(&t);
	}
	printf("\n");
	return 0;
}
