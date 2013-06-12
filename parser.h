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
	int op;
	int fls;
	int tru;
};

struct cmd_t {
	int op;
	int a;
	int b;
	int c;
};

/* code generation */
char *srcfile;
char *outfile;
struct cmd_t * *out_cmd_buff;
/* int nrOfGVar; have a look at symboltable */
int nrOfStrs;
int PC;
int returnFJumpAddress;
int mainPos;
struct object_t *procedureContext;

int RR;		/* return register */
int LINK;	/* link pointer */
int FPTR;	/* frame pointer */

int GPTR;	/* global pointer */
int SPTR;	/* stack pointer */
int HPTR;	/* heap pointer */

int *regs;
int nrOfRegs;
int ITEM_MODE_CONST;
int ITEM_MODE_VAR;
int ITEM_MODE_REF;
int ITEM_MODE_REG;
int ITEM_MODE_COND;
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
void cg_put(int op, int a, int b, int c);
void cg_encodeC(int branchAddress, int newC);
int cg_decodeC(int branchAddress);
int cg_encode(int op, int a, int b, int c);
int cg_negateOperator(int op);
int cg_branch(int op);
void cg_loadBool(struct item_t *item);
void cg_fixLink(int branchAddress);
void cg_unloadBool(struct item_t *item);

/* new methods */
int returnType(struct item_t *item);
struct object_t *createObject(string_t name);
struct type_t *basicArrayRecordType();
int variableDeclarationSequence(struct object_t *head, int isStruct);
int procedureImplementation();
void prologue(int localSize);
void epilogue(int paramSize);
int formalParameters(struct object_t* object);
struct object_t* formalParameter(struct object_t* object, struct object_t* formalParameter);
int procedureReturn();
int fJumpChain(int branchAddress);
// ******PROC CALL*******************************************************
int pushUsedRegisters();
void popUsedRegisters();
int isBSR(int offset);
void procedureCall(struct item_t* item, string_t procName);
int sJump(int branchAddress);
void pushParameter(struct item_t* item);
int actualParameters(struct object_t* object);
struct object_t* createAnonymousParameter(struct object_t* object, struct type_t *type);
struct object_t* actualParameter(struct object_t* object, struct object_t* formalParameter);
