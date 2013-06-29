/* 
 * PSEUDOCODE - tokenMapping.h
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */

struct token_t {
	int id;
	int digitValue;
	int lineNr;
	int colNr;
	char *valueStr;
};
struct type_t {
	int form;
	int size;
	struct object_t *fields; 
	struct type_t *base;
};
struct object_t { 
	char *name;
	int class;
	int offset;
	int scope;
	struct type_t *type; 
	struct object_t *next;
	struct object_t *params;
	int value;
};
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

/************************************** tokenMapping **************************************/
void initTokenMapping();
void printTokenMapping();

int ERROR		; /* = -1;	invalid token	*/
int INIT		; /* =  0;	  */
int LSQBR		; /* =  1;	[ */
int RSQBR		; /* =  2;	] */
int LPAR		; /* =  3;	( */
int RPAR		; /* =  4;	) */
int LCUBR		; /* =  5;	{ */
int RCUBR		; /* =  6;	} */

int SEMCOL		; /* =  7;	; */
int COMMA		; /* =  8;	, */
int DQUOTE		; /* =  9;	" */
int QUOTE		; /* = 10;	' */
int EQSIGN		; /* = 11;	= */
int PLUS		; /* = 12;	+ */
int MINUS		; /* = 13;	- */
int TIMES		; /* = 14;	* */
int DIV			; /* = 15;	/ */

int LT			; /* = 16;	< */
int GT			; /* = 17;	> */
int EQ			; /* = 18;	== */
int NEQ			; /* = 19;	!= */
int LET			; /* = 20;	<= */
int GET			; /* = 21;	>= */
int AND			; /* = 22;	&& */
int OR			; /* = 23;	|| */

int INT			; /* = 30;	key-word: int		*/
int CHAR		; /* = 31;	key-word: char 		*/
int VOID		; /* = 32;	key-word: void 		*/
int STRUCT		; /* = 33;	key-word: struct 	*/
int TYPEDEF		; /* = 34;	key-word: typedef	*/
int BOOL 	    ; /* = 35;	boolean value 		*/
int IDENT		; /* = 36;	identifier 			*/
int NUMBER		; /* = 37;	number value		*/
int STRING		; /* = 38;	string value		*/
int CHARACTER 	; /* = 39;	character value		*/ 

int IF			; /* = 40;	key-word: if		*/
int ELSE		; /* = 41;	key-word: else		*/
int WHILE		; /* = 42;	key-word: while		*/
int RETURN		; /* = 43;	key-word: return	*/ 

int COMMENT		; /* = 50;	comment (this) 		*/
int DOT			; /* = 51;	. 					*/
int INCLUDE		; /* = 52;	#include 			*/
int LF			; /* = 53;	\n 					*/
int END			; /* = 54;	last token			*/
int ARROW		; /* = 55;	->					*/
int SIZEOF		; /* = 56;	size of type		*/
int MALLOC		; /* = 57;	allocate memory		*/
int PRINTF		; /* = 58;  write to output	    */

int OPEN		; /* = 60;  open file       	*/
int CLOSE		; /* = 61;  close file      	*/
int READ		; /* = 62;  read file       	*/
int WRITE		; /* = 63;  write file      	*/
/******************************************************************************************/

/**************************************** scanner *****************************************/
typedef struct token_t *Token;
Token symbol;

void initScanner(char *file);
void closeScanner();

char getChar();
void ungetChar(char c);
int hasMoreChars();
int hasMoreTokens();
void getNextToken();
void initToken();

int getNumber(char nr);
void getIdentifier(char identifier [], char c);

int isLetter(char c);
int isDigit(char c);

int strnCmp(char *s1, char *s2, int n);
int strnCpy(char *s1, char *s2, int n);
void printToken(Token t);
void printSymbol(char *prefix);
/******************************************************************************************/

/************************************** symboltable ***************************************/
typedef char *string_t;

int GLOBAL_SCOPE;
int LOCAL_SCOPE;

int TYPE_FORM_INT;
int	TYPE_FORM_CHAR;
int	TYPE_FORM_VOID;
int	TYPE_FORM_BOOL;
int	TYPE_FORM_ARRAY;
int	TYPE_FORM_RECORD;
int	TYPE_FORM_UNKNOWN;

int	OBJECT_CLASS_VAR;
int	OBJECT_CLASS_TYPE;
int	OBJECT_CLASS_FIELD;
int	OBJECT_CLASS_PROC;

int globOffset;
int locOffset;
int paramOffset;
int nrOfGVar;
char *file;
struct object_t *globList;
struct object_t *locList;

void printObject(struct object_t *ptr);
void printType(struct type_t *t);
void printTable(struct object_t *head);
struct object_t *lookUp(struct object_t *head, string_t name);
struct object_t *findProcedureObject(struct object_t *head, string_t name);
void insertName(struct object_t *ptr, string_t name);
int insert(struct object_t *head, struct object_t *obj);
int insertValues(struct object_t *head, string_t name, int class, struct type_t *type);
struct object_t *delete(struct object_t *head, string_t name);
struct type_t *newType(int form);
void initSymbolTable();
/******************************************************************************************/


/**************************************** parser  *****************************************/
char *srcfile;
char *outfile;
struct cmd_t * *out_cmd_buff;
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
int fJumpChain(int branchAddress);
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
/******************************************************************************************/

void main(){
	char *sfile;
	char *ofile;
	sfile = malloc (20*sizeof(char));
	strnCpy(sfile, "./tests/test.c", 20);

	ofile = malloc (20*sizeof(char));	
	strnCpy(ofile, "./my_pccc", 20); 

	initScanner(sfile);
	startParsing(sfile, ofile);
}
