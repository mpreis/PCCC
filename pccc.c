/*
 * PSEUDOCODE - pccc.c (pseudocode c compiler)
 * authors: thomas huetter 1120239, mario preishuber 1120643
 */
#include "scanner.h"
#include "parser.h"
int main(int argc, char *argv[]){
	//char *file = "./pccc.c";
	char file[20];
	strnCpy(file, "./pccc.c", 11);
	initScanner(file);
	startParse();
	printf("\n");
	return 0;
}
