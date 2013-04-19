#include "parser.h"
#include "scanner.h"
#include "tokenMapping.h"
int main(int argc, char *argv[]){
	char file[20];
	strnCpy(file, "./tests/parserTest.c", 35);
	initScanner(file);
	/* scanner test */
	/*
	while(hasMoreTokens()) {
		getNextToken();
		printToken(symbol);
	}
	*/
	startParse();
	printf("\n");
	return 0;
}
