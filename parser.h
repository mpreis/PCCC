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
int errorCounter;
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
int STRPTR; /* string pointer */

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

void initOutputFile();
void finalizeOutputFile();
int cg_encode(int op, int a, int b, int c);
void cg_put(int op, int a, int b, int c);
void copyItem(struct item_t *copy, struct item_t *orig);
void initItemModes();
void initOperators();
void cg_initRegs();
int cg_requestReg();
void cg_releaseReg(int r);
void cg_const2Reg(struct item_t *item);
void cg_var2Reg(struct item_t *item);
void cg_ref2Reg(struct item_t *item);
void cg_load(struct item_t *item);
void cg_simpleExpOR(struct item_t* item);
void cg_termOpAND(struct item_t* item);
void cg_simpleExpBinOp(struct item_t *leftItem, struct item_t *rightItem, int op);
void cg_termOperator(struct item_t *leftItem, struct item_t *rightItem, int op);
void cg_expressionOperator(struct item_t *leftItem, struct item_t *rightItem, int op);
void cg_field(struct item_t *item, struct object_t *object);
void cg_index(struct item_t *item, struct item_t *indexItem);
void cg_assignment(struct item_t *leftItem, struct item_t *rightItem);
void cg_allocate(struct item_t *item);
void cg_cJump(struct item_t *item);
int cg_fJump();
void cg_bJump(int backAddress);
void cg_fixUp(int branchAddress);
void cg_fixLink(int branchAddress);
int cg_concatenate(int right, int left);
int cg_negateOperator(int op);
int cg_branch(int op);
void cg_encodeC(int branchAddress, int newC);
int cg_decodeC(int branchAddress);
void cg_loadBool(struct item_t *item);
void cg_unloadBool(struct item_t *item);
void printError(char *msg);
void printItem(struct item_t *item);
int number();
int compOp();
int op();
int identifier();
int typeSpec(struct item_t *item, struct object_t *head);
int include();
int storeString(struct item_t *item);
int sizeOf(struct item_t *item, struct object_t *head);
int selector(struct item_t *item);
int factor(struct item_t *item);
int term(struct item_t *item);
int arithExp(struct item_t *item);
int expression(struct item_t *item);
int paramList();
int printMethod(struct item_t *item);
int fileOpen(struct item_t *item);
int fileClose();
int fileWrite();
int fileRead();
int fileMethod(struct item_t *item);
int ifCmd(struct item_t *item);
int whileLoop(struct item_t *item);
int variableDeclarationSequence(struct object_t *head, int isStruct);
int typedefDec(struct object_t *head);
int structDec();
int returnType(struct item_t *item);
struct object_t *createObject(string_t name);
struct type_t *basicArrayRecordType();
void prologue(int localSize);
void epilogue(int paramSize);
int procedureImplementation(struct item_t* item, string_t identifier);
int formalParameters(struct object_t* object);
struct object_t* createFormalParameter(struct object_t* object, struct type_t* type, string_t identifier);
struct object_t* formalParameter(struct object_t* object, struct object_t* formalParameter);
int procedureReturn();
int fJumpChain(int branchAddress):
int pushUsedRegisters();
void popUsedRegisters(int counter);
int isBSR(int offset);
void procedureCall(struct item_t* item, string_t procName);
int sJump(int branchAddress);
void pushParameter(struct item_t* item);
int actualParameters(struct object_t* object);
struct object_t* createAnonymousParameter(struct object_t* object, struct type_t *type);
struct object_t* actualParameter(struct object_t* object, struct object_t* formalParameter);
int globalDec();
int statementSeq();
int block();
int programm();
int startParsing(char *sfile, char *ofile);
