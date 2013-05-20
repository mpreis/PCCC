/*
 * PSEUDOCODE - parser.h
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */
#include "symboltable.h"

struct item_t {
	int mode;
	struct type_t *type;
	int reg;
	int offset;
	int value;
};

int expression(struct item_t *item);
void printError(char *msg);
int validate();
int globalDec();
int statementSeq();
int block();
int paramList();
int typedefDec(struct object_t *head);
int startParsing(char *sfile, char *ofile);
void put(int op, int a, int b, int c);
