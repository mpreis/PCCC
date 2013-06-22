#include "parser.h"
#include "scanner.h"
#include "tokenMapping.h"
#include "./tm/tm.h"

int main(int argc, char *argv[]){
	char *sfile;
	char *ofile;
	int fileNameSize;
	fileNameSize = 20*sizeof(char);
	sfile = malloc (fileNameSize);
	strnCpy(sfile, "./tests/ioTest.c", 20);

	ofile = malloc (fileNameSize);	
	strnCpy(ofile, "./my_pccc", 20); 

	initScanner(sfile);
	startParsing(sfile, ofile);

	startTM("./my_pccc");
	printf("\n");
	return 0;
}
