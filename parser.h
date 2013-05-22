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

/* code generation */
char *srcfile;
char *outfile;
int out_fp_bin;
int out_fp_ass;
/* int nrOfGVar; have a look at symboltable */
int nrOfStrs;
int nrOfCmds;

int GPTR;	/* global pointer */
int LPTR;	/* local pointer */
int HPTR;	/* heap pointer */

int *regs;
int nrOfRegs;
int ITEM_MODE_CONST;
int ITEM_MODE_VAR;
int ITEM_MODE_REF;
int ITEM_MODE_REG;
int ITEM_MODE_NONE;

int OP_NONE;
int OP_ADD;
int OP_SUB;
int OP_MUL;
int OP_DIV;
int OP_AND;
int OP_OR;
int OP_EQ;
int OP_NEQ;
int OP_GT;
int OP_GET;
int OP_LT;
int OP_LET;

void printItem(struct item_t *item);
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
