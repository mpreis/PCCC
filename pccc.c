#include "parser.h"
#include "scanner.h"
#include "tokenMapping.h"

int main(int argc, char *argv[]){
	char *file;
	int fileNameSize;
	fileNameSize = 20*sizeof(char);
	file = malloc (fileNameSize);
	strnCpy(file, "./tests/stTest.c", 35); 
	initScanner(file);
	startParsing(file);
	printf("\n");
	return 0;
}
