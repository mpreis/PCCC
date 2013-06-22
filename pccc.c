#include "parser.h"
#include "scanner.h"
#include "tokenMapping.h"
#include "./tm/tm.h"

int main(int argc, char *argv[]){
	char *sfile;
	char *ofile;
	sfile = malloc (20*sizeof(char));
	strnCpy(sfile, "./tests/presDemo.c", 20);

	ofile = malloc (20*sizeof(char));	
	strnCpy(ofile, "./my_pccc", 20); 

	initScanner(sfile);
	startParsing(sfile, ofile);

	startTM("./my_pccc");
	printf("\n");
	return 0;
}
