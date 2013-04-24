#include "parser.h"
#include "scanner.h"
#include "tokenMapping.h"
int main(int argc, char *argv[]){
	char file[20];
/*	strnCpy(file, "./tests/test.c", 35); */
	strnCpy(file, "parser.c", 35);
	initScanner(file);
	/* scanner test */
	/*	
	while(hasMoreTokens()) {
		getNextToken();
		printToken(symbol);
	}
	*/	
	startParsing();
	printf("\n");
	return 0;
}
