/* 
 * PSEUDOCODE (SELFCOMPILING) C COMPILER
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */
struct object_t;
struct type_t;
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

/******************************************************************************************/
/************************************** targetMachine Commands ****************************/
/******************************************************************************************/
void initTMCmd();
char *getCmdName(int cmd_id);
/* immediate addressing */
int CMD_ADDI; //= 10;
int CMD_SUBI; //= 11;
int CMD_MULI; //= 12;
int CMD_DIVI; //= 13;
int CMD_MODI; //= 14;
int CMD_CMPI; //= 15;

/* register addressing */
int CMD_ADD; //= 20;
int CMD_SUB; //= 21;
int CMD_MUL; //= 22;
int CMD_DIV; //= 23;
int CMD_MOD; //= 24;
int CMD_CMP; //= 25;

/* memory: load and store */
int CMD_LDW; //= 30;
int CMD_STW; //= 31;

/* stack operations */
int CMD_POP; //= 40;
int CMD_PSH; //= 41;

/* conditional branching */
int CMD_BEQ; //= 50;
int CMD_BGE; //= 51;
int CMD_BGT; //= 52;
int CMD_BLE; //= 53;
int CMD_BLT; //= 54;
int CMD_BNE; //= 55;

/* unconditional branching */
int CMD_BR ; //= 56;
int CMD_BSR; //= 57;
int CMD_JSR; //= 58;
int CMD_RET; //= 59;

/* i/o operations */
int CMD_FLO; //= 60;
int CMD_FLC; //= 61;
int CMD_RDC; //= 62;
int CMD_WRC; //= 63;

/* end of code */
int CMD_TRAP; //= 1;

/* meta data */
int CMD_GP;  // = 1;	/* global pointer */
int CMD_SP;  // = 3; 	/* string pointer */
int CMD_CS;  // = 2; 	/* code size */
int CMD_MAL; // = 4;	/* malloc */
int CMD_PRN; // = 5;  /* print given value as integer */
int CMD_PRC; // = 6;  /* print given value as char */
/******************************************************************************************/
/************************************** tokenMapping **************************************/
/******************************************************************************************/
void initTokenMapping();

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
/******************************************************************************************/
int fd;
int lineNr;
int colNr;
char ungetc;
struct token_t *symbol;

void initScanner(char *file);
void closeScanner();

char getChar();
void ungetChar(char c);
bool hasMoreChars();
bool hasMoreTokens();
void getNextToken();
void initToken();

int getNumber(char pnr);
void getIdentifier(char *identifier, char c);

bool isLetter(char c);
bool isDigit(char c);

int strCmp(char *s1, char *s2);
int strnCpy(char *s1, char *s2, int n);
/******************************************************************************************/
/************************************** symboltable ***************************************/
/******************************************************************************************/
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

struct object_t *lookUp(struct object_t *head, string_t name);
struct object_t *findProcedureObject(struct object_t *head, string_t name);
void insertName(struct object_t *ptr, string_t name);
int insert(struct object_t *head, struct object_t *obj);
struct object_t *delete(struct object_t *head, string_t name);
struct type_t *newType(int form);
void initSymbolTable();
/******************************************************************************************/
/**************************************** parser  *****************************************/
/******************************************************************************************/
char *srcfile;
char *outfile;

int *out_cmd_op;
int *out_cmd_a;
int *out_cmd_b;
int *out_cmd_c;

int nrOfStrs;
int errorCounter;
int PC;
int returnFJumpAddress;
int mainPos;
int typeWA;
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
bool number();
bool compOp();
bool op();
bool identifier();
int typeSpec(struct item_t *item, struct object_t *head);
bool include();
bool storeString(struct item_t *item);
bool sizeOf(struct item_t *item, struct object_t *head);
bool selector(struct item_t *item);
bool factor(struct item_t *item);
bool term(struct item_t *item);
bool arithExp(struct item_t *item);
bool expression(struct item_t *item);
bool printMethod(struct item_t *item);
bool fileOpen(struct item_t *item);
bool fileClose(struct item_t *item);
bool fileWrite(struct item_t *item);
bool fileRead(struct item_t *item);
bool ifCmd(struct item_t *item);
bool whileLoop(struct item_t *item);
int variableDeclarationSequence(struct object_t *head, int isStruct);
bool typedefDec(struct object_t *head);
bool structDec();
struct object_t *createObject(string_t name);
struct type_t *basicArrayRecordType();
void prologue(int localSize);
void epilogue(int paramSize);
bool procedureImplementation(struct item_t* item, string_t identifier);
bool formalParameters(struct object_t* object);
struct object_t* createFormalParameter(struct object_t* object, struct type_t* type, string_t identifier);
struct object_t* formalParameter(struct object_t* object, struct object_t* formalParameter);
bool procedureReturn();
bool fJumpChain(int branchAddress);
bool pushUsedRegisters();
void popUsedRegisters(int counter);
bool isBSR(int offset);
void procedureCall(struct item_t* item, string_t procName);
bool sJump(int branchAddress);
void pushParameter(struct item_t* item);
bool actualParameters(struct object_t* object);
struct object_t* createAnonymousParameter(struct object_t* object, struct type_t *type);
struct object_t* actualParameter(struct object_t* object, struct object_t* formalParameter);
bool globalDec();
bool statementSeq();
bool block();
bool programm();
bool startParsing(char *sfile, char *ofile);
void printString(char *str);
/******************************************************************************************/

void initTokenMapping() {
	ERROR		= -1;	/* invalid token	*/
	INIT		=  0;
	LSQBR		=  1;	/* [ */
	RSQBR		=  2;	/* ] */
	LPAR		=  3;	/* ( */
	RPAR		=  4;	/* ) */
	LCUBR		=  5;	/* { */
	RCUBR		=  6;	/* } */

	SEMCOL		=  7;	/* ; */
	COMMA		=  8;	/* , */
	DQUOTE		=  9;	/* " */
	QUOTE		= 10;	/* ' */
	EQSIGN		= 11;	/* = */
	PLUS		= 12;	/* + */
	MINUS		= 13;	/* - */
	TIMES		= 14;	/* * */
	DIV			= 15;	/* / */

	LT			= 16;	/* < */
	GT			= 17;	/* > */
	EQ			= 18;	/* == */
	NEQ			= 19;	/* != */
	LET			= 20;	/* <= */
	GET			= 21;	/* >= */
	AND			= 22;	/* && */
	OR			= 23;	/* || */

	INT			= 30;	/* key-word: 		*/
	CHAR		= 31;	/* key-word: char 	*/
	VOID		= 32;	/* key-word: void 	*/
	STRUCT		= 33;	/* key-word: struct */
	TYPEDEF		= 34;	/* key-word: typedef*/
	BOOL	 	= 35;	/* boolean value 	*/
	IDENT		= 36;	/* identifier 		*/
	NUMBER		= 37;	/* number value		*/
	STRING		= 38;	/* string value		*/
	CHARACTER 	= 39;	/* character value	*/

	IF			= 40;	/* key-word: if		*/
	ELSE		= 41;	/* key-word: else	*/
	WHILE		= 42;	/* key-word: while	*/
	RETURN		= 43;	/* key-word: return	*/

	COMMENT		= 50;	/* comment (this) 	*/
	DOT			= 51;	/* . 				*/
	INCLUDE		= 52;	/* #include 		*/
	LF			= 53;	/* \n 				*/
	END			= 54;	/* last token		*/
	ARROW		= 55;	/* ->				*/
	SIZEOF		= 56;	/* size of type		*/
	MALLOC		= 57;	/* allocate memory	*/
	PRINTF		= 58;	/* write to output	*/
	
	OPEN		= 60;	/* open file       	*/
	CLOSE		= 61;   /* close file      	*/
	READ		= 62;  	/* read file       	*/
	WRITE		= 63;  	/* write file      	*/
}

void initTMCmd() {
	/* immediate addressing */
	CMD_ADDI = 10;
	CMD_SUBI = 11;
	CMD_MULI = 12;
	CMD_DIVI = 13;
	CMD_MODI = 14;
	CMD_CMPI = 15;

	/* register addressing */
	CMD_ADD = 20;
	CMD_SUB = 21;
	CMD_MUL = 22;
	CMD_DIV = 23;
	CMD_MOD = 24;
	CMD_CMP = 25;

	/* memory: load and store */
	CMD_LDW = 30;
	CMD_STW = 31;

	/* stack operations */
	CMD_POP = 40;
	CMD_PSH = 41;

	/* conditional branching */
	CMD_BEQ = 50;
	CMD_BGE = 51;
	CMD_BGT = 52;
	CMD_BLE = 53;
	CMD_BLT = 54;
	CMD_BNE = 55;

	/* unconditional branching */
	CMD_BR  = 56;
	CMD_BSR = 57;
	CMD_JSR = 58;
	CMD_RET = 59;

	/* i/o operations */
	CMD_FLO = 60;
	CMD_FLC = 61;
	CMD_RDC = 62;
	CMD_WRC = 63;

	/* end of code */
	CMD_TRAP = 9;

	/* meta data */
	CMD_GP  = 1;	/* global pointer */
	CMD_SP  = 2; 	/* string pointer */
	CMD_CS  = 3; 	/* code size */
	CMD_MAL = 4;	/* malloc */
	CMD_PRN = 5;  /* print given value as integer */
	CMD_PRC = 6;  /* print given value as char */
}

char *getCmdName(int cmd_id) {
	initTMCmd();
	if(cmd_id == CMD_ADDI) return "addi";
	if(cmd_id == CMD_SUBI) return "subi";
	if(cmd_id == CMD_MULI) return "muli";
	if(cmd_id == CMD_DIVI) return "divi";
	if(cmd_id == CMD_CMPI) return "cmpi";
	if(cmd_id == CMD_ADD ) return "add ";
	if(cmd_id == CMD_SUB ) return "sub ";
	if(cmd_id == CMD_MUL ) return "mul ";
	if(cmd_id == CMD_DIV ) return "div ";
	if(cmd_id == CMD_CMP ) return "cmp ";
	if(cmd_id == CMD_LDW ) return "ldw ";
	if(cmd_id == CMD_STW ) return "stw ";
	if(cmd_id == CMD_POP ) return "pop ";
	if(cmd_id == CMD_PSH ) return "psh ";
	if(cmd_id == CMD_BEQ ) return "beq ";
	if(cmd_id == CMD_BGE ) return "bge ";
	if(cmd_id == CMD_BGT ) return "bgt ";
	if(cmd_id == CMD_BLE ) return "ble ";
	if(cmd_id == CMD_BLT ) return "blt ";
	if(cmd_id == CMD_BNE ) return "bne ";
	if(cmd_id == CMD_BR  ) return "br  ";
	if(cmd_id == CMD_BSR ) return "bsr ";
	if(cmd_id == CMD_JSR ) return "jsr ";
	if(cmd_id == CMD_RET ) return "ret ";
	if(cmd_id == CMD_TRAP) return "trap";
	if(cmd_id == CMD_GP  ) return "gp  ";
	if(cmd_id == CMD_SP  ) return "sp  ";
	if(cmd_id == CMD_CS  ) return "cs  ";
	if(cmd_id == CMD_MAL ) return "mal ";
	if(cmd_id == CMD_PRN ) return "prn ";
	if(cmd_id == CMD_PRC ) return "prc ";
	if(cmd_id == CMD_FLO ) return "flo ";
	if(cmd_id == CMD_FLC ) return "flc ";
	if(cmd_id == CMD_WRC ) return "wrc ";
	if(cmd_id == CMD_RDC ) return "rdc ";
	return "unknown";
}

void main(){
	char *sfile;
	char *ofile;
	sfile = "../selftest.c";
	ofile = "../out_selftest";

	initScanner(sfile);
	if(fd == -1) { printError("can not open file"); }
	else {
		startParsing(sfile, ofile);
		closeScanner();
	}
}

/******************************************************************************************/
/**************************************** scanner *****************************************/
/******************************************************************************************/
void initScanner(char *file) {	
	fd = open(file, 0, 448);		/* 0 ... read only 448 ... S_IWUSR | S_IRUSR | S_IXUSR */
	ungetc = -1;
	lineNr = 1;
	colNr = 1;
	initTokenMapping();
	initToken();
}
void closeScanner() { close(fd); }

void initToken() {
	char *str; str = "";
	symbol = malloc(sizeof(struct token_t));
	symbol->valueStr = malloc(64*sizeof(char));
	symbol->id = INIT; 
	symbol->digitValue = -1; 
	strnCpy(symbol->valueStr, str, 0);
}

bool hasMoreTokens() {
	char c; c = getChar();
	while (c == '\n') {
		c = getChar();
		if (c == -1) { return 0; }
	}
	if (c == -1) { return 0; }
	ungetChar(c);
	return 1;
}

bool hasMoreChars() {
	char c; c = getChar();
	if (c == -1) { return 0; }
	ungetChar(c);
	return 1;
}

char getChar() {
	char c;
	char *buff;
	int r;

	buff = malloc(sizeof(char));
	colNr = colNr + 1;
	if(ungetc == -1) {
		r = read(fd, buff, 1);
		if(buff[0] == '\n') {
			lineNr = lineNr + 1;
			colNr = 0;
		}
		if(r == 0) { return -1; }
		return buff[0];
	}
	c = ungetc;
	ungetc = -1;
	return c;
}
void ungetChar(char c) { 
	ungetc = c;
	colNr = colNr - 1; 
}

int strCmp(char *s1, char *s2) {
	int i; i = 0;
	while(s1[i] == s2[i]) {
		if( (s1[i] == '\0') || (s1[i] == 0) ) { return 0; }
		i = i + 1;
	}
	if(s1[i] < s2[i]) { return -1; }
	return 1;
}

int strnCpy(char *s1, char *s2, int n) {
	int i; i = 0;
	while ((i < n) && (s2[i] != '\0') && (s2[i] != 0)) {
		s1[i] = s2[i];
		i = i+1;
	}
	s1[i] = 0;
	return 0;
}

bool isLetter(char c) {
	return (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')));
}

bool isDigit(char c) { return ((c >= '0') && (c <= '9')); }

int getNumber(char pnr) {
	int nr; 
	char actnr;
	nr = pnr - '0';
	actnr = getChar();
	while(isDigit(actnr)) {
		nr = nr * 10 + (actnr - '0');
		actnr = getChar();
	}
	ungetChar(actnr);
	return nr;
}

void getIdentifier(char *identifier, char c) {
	char nc;
	int i;
	nc = getChar();
	i = 1;
	identifier[0] = c;
	while((isLetter(nc) || isDigit(nc) || (nc == '_')) && hasMoreChars() && (i < 63)) {
		identifier[i] = nc;
		nc = getChar();
		i = i + 1;
	}
	identifier[i] = 0;
	ungetChar(nc);
}

/* fd ... filedescriptor */
void getNextToken() {
	char c;
	char nc;
	char nnc;
	char *token_ident;
	char *buff;
	char *str;
	int i;

	token_ident = malloc(64*sizeof(char));
	buff = malloc(1000*sizeof(char));
	c = getChar();

	symbol->id = INIT; symbol->digitValue = -1; str = ""; strnCpy(symbol->valueStr, str, 0);
	while(( (c == ' ') || (c == '\n') || (c == 10) || (c == '\r') || (c == '\t') || (c == 9) ) && hasMoreChars()) {
		c = getChar();
	}
	if(isLetter(c)) {
		getIdentifier(token_ident, c);
		str = "if";      if(strCmp(token_ident, str) == 0) { symbol->id = IF;      strnCpy(symbol->valueStr, token_ident, 2); }
		str = "int";     if(strCmp(token_ident, str) == 0) { symbol->id = INT;     strnCpy(symbol->valueStr, token_ident, 3); }
		str = "char";    if(strCmp(token_ident, str) == 0) { symbol->id = CHAR;    strnCpy(symbol->valueStr, token_ident, 4); }
		str = "void";    if(strCmp(token_ident, str) == 0) { symbol->id = VOID;    strnCpy(symbol->valueStr, token_ident, 4); }
		str = "bool";    if(strCmp(token_ident, str) == 0) { symbol->id = BOOL;    strnCpy(symbol->valueStr, token_ident, 4); }
		str = "else";    if(strCmp(token_ident, str) == 0) { symbol->id = ELSE;    strnCpy(symbol->valueStr, token_ident, 4); }
		str = "open";    if(strCmp(token_ident, str) == 0) { symbol->id = OPEN;    strnCpy(symbol->valueStr, token_ident, 4); }
		str = "read";    if(strCmp(token_ident, str) == 0) { symbol->id = READ;    strnCpy(symbol->valueStr, token_ident, 4); }
		str = "write";   if(strCmp(token_ident, str) == 0) { symbol->id = WRITE;   strnCpy(symbol->valueStr, token_ident, 5); }
		str = "close";   if(strCmp(token_ident, str) == 0) { symbol->id = CLOSE;   strnCpy(symbol->valueStr, token_ident, 5); }
		str = "while";   if(strCmp(token_ident, str) == 0) { symbol->id = WHILE;   strnCpy(symbol->valueStr, token_ident, 5); }
		str = "return";  if(strCmp(token_ident, str) == 0) { symbol->id = RETURN;  strnCpy(symbol->valueStr, token_ident, 6); }
		str = "sizeof";  if(strCmp(token_ident, str) == 0) { symbol->id = SIZEOF;  strnCpy(symbol->valueStr, token_ident, 6); }
		str = "malloc";  if(strCmp(token_ident, str) == 0) { symbol->id = MALLOC;  strnCpy(symbol->valueStr, token_ident, 6); }
		str = "struct";  if(strCmp(token_ident, str) == 0) { symbol->id = STRUCT;  strnCpy(symbol->valueStr, token_ident, 6); }
		str = "printf";  if(strCmp(token_ident, str) == 0) { symbol->id = PRINTF;  strnCpy(symbol->valueStr, token_ident, 6); }
		str = "typedef"; if(strCmp(token_ident, str) == 0) { symbol->id = TYPEDEF; strnCpy(symbol->valueStr, token_ident, 7); }
		if(symbol->id == INIT) { symbol->id = IDENT; strnCpy(symbol->valueStr, token_ident, 64); }
	}
	else {
		if(isDigit(c)) { symbol->id = NUMBER; symbol->digitValue = getNumber(c); } 
		else {	
			if(c == '[') { symbol->id = LSQBR; }
			if(c == ']') { symbol->id = RSQBR; }
			if(c == '(') { symbol->id = LPAR; }
			if(c == ')') { symbol->id = RPAR; }
			if(c == '{') { symbol->id = LCUBR; }
			if(c == '}') { symbol->id = RCUBR; }
			if(c == ';') { symbol->id = SEMCOL; }
			if(c == ',') { symbol->id = COMMA; }
			if(c == '.') { symbol->id = DOT; }
			if(c == '"') {
				i = 0;
				nc = getChar();
				while( (nc != '"') && hasMoreTokens()) { 
					buff[i] = nc;
					nc = getChar();
					i = i + 1;
				}
				symbol->id = STRING; strnCpy(symbol->valueStr, buff, 1000);
				symbol->digitValue = i;
			}
			if(c == '\'') {
				buff[0] = getChar();
				if(buff[0] == '\\') {
					buff[1] = getChar();
					buff[2] = '\0';
				} 
				else {buff[1] = '\0';}
				nc = getChar();
				if(nc == '\'') { symbol->id = CHARACTER; strnCpy(symbol->valueStr, buff, 1); }
				else { 
					symbol->id = QUOTE; 
					ungetChar(nc); 
				}
			}
			if(c == '=') {
				nc = getChar();
				if(nc == '=') {symbol->id = EQ;}
				else { 
					symbol->id = EQSIGN; 
					ungetChar(nc);
				}
			}
			if(c == '+') { symbol->id = PLUS; }
			if(c == '-') { 
				nc = getChar();
				if(nc == '>') {symbol->id = ARROW;}
				else { 
					symbol->id = MINUS; 
					ungetChar(nc);
				}
			}
			if(c == '*') { symbol->id = TIMES; }
			if(c == '/') {
				nc = getChar();
				if(nc == '*') {
					nc = getChar();
					nnc = getChar();
					while(( (nc != '*') || (nnc != '/') ) && hasMoreChars()) {
						nc = nnc;
						nnc = getChar();
					}
					symbol->id = COMMENT;
				}
				else {
					if(nc == '/') {
						nc = getChar();
						while( (nc != '\n') && hasMoreChars() ) { nc = getChar(); }
						symbol->id = COMMENT;
					}
					else {
						symbol->id = DIV;
						ungetChar(nc);
					}
				}
			}
			if(c == '!') {
				nc = getChar();
				if(nc == '=') { symbol->id = NEQ; }
				else {
					symbol->id = ERROR; 
					symbol->valueStr[0] = c; symbol->valueStr[1] = 0;
					ungetChar(nc);
				}
			}
			if(c == '<') {
				nc = getChar();
				if(nc == '=') { symbol->id = LET; }
				else { 
					symbol->id = LT;
					ungetChar(nc);
				}
			}
			if(c == '>') {
				nc = getChar();
				if(nc == '=') { symbol->id = GET;}
				else { 
					symbol->id = GT;
					ungetChar(nc);
				}
			}
			if(c == '&') {
				nc = getChar();
				if(nc == '&') { symbol->id = AND; }
				else {
					symbol->id = ERROR;
					symbol->valueStr[0] = c; symbol->valueStr[1] = 0;
					ungetChar(nc);
				}
			}
			if(c == '|') {
				nc = getChar();
				if(nc == '|') { symbol->id = OR; }
				else {
					symbol->id = ERROR;
					symbol->valueStr[0] = c; symbol->valueStr[1] = 0;
					ungetChar(nc);
				}
			}
			if(c == '#') {
				nc = getChar();
				str = "include ";
				strnCpy(buff, str, 8);
				i = 0;
				while( (i < 8) && (nc == buff[i]) ) {
					nc = getChar();
					i = i + 1;		
				}
				if(i == 8) {
					if(nc == '"') {
						nc = getChar();
						i = 0;
						buff[i] = nc;
						while( (buff[i] != '\"') && (i < 1000) ) {
							i = i + 1;					
							buff[i] = getChar();					
						}
						buff[i] = 0;
						symbol->id = INCLUDE;
						strnCpy(symbol->valueStr, buff, 1000);
					}
				}
				else { symbol->id = ERROR; strnCpy(symbol->valueStr, buff, 10); }
			}
			if(symbol->id == INIT) { symbol->id = ERROR; symbol->valueStr[0] = c; symbol->valueStr[1] = 0; }
		}
	}
	if(symbol->id == COMMENT) { getNextToken(); }
	symbol->lineNr = lineNr;
	symbol->colNr = colNr;
}
/******************************************************************************************/
/************************************** symboltable ***************************************/
/******************************************************************************************/

struct object_t *lookUp(struct object_t *head, string_t name) {
	struct object_t *ptr;
	struct object_t *ptrParams;
	if(head->name != 0) {
		ptr = head;
		while(ptr != 0) {
			if(strCmp(ptr->name, name) == 0) {
				return ptr;
			}
			ptr = ptr->next;
		}
	}
	return 0;
}

struct object_t *findProcedureObject(struct object_t *head, string_t name) {
	struct object_t *ptr;
	if(head->name != 0) {
		ptr = head;
		while(ptr != 0) {
			if((ptr->class == OBJECT_CLASS_PROC) && (strCmp(ptr->name, name) == 0)) {
				return ptr;
			}
			ptr = ptr->next;
		}
	}
	return 0;
}

void insertName(struct object_t *ptr, string_t name) {
	ptr->name = malloc(64 * sizeof(char));
	strnCpy(ptr->name, name, 64);
}
	
int insert(struct object_t *head, struct object_t *obj) {
	struct object_t *ptr;
	char *str; str = "--first entry--";
	obj->next = 0;
	if(head->name == 0) {
		head->name = malloc(64 * sizeof(char));
		strnCpy(head->name, str, 64);
		head->class = 0;
		head->offset = 0;
		head->scope = 0;
		head->type = 0;
		head->next = obj;
	} else {
		if(lookUp(head, obj->name) != 0)	{
			errorCounter = errorCounter + 1;
			printf("    ");
			printString(srcfile); printf(":");
			printf(symbol->lineNr);printf(":");
			printf(symbol->colNr);printf(": ERROR: multible declaration of ");
			printf(obj->name); printf("\n");
			return 0;
		}
		ptr = head;
		while(ptr->next != 0) {
			ptr = ptr->next;
		}
		ptr->next = obj;
	}
	return 1;
}

struct object_t *delete(struct object_t *head, string_t name) {
   	struct object_t *ptr;
	struct object_t *ptr1;
	if(head != 0) {
		if(strCmp(head->name, name) == 0) {
			ptr = head->next;
			head = ptr;
		} else {
			ptr = head;
			while(ptr->next != 0) {
				ptr1 = ptr->next;
				if(strCmp(ptr1->name, name) == 0) {
					ptr->next = ptr1->next;
				}
				ptr = ptr1;
			}
		}
	}
	return head;
}

struct type_t *newType(int form) {
	struct type_t *type;
	type = malloc (sizeof (struct type_t));
	type->form = form;
	return type;
}

void initSymbolTable() {
	globList = malloc(sizeof(struct object_t));

	GLOBAL_SCOPE = 1;
	LOCAL_SCOPE = 2;

	TYPE_FORM_INT = 1;
	TYPE_FORM_CHAR = 2;
	TYPE_FORM_VOID = 3;
	TYPE_FORM_BOOL = 4;
	TYPE_FORM_ARRAY = 5;
	TYPE_FORM_RECORD = 6;
	TYPE_FORM_UNKNOWN = 7;

	OBJECT_CLASS_VAR = 1;
	OBJECT_CLASS_TYPE = 2;
	OBJECT_CLASS_FIELD = 3;
	OBJECT_CLASS_PROC = 4;

	nrOfGVar = 0;

	globOffset = 0;
	locOffset = 0;
	paramOffset = 0;
}
/******************************************************************************************/
/************************************** parser ********************************************/
/******************************************************************************************/

/*************************************************************
 * START PARSER
 ************************************************************/
bool startParsing(char *sfile, char *ofile){
	int i;
	srcfile = sfile;
	outfile = ofile;

	nrOfGVar = 0;
	nrOfStrs = 0;
	errorCounter = 0;
	PC = 1;	

	initTMCmd();
	cg_initRegs();
	initItemModes();
	initOperators();	
	initSymbolTable();
	initOutputFile();
	
	printf("\nstart parsing "); printString(srcfile); printf("...\n");
	if ( hasMoreTokens() ) {
		getNextToken();
		if(symbol->id == ERROR) {
			printError("file does not exist.");
			return 0;
		}
		i = programm();
	}
	
	if(errorCounter == 0) { finalizeOutputFile(); printf("... "); printString(outfile); printf(" successful generated.\n"); }
	else { printf("ERROR: "); printf(errorCounter); printf("\n"); }
	printf("\n -- DONE. --\n\n");
	return i;
}
/******************************************************************************************/

bool programm() {
	int i;
	int j;
	int k;
	i = 1; j = 1; k = 1;
	while(i != 0) {
		i = include();
		if(i == 1) { getNextToken(); }
	}
	while(j != 0) {
		j = structDec();
		if(j == 1) { getNextToken(); }
	}
	while(k != 0) {
		k = globalDec();
		if(k == 1) { getNextToken(); }
	}
	return i;
}

/*************************************************************
 * CODE GENERATION METHODS
 ************************************************************/
void initOutputFile() {
	int d; int size; size = 8000;
	d = malloc(sizeof(int) * size);
	d = malloc(sizeof(int) * size);
	d = malloc(sizeof(int) * size);
	out_cmd_op = malloc(sizeof(int) * size);

	d = malloc(sizeof(int) * size);
	d = malloc(sizeof(int) * size);
	d = malloc(sizeof(int) * size);
	out_cmd_a  = malloc(sizeof(int) * size);

	d = malloc(sizeof(int) * size);
	d = malloc(sizeof(int) * size);
	d = malloc(sizeof(int) * size);
	out_cmd_b  = malloc(sizeof(int) * size);

	d = malloc(sizeof(int) * size);
	d = malloc(sizeof(int) * size);
	d = malloc(sizeof(int) * size);
	out_cmd_c  = malloc(sizeof(int) * size);
}

void finalizeOutputFile() {
	int wb; int i; int out_fp_bin;
	int *tempBuff;
	tempBuff = malloc(32);
	out_fp_bin = open(outfile, 65, 448); /* 65  ... O_CREAT (64)  | O_WRONLY (1) 448 ... S_IWUSR | S_IRUSR | S_IXUSR  --> Ubuntu */
										 /* 513 ... O_CREAT (512) | O_WRONLY (1) 448 ... S_IWUSR | S_IRUSR | S_IXUSR  --> Mac */
	if(out_fp_bin < 0) {
		printError("can not open/create output file.");
		return;
	}
	cg_put(CMD_TRAP,0,0,0);
	tempBuff[0] = cg_encode(CMD_CS,	0, 0, PC);
	write(out_fp_bin, tempBuff, 4); 

	tempBuff[0] = cg_encode(CMD_GP,	0, 0, nrOfGVar*4);
	write(out_fp_bin, tempBuff, 4);

	if(nrOfStrs > 0) { nrOfStrs = nrOfStrs - 1; }
	tempBuff[0] = cg_encode(CMD_SP,	0, 0, nrOfStrs);
	write(out_fp_bin, tempBuff, 4); 

	if(mainPos != -1) {
		tempBuff[0] = cg_encode(CMD_BR,	0, 0, mainPos);	
		write(out_fp_bin, tempBuff, 4); 
	} else { printError("missing main method"); }
	i = 1;
	while(i < PC) {
		tempBuff[0] = cg_encode(out_cmd_op[i], out_cmd_a[i], out_cmd_b[i], out_cmd_c[i]);
		wb = write(out_fp_bin, tempBuff, 4); 
    	if ( wb != 4 ) { 
			errorCounter = errorCounter + 1;
			printf("    ");
			printString(srcfile); printf(":");
			printf(symbol->lineNr);printf(":");
			printf(symbol->colNr);printf(": ERROR: could only write ");
			printf(wb);printf(" byte.\n"); 
		}
		i = i + 1;
	}
	close(out_fp_bin);
}

int cg_encode(int op, int a, int b, int c) {
	int z; z = 4;
	if (c < 0) { c = c + (16384*z); } 			// 16384 = 1/4 * 65536 = 0x10000 = 2^16
	return (((((op * 32) + a) * 32) + b) * (16384*z)) + c;
}

void cg_put(int op, int a, int b, int c) {
	out_cmd_op[PC] = op;
	out_cmd_a [PC] = a;
	out_cmd_b [PC] = b;
	out_cmd_c [PC] = c;
	PC = PC + 1;
}

void copyItem(struct item_t *copy, struct item_t *orig) {
	copy->mode = orig->mode;
	if(copy->type == 0) { copy->type = malloc(sizeof(struct type_t)); }
	copy->type->form = orig->type->form;
	copy->type->size = orig->type->size;
	copy->type->fields = orig->type->fields;
	copy->type->base = orig->type->base;
	copy->reg = orig->reg;
	copy->offset = orig->offset;
	copy->value = orig->value;
	copy->op = orig->op;
	copy->fls = orig->fls;
	copy->tru = orig->tru;
}

void initItemModes() {
	ITEM_MODE_NONE  = 0;
	ITEM_MODE_CONST = 1;
	ITEM_MODE_VAR   = 2;
	ITEM_MODE_REF   = 3;
	ITEM_MODE_REG   = 4;
	ITEM_MODE_COND  = 5;
}

void initOperators() {
	OP_NONE = 0;
	OP_ADD  = 1;
	OP_SUB  = 2;
	OP_MUL  = 3;
	OP_DIV  = 4;
	OP_AND  = 5;
	OP_OR   = 6;
	OP_EQ   = 7;
	OP_NEQ  = 8;
	OP_GT   = 9;
	OP_GET  =10;
	OP_LT   =11;
	OP_LET  =12;	
}

/***  REGISTER METHODS ***/
void cg_initRegs() {
	int i; i = 0;
	nrOfRegs = 32;
	mainPos = -1;
	RR   = 25;
	FPTR = 26;
	GPTR = 28;
	STRPTR=27;
	SPTR = 29;
	HPTR = 30;
	LINK = 31;
	regs = malloc( nrOfRegs*sizeof(int) );
	while (i < nrOfRegs) { regs[i] = 0; i = i + 1; }
}
int cg_requestReg() {
	int i; i = 1;
	/* register for calculation: 1-27 */
	while (i < 25) { 
		if (regs[i] == 0) { regs[i] = 1; return i; }
		i = i + 1;
	}
	return -1;		
}
void cg_releaseReg(int r) { regs[r] = 0; }

void cg_const2Reg(struct item_t *item) {
	item->mode = ITEM_MODE_REG;
	item->reg = cg_requestReg();
	cg_put(CMD_ADDI, item->reg, 0, item->value);
	item->value = 0;
	item->offset = 0;
}

void cg_var2Reg(struct item_t *item) {
	int newReg;
	item->mode = ITEM_MODE_REG;
	newReg = cg_requestReg();
	cg_put(CMD_LDW, newReg, item->reg, item->offset);
	item->reg = newReg;
	item->offset = 0;
}

void cg_ref2Reg(struct item_t *item) {
	item->mode = ITEM_MODE_REG;
	cg_put(CMD_LDW, item->reg, item->reg, item->offset);
	item->offset = 0;
}

void cg_load(struct item_t *item) {
		   if(item->mode == ITEM_MODE_CONST) { cg_const2Reg(item); }
	else { if(item->mode == ITEM_MODE_VAR)   { cg_var2Reg(item);   }
	else { if(item->mode == ITEM_MODE_REF)   { cg_ref2Reg(item);   } } }
}

void cg_simpleExpOR(struct item_t* item) { 
	if (item->type->form == TYPE_FORM_BOOL) {
		cg_loadBool(item);
		cg_put(cg_branch(item->op), item->reg, 0, item->tru);
		cg_releaseReg(item->reg);
		item->tru = PC - 1;
		cg_fixLink(item->fls);
		item->fls = 0;
	} else { printError("boolean expression expected"); }
}

void cg_termOpAND(struct item_t* item) { 
	if (item->type->form == TYPE_FORM_BOOL) {
		cg_loadBool(item);
		cg_put(cg_branch(cg_negateOperator(item->op)), item->reg, 0, item->fls);
		cg_releaseReg(item->reg);
		item->fls = PC - 1;
		cg_fixLink(item->tru);
		item->tru = 0;
	} else { printError("boolean expression expected"); }
}

void cg_simpleExpBinOp(struct item_t *leftItem, struct item_t *rightItem, int op) {
	if(op == OP_OR) {
		if ((leftItem->type->form == TYPE_FORM_BOOL) && (rightItem->type->form == TYPE_FORM_BOOL)) { 
			cg_loadBool(rightItem);
			leftItem->reg = rightItem->reg; 
			leftItem->fls = rightItem->fls;
			leftItem->tru = cg_concatenate(rightItem->tru, leftItem->tru);
			leftItem->op = rightItem->op;
		} else { printError("boolean expressions expected"); }
	}
	else {
		if(((leftItem->type->form == TYPE_FORM_INT) || (leftItem->type->form == TYPE_FORM_CHAR)) && 
				((rightItem->type->form == TYPE_FORM_INT) || (rightItem->type->form == TYPE_FORM_CHAR))) {
			if(rightItem->mode == ITEM_MODE_CONST) {
				if(leftItem->mode == ITEM_MODE_CONST) {
					if(op == OP_ADD) { leftItem->value = leftItem->value + rightItem->value; } 
					else {
						 if(op == OP_SUB) { leftItem->value = leftItem->value - rightItem->value; } 
					}
				} else {
					cg_load(leftItem);
					if(op == OP_ADD) { cg_put(CMD_ADDI, leftItem->reg, leftItem->reg, rightItem->value); } 
					else {
						if(op == OP_SUB) { cg_put(CMD_SUBI, leftItem->reg, leftItem->reg, rightItem->value); } 
					}
				}
			} else {
				cg_load(leftItem);
				cg_load(rightItem);
				if(op == OP_ADD) { cg_put(CMD_ADD, leftItem->reg, leftItem->reg, rightItem->reg); } 
				else {
					if(op == OP_SUB) { cg_put(CMD_SUB, leftItem->reg, leftItem->reg, rightItem->reg); } 
				}
				cg_releaseReg(rightItem->reg);
			}
		} else { printError("Type mismatch (int expected)."); }
	}
}

void cg_termOperator(struct item_t *leftItem, struct item_t *rightItem, int op) {
	if(op == OP_AND) {
		if ((leftItem->type->form == TYPE_FORM_BOOL) && (rightItem->type->form == TYPE_FORM_BOOL)) {  
			cg_loadBool(rightItem);
			leftItem->reg = rightItem->reg;
			leftItem->fls = cg_concatenate(rightItem->fls, leftItem->fls);
			leftItem->tru = rightItem->tru;
			leftItem->op = rightItem->op;
		} else { printError("boolean expressions expected"); }
	}	
	else {
		if(rightItem->mode == ITEM_MODE_CONST) {
			if(leftItem->mode == ITEM_MODE_CONST) {
				if(op == OP_MUL) { leftItem->value = leftItem->value * rightItem->value; } 
				else {	
					if(op == OP_DIV) { leftItem->value = leftItem->value / rightItem->value; } 
				}
			} else {
				cg_load(leftItem);
				if(op == OP_MUL) { cg_put(CMD_MULI, leftItem->reg, leftItem->reg, rightItem->value); } 
				else {
					if(op == OP_DIV) { cg_put(CMD_DIVI, leftItem->reg, leftItem->reg, rightItem->value); } 
				}
			}
		} else {
			cg_load(leftItem);
			cg_load(rightItem);
			if(op == OP_MUL) { cg_put(CMD_MUL, leftItem->reg, leftItem->reg, rightItem->reg); } 
			else {
				if(op == OP_DIV) { cg_put(CMD_DIV, leftItem->reg, leftItem->reg, rightItem->reg); } 
			}
			cg_releaseReg(rightItem->reg);
		}
	}
}

void cg_expressionOperator(struct item_t *leftItem, struct item_t *rightItem, int op) {
	cg_load(leftItem);
	if ((rightItem->mode != ITEM_MODE_CONST) || (rightItem->value != 0)) {
		cg_load(rightItem);
		cg_put(CMD_CMP, leftItem->reg, leftItem->reg, rightItem->reg); 
		cg_releaseReg(rightItem->reg);
	}
	leftItem->mode = ITEM_MODE_COND; 
	leftItem->type->form = TYPE_FORM_BOOL; 
	leftItem->op = op;
	leftItem->fls = 0;
	leftItem->tru = 0;
}

void cg_field(struct item_t *item, struct object_t *object) {
	cg_load(item);
	item->mode = ITEM_MODE_REF;
	item->type->form = object->type->form;
	item->type->size = object->type->size;
	item->type->fields = object->type->fields;
	item->type->base = object->type->base;
	item->offset = object->offset;
}

void cg_index(struct item_t *item, struct item_t *indexItem) {
	if(indexItem->mode == ITEM_MODE_CONST) {
		cg_load(item);
		item->mode = ITEM_MODE_REF;
		item->offset = indexItem->value * (-4);
	} else {
		cg_load(indexItem);
		cg_put(CMD_MULI, indexItem->reg, indexItem->reg, -1);
		cg_load(item);
		item->mode = ITEM_MODE_REF;
		cg_put(CMD_ADD, item->reg, item->reg, indexItem->reg);	
		cg_releaseReg(indexItem->reg);
	}
	item->type = item->type->base;
}

void cg_assignment(struct item_t *leftItem, struct item_t *rightItem) {
	struct type_t *leftBase;
	struct type_t *rightBase;
	leftBase = leftItem->type->base;
	rightBase = rightItem->type->base;
	if(leftItem->type->form == TYPE_FORM_ARRAY) {
		if( (rightItem->type->form != TYPE_FORM_ARRAY) 
			&& (leftBase->form != rightItem->type->form) && (rightItem->type->form != TYPE_FORM_VOID)) { 
			printError("Type mismatch in assignment"); 
		}
	} else {
		if(rightItem->type->form == TYPE_FORM_ARRAY) {	
			if((rightBase->form != leftItem->type->form) && (rightItem->type->form != TYPE_FORM_VOID)) { 
				printError("Type mismatch in assignment"); 
			}
		} else {
			if( (leftItem->type->form != rightItem->type->form) && (rightItem->type->form != TYPE_FORM_VOID) ) {
				if ( (leftItem->type->form != TYPE_FORM_INT) && (rightItem->type->form != TYPE_FORM_CHAR) ) {
					if ( (leftItem->type->form != TYPE_FORM_CHAR) && (rightItem->type->form != TYPE_FORM_INT) ) {
						printError("Type mismatch in assignment"); 
					}
				}
			}
		}
	}
	if (rightItem->type->form == TYPE_FORM_BOOL) { cg_unloadBool(rightItem); }
	cg_load(rightItem);
	cg_put(CMD_STW, rightItem->reg, leftItem->reg, leftItem->offset);
	if(leftItem->mode == ITEM_MODE_REF) { cg_releaseReg(leftItem->reg); }
	cg_releaseReg(rightItem->reg);
}

void cg_allocate(struct item_t *item) {
	cg_load(item);
	item->type->form = TYPE_FORM_VOID;
	item->type->fields = 0;
	cg_put(CMD_MAL,item->reg,HPTR,item->reg);
}

void cg_cJump(struct item_t *item) {
	cg_put(cg_branch(cg_negateOperator(item->op)), item->reg, 0, item->fls);
	cg_releaseReg(item->reg);
	item->fls = PC - 1;
}

int cg_fJump() {
	cg_put(CMD_BR, 0, 0, 0);
	return PC - 1;
}

void cg_bJump(int backAddress) { cg_put(CMD_BR, 0, 0, backAddress - PC); }

void cg_fixUp(int branchAddress) { cg_encodeC(branchAddress, PC - branchAddress); }

void cg_fixLink(int branchAddress) {
	int nextBranchAddress;
	while (branchAddress != 0) {
		nextBranchAddress = cg_decodeC(branchAddress);
		cg_fixUp(branchAddress);
		branchAddress = nextBranchAddress;
	}
}

int cg_concatenate(int right, int left) {
	int nextBranchAddress;
	int newTru;

	if(right == 0) { return left; }
	if(left == 0) { return right; }

	newTru = right;
	while (right != 0) {
		nextBranchAddress = cg_decodeC(right);
		right = nextBranchAddress; 
	}
	cg_encodeC(right, left);
	return newTru;
}

int cg_negateOperator(int op) {
	if(op == OP_LT)	 { return OP_GET; }
	if(op == OP_GT)	 { return OP_LET; }
	if(op == OP_LET) { return OP_GT;  }
	if(op == OP_GET) { return OP_LT;  }
	if(op == OP_EQ)	 { return OP_NEQ; }
	if(op == OP_NEQ) { return OP_EQ;  }
	return -1;
}

int cg_branch(int op) {
	if(op == OP_LT)	 { return CMD_BLT; }
	if(op == OP_GT)	 { return CMD_BGT; }
	if(op == OP_LET) { return CMD_BLE; }
	if(op == OP_GET) { return CMD_BGE; }
	if(op == OP_EQ)	 { return CMD_BEQ; }
	if(op == OP_NEQ) { return CMD_BNE; }
	return -1;
}

void cg_encodeC(int branchAddress, int newC) {
//	out_cmd_buff[branchAddress]->c = newC;
	out_cmd_c[branchAddress] = newC;
}

int cg_decodeC(int branchAddress) {
//	return out_cmd_buff[branchAddress]->c;
	return out_cmd_c[branchAddress];
}

void cg_loadBool(struct item_t *item) {
	if(item->mode != ITEM_MODE_COND) {
		cg_load(item);
		item->mode = ITEM_MODE_COND;
		item->op = OP_NEQ;
		item->fls = 0;
		item->tru = 0;
	}
}

void cg_unloadBool(struct item_t *item) {
	if(item->mode == ITEM_MODE_COND) {
		cg_cJump(item);
		cg_fixLink(item->tru);
		item->mode = ITEM_MODE_REG;
		cg_put(CMD_ADDI, item->reg, 0, 1);
		cg_put(CMD_BR, 0, 0, 2);
		cg_fixLink(item->fls);
		cg_put(CMD_ADDI, item->reg, 0, 0);
	}
}


/*************************************************************
 * HELP METHODS
 ************************************************************/
void printString(char *str) {
	int i; i = 0; 
	while( (str[i] != 0) && (str[i] != '\0') ) { 
		printf(str[i]); 
		i = i + 1; 
	}
}
void printError(char *msg) {
	errorCounter = errorCounter + 1;
	printf("    ");
	printString(srcfile); printf(":");
	printf(symbol->lineNr);printf(":");
	printf(symbol->colNr);printf(": ERROR: ");
	printString(msg);printf(" (found: ");
	printString(symbol->valueStr);
	printf("/");printf(symbol->id);printf(")\n");

}

/*************************************************************
 * PARSING METHODS
 ************************************************************/

/* ["-"] digit {digit} . */
bool number() {
	if(symbol->id == MINUS)  { 
		if(hasMoreTokens() == 0) { return 0; }	
		getNextToken(); 
	}
	if(symbol->id == NUMBER) { return 1; }
	return 0;
}

/* "==" | "!=" | "<=" | ">=" | "<" | ">" . */
bool compOp() {
	if( (symbol->id == EQ) || (symbol->id == NEQ) || (symbol->id == LT) || (symbol->id == GT) || (symbol->id == LET) || (symbol->id == GET)) {
		return 1;
	}
	return 0;
}

/* "+" | "-" | "*" | "/" | "&&" | "||" . */
bool op() {
	if((symbol->id == PLUS) || (symbol->id == MINUS) || (symbol->id == DIV) || (symbol->id == TIMES) 
		|| compOp() || (symbol->id == AND) || (symbol->id == OR)) {
		return 1;
	}
	return 0;
}

/* letter {letter | digit} . */
bool identifier() {
	if(symbol->id == IDENT) { return 1; }
	return 0;
}

/* "int" | "char" | "void" | "bool" | identifier. */ 
int typeSpec(struct item_t *item, struct object_t *head) {
	struct object_t *ptr;

	item->mode = ITEM_MODE_CONST;
	item->type->size = 4;
	item->value = 4;

	if(symbol->id == INT ) { item->type->form = TYPE_FORM_INT;  return TYPE_FORM_INT;  }
	if(symbol->id == CHAR) { item->type->form = TYPE_FORM_CHAR; return TYPE_FORM_CHAR; } /* return value for symbol table */
	if(symbol->id == VOID) { item->type->form = TYPE_FORM_VOID; return TYPE_FORM_VOID; }
	if(symbol->id == BOOL) { item->type->form = TYPE_FORM_BOOL; return TYPE_FORM_BOOL; }
	if(identifier()) {
		ptr = lookUp(globList, symbol->valueStr);
		if((ptr != 0) && (ptr->class == OBJECT_CLASS_TYPE)) {
			item->type->form = ptr->type->form;
			item->type->size = ptr->type->size;
			item->value = ptr->type->size;
			return TYPE_FORM_RECORD;
		}
		ptr = lookUp(head, symbol->valueStr);
		if((ptr != 0) && (ptr->class == OBJECT_CLASS_TYPE)) { 
			item->type->form = ptr->type->form;
			item->type->size = ptr->type->size;
			item->value = ptr->type->size;
			return TYPE_FORM_RECORD;
		} else {	// creates error between variable declaration sequence and statement sequence
			if(typeWA != 0) {			
				printf("    ");
				printString(srcfile); printf(":");
				printf(symbol->lineNr);printf(":");
				printf(symbol->colNr);printf(": ERROR: unknown type.");
				printf(" (found: "); printString(symbol->valueStr);
				printf("/");printf(symbol->id);printf(")\n");
			} else { typeWA = 1; }
		}
	}
	return 0;
}

/* include . */
bool include() {
	if(symbol->id == INCLUDE) { return 1; }
	return 0;
}

bool storeString(struct item_t *item) {
	int i; int reg; int r; int c;
	struct type_t *array;
	struct type_t *arrayBase;

	array = malloc(sizeof(struct type_t));
	array->base = malloc(sizeof(struct type_t));
	arrayBase = array->base;

	array->form = TYPE_FORM_ARRAY;
	arrayBase->form = TYPE_FORM_CHAR;
	i = 0;

	r = cg_requestReg();
	cg_put( CMD_ADDI, r, STRPTR, (nrOfStrs * (-1)) );

	item->mode = ITEM_MODE_REG;
	item->offset = 0;
	item->reg = r;
	if(item->type == 0) { item->type = malloc(sizeof(struct type_t)); }
	//item->type->form = TYPE_FORM_VOID;
	//item->type->fields = 0;
	item->type = array;

	reg = cg_requestReg();
	while(i < symbol->digitValue) {
		c = symbol->valueStr[i];
		cg_put( CMD_ADDI, reg, 0, c);
		cg_put( CMD_STW, reg, STRPTR, ( (nrOfStrs + i) * 4 * (-1)) );
		i = i + 1;
	}
	cg_put( CMD_STW, 0, STRPTR, ( (nrOfStrs + i) * 4 * (-1)) );
	nrOfStrs = nrOfStrs + i + 1;
	cg_releaseReg(reg);

	return 1;
}

/* sizeof "(" (typeSpec | identifier) ")" . */
bool sizeOf(struct item_t *item, struct object_t *head) {
	if(symbol->id == SIZEOF) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol->id == LPAR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(symbol->id == STRUCT) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
			}
			if(typeSpec(item, head) != 0) {
				item->type->form = TYPE_FORM_INT;
				item->type->fields = 0;

				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				if(symbol->id == RPAR) { 
					return 1; 
				} else { printError("')' missing."); }
			} else { printError("typeSpec or identifier expected."); }
		} else { printError("'(' missing."); }
	}
	return 0;
}

bool selector(struct item_t *item){
	struct object_t *object;
	struct item_t * indexItem;
	while( (symbol->id == ARROW) || (symbol->id == LSQBR) ) {
		if(symbol->id == ARROW) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();

			if(identifier()) {
				object = lookUp(item->type->fields, symbol->valueStr);

				if(object != 0) { cg_field(item,object); }
				else { printError("unknown field"); }

				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
			} else {
				printError("identifier expected");
			}
		} else { 
			if(symbol->id == LSQBR) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();

				indexItem = malloc(sizeof(struct item_t));
				indexItem->type = malloc(sizeof(struct type_t));
				indexItem->offset = 0;
				indexItem->value = 0;
				expression(indexItem);
				cg_index(item, indexItem);

				if(symbol->id == RSQBR) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
				} else { printError("']' missing"); }
			}
		}
	}
	return 1;
}

/* identifier | number | procCall | "(" arithExp ")" . */
bool factor(struct item_t *item) {
	int result;
	struct item_t *leftItem;
	struct item_t *rightItem;
	struct object_t *object;
	leftItem = malloc(sizeof(struct item_t));
	leftItem->type = malloc(sizeof(struct type_t));
	rightItem = malloc(sizeof(struct item_t));
	rightItem->type = malloc(sizeof(struct type_t));
	object = malloc(sizeof(struct object_t));
	
	leftItem->offset = 0;
	leftItem->value = 0;
	rightItem->offset = 0;
	rightItem->value = 0;

	if(number() || (symbol->id == CHARACTER) || (symbol->id == STRING)) {
		if(number()) {
			item->mode = ITEM_MODE_CONST;
			if(item->type == 0) { item->type = malloc(sizeof(struct type_t)); }
			item->type->form = TYPE_FORM_INT;
			item->value = symbol->digitValue;
		} else {
			if(symbol->id == STRING) { storeString(item); } 
			else { /* if symbol->id == CHARACTER */
				item->mode = ITEM_MODE_CONST;
				item->type->form = TYPE_FORM_CHAR;
				item->value = symbol->valueStr[0];
			}
		}
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		return 1;
	}
	if(symbol->id == LPAR) { 
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(expression(item)) {
			if(symbol->id == RPAR) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				return 1; 
			} else { printError("')' missing."); }
		} else { printError("expression expected."); }
		return 0;
	}
	if(symbol->id == TIMES) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(identifier()) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			return 1;
		}
		return 0;
	}
	if(sizeOf(item, globList)) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		return 1;
	} 
	if(symbol->id == MALLOC) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		result = expression(item);
		cg_allocate(item);
		return result;
	} 
	if(symbol->id == OPEN)  { fileOpen(item);  return 1; }
	if(symbol->id == READ)  { fileRead(item);  return 1; }
	if(symbol->id == WRITE) { fileWrite(item); return 1; }
	if(symbol->id == CLOSE) { fileClose(item); return 1; } 
	if(identifier()) {
		object = lookUp(locList, symbol->valueStr);
		if(object == 0) {
			if(procedureContext->params != 0) { object = lookUp(procedureContext->params, symbol->valueStr); }
			if(object == 0) {
				object = lookUp(globList, symbol->valueStr);
				if(object == 0) {
					printError("unknown identifier");
					return 0;
				}
			}
		}
		leftItem->mode = ITEM_MODE_VAR;
		leftItem->type->form = object->type->form;
		leftItem->type->size = object->type->size;
		leftItem->type->fields = object->type->fields;
		leftItem->type->base = object->type->base;
		if(object->scope == GLOBAL_SCOPE) { leftItem->reg = GPTR; } 
		else { leftItem->reg = FPTR; }
		leftItem->offset = object->offset;

		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();

		selector(leftItem);

		if(symbol->id == EQSIGN) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			result =  expression(rightItem);
			cg_assignment(leftItem, rightItem);
			copyItem(item, leftItem);
			return result;
		}
		if(op()) { copyItem(item,leftItem); return 1; }		
		if(symbol->id == LPAR) {
			procedureCall(leftItem, object->name);
			if(symbol->id == RPAR) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				copyItem(item, leftItem);
				return 1;
			}
		}

		if((symbol->id == SEMCOL) || (symbol->id == COMMA) || (symbol->id == RSQBR) || (symbol->id == ARROW) || (symbol->id == RPAR)) 
		{ copyItem(item, leftItem); return 1; }
	}
	return 0;
}

/* factor { ( "*" | "/" ) factor } . */
bool term(struct item_t *item) {
	struct item_t *leftItem;
	struct item_t *rightItem;
	int op; 

	op = OP_NONE;
	leftItem = 0;
	rightItem = 0;
	while( 0 < 1 ) {
		if(factor(item)) {
			if(leftItem == 0){
				leftItem = malloc(sizeof(struct item_t));
				leftItem->type = malloc(sizeof(struct type_t));
				leftItem->offset = 0;
				leftItem->value = 0;
				copyItem(leftItem, item);
			} 
			else { 
				if(rightItem == 0) { 
					rightItem = malloc(sizeof(struct item_t));
					rightItem->type = malloc(sizeof(struct type_t));
					rightItem->offset = 0;
					rightItem->value = 0;
					copyItem(rightItem, item);
				} 
			}
			if((leftItem != 0) && (rightItem != 0)) {
				if(op != OP_NONE) {
					cg_termOperator(leftItem, rightItem, op);
					rightItem = 0;
					op = OP_NONE;
				} else { printError("missing operator"); }
			}
		} else { return 0; }

		if((symbol->id == PLUS) || (symbol->id == MINUS) || (symbol->id == OR) || 
				compOp() || (symbol->id == RPAR) || (symbol->id == SEMCOL) || (symbol->id == COMMA) || 
				(symbol->id == RSQBR) || (symbol->id == RCUBR) || (symbol->id == ARROW)) {
			copyItem(item, leftItem);
			return 1;
		}
		if((symbol->id == TIMES) || (symbol->id == DIV) || (symbol->id == AND)) {
			if(symbol->id == TIMES) { op = OP_MUL; }
			if(symbol->id == DIV) { op = OP_DIV; }
			if(symbol->id == AND) { op = OP_AND; cg_termOpAND(leftItem); }
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
		} else {
			printError("'*', '/' or ';' expected.");
			return 0;
		}
	}
}

/* [ "-" ] term { ( "+" | "-" ) term } . */
bool arithExp(struct item_t *item) {
	struct item_t *leftItem;
	struct item_t *rightItem;
	int op; 
	int minusFlag;

	leftItem = 0;
	rightItem = 0;

	op = OP_NONE;
	minusFlag = 1;

	if(symbol->id == MINUS) { 
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		minusFlag = -1;
	}
	while( 0 < 1 ) {
		if(term(item)) {
			if(leftItem == 0){
				leftItem = malloc(sizeof(struct item_t));
				leftItem->type = malloc(sizeof(struct type_t));
				leftItem->value = 0;
				leftItem->offset = 0;
				copyItem(leftItem, item);
				leftItem->value = leftItem->value * minusFlag;
			} else {
				if(rightItem == 0) {
					rightItem = malloc(sizeof(struct item_t));
					rightItem->type = malloc(sizeof(struct type_t));
					rightItem->value = 0;
					rightItem->offset = 0;
					copyItem(rightItem, item);
				}
			} 
			if((leftItem != 0) && (rightItem != 0)) {
				if(op != OP_NONE) {
					cg_simpleExpBinOp(leftItem, rightItem, op);
					rightItem = 0;
					op = OP_NONE;
				} else {
					printError("missing operator");
				}
			}
		} else { return 0; }

		if(compOp() || (symbol->id == RPAR) || (symbol->id == SEMCOL) || (hasMoreTokens() == 0) || 
			(symbol->id == COMMA) || (symbol->id == RSQBR) || (symbol->id == RCUBR) || (symbol->id == ARROW)) {
			copyItem(item, leftItem);
			return 1;
		}
		if((symbol->id == PLUS) || (symbol->id == MINUS) || (symbol->id == OR)) {
			if(symbol->id == PLUS) { op = OP_ADD; }
			if(symbol->id == MINUS) { op = OP_SUB; }
			if(symbol->id == OR) { op = OP_OR; cg_simpleExpOR(leftItem); }
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			cg_load(leftItem);
		} else {
			printError("'+', '-' or ';' expected..");
			return 0;
		}
	}
}

/* arithExp { boolOp arithExp } . */
bool expression(struct item_t *item) {
	struct item_t *leftItem;
	struct item_t *rightItem;
	int op; 

	leftItem = 0;
	rightItem = 0;
	op = OP_NONE;
	while( 0 < 1 ) {
		if(arithExp(item)) {
			if(leftItem == 0){
				leftItem = malloc(sizeof(struct item_t));
				leftItem->type = malloc(sizeof(struct type_t));
				leftItem->value = 0;
				leftItem->offset = 0;
				copyItem(leftItem, item);
			} else {
				if(rightItem == 0) {
					rightItem = malloc(sizeof(struct item_t));
					rightItem->type = malloc(sizeof(struct type_t));
					rightItem->value = 0;
					rightItem->offset = 0;
					copyItem(rightItem, item);
				} 
			}
			if((leftItem != 0) && (rightItem != 0)) {
				if(op != OP_NONE) {
					cg_expressionOperator(leftItem, rightItem, op);
					rightItem = 0;
					op = OP_NONE;
				} else {
					printError("missing operator");
				}
			}
		} else { return 0; }
		if((symbol->id == RPAR) || (symbol->id == SEMCOL) || (hasMoreTokens() == 0) || 
			(symbol->id == COMMA) || (symbol->id == RSQBR) || (symbol->id == RCUBR) || (symbol->id == ARROW)) {
			copyItem(item, leftItem);
			return 1;
		}

		if(compOp()) {
			if(symbol->id == EQ) { op = OP_EQ; }
			if(symbol->id == NEQ) { op = OP_NEQ; }
			if(symbol->id == GT) { op = OP_GT; }
			if(symbol->id == GET) { op = OP_GET; }
			if(symbol->id == LT) { op = OP_LT; }
			if(symbol->id == LET) { op = OP_LET; }
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
		} else {
			printError("boolean operator  or ';' expected.");
			return 0;
		}
	}
}

bool printMethod(struct item_t *item) {
	int i; int c;
	int reg;
	struct type_t *itemBase;
	itemBase = item->type->base;
	if(symbol->id == PRINTF) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol->id == LPAR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(symbol->id == STRING) {
				i = 0;
				reg = cg_requestReg();
				while(symbol->valueStr[i] != 0) { 
					if((symbol->valueStr[i] == '\\') && (symbol->valueStr[i+1] == 'n')) {
						c = '\n';
						cg_put(CMD_ADDI, reg, 0, c);
						cg_put(CMD_PRC, reg, 0, 0);
						i = i + 1;
					} else {
						c = symbol->valueStr[i];
						cg_put(CMD_ADDI, reg, 0, c);
						cg_put(CMD_PRC, reg, 0, 0); /* end of self comp (type mismatch) */ 
					}
					i = i + 1;
				} 
				cg_releaseReg(reg);
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
			} else {
				if (expression(item)) {
					cg_load(item);
					if(item->type->form == TYPE_FORM_CHAR) { cg_put(CMD_PRC, item->reg, 0, 0); } 
					else { cg_put(CMD_PRN, item->reg, 0, 0); }
				} else { printError("expression expected."); }
			}
			if(symbol->id == RPAR) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				if(symbol->id == SEMCOL) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					return 1;
				} else { printError("';' missing."); }
			} else { printError("')' missing."); }
		} else { printError("'(' missing."); }
	}
	return 0;
}

bool fileOpen(struct item_t *item) {
	struct item_t *firstItem;
	struct item_t *secondItem;
	struct item_t *thirdItem;
	struct object_t *object;
	firstItem = malloc(sizeof(struct item_t));
	secondItem = malloc(sizeof(struct item_t));	
	thirdItem = malloc(sizeof(struct item_t));	
	object = malloc(sizeof(struct object_t));
	object->type = malloc(sizeof(struct type_t));

	firstItem->value = 0;
	firstItem->offset = 0;	
	secondItem->value = 0;
	secondItem->offset = 0;
	thirdItem->value = 0;
	thirdItem->offset = 0;

	if(symbol->id == OPEN) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol->id == LPAR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();

			if(expression(firstItem)) {
				if(symbol->id == COMMA) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					if(expression(secondItem)) {
						if(symbol->id == COMMA) {
							if(hasMoreTokens() == 0) { return 0; }
							getNextToken();
							if(expression(thirdItem)) {
								if(symbol->id == RPAR) {
									if(hasMoreTokens() == 0) { return 0; }
									getNextToken();
									if(symbol->id == SEMCOL) {
										cg_load(firstItem);
										cg_load(secondItem);
										cg_load(thirdItem);
										cg_put( CMD_FLO, firstItem->reg, secondItem->reg, thirdItem->reg );	
										cg_releaseReg(secondItem->reg);
										cg_releaseReg(thirdItem->reg);
										copyItem(item, firstItem);
										item->type->form = TYPE_FORM_INT;
										return 1;
									} else { printError("';' missing."); }							
								} else { printError("')' missing."); }
							} else { printError("expression expected."); }
						} else { printError("',' missing."); }
					} else { printError("expression expected."); }
				} else { printError("',' missing."); }
			} else { printError("identifier expexcted."); }			
		} else { printError("'(' missing."); }
	}
	return 0;
}

bool fileClose(struct item_t *item) {
	if(symbol->id == CLOSE) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol->id == LPAR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(expression(item)) {
				if(symbol->id == RPAR) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					if(symbol->id == SEMCOL) {
						cg_load(item);
						cg_put( CMD_FLC, 0, 0, item->reg );
						item->type->form = TYPE_FORM_INT;
						return 1;
					} else { printError("';' missing."); }
				} else { printError("')' missing."); }
			} else { printError("expression expected."); }
		} else { printError("'(' missing."); }
	} 
	return 0;
}

bool fileWrite(struct item_t *item) {
	int sreg;
	struct item_t *firstItem;
	struct item_t *secondItem;
	struct item_t *thirdItem;
	struct object_t *object;

	firstItem = malloc(sizeof(struct item_t));
	secondItem = malloc(sizeof(struct item_t));	
	thirdItem = malloc(sizeof(struct item_t));	

	firstItem->value = 0;
	firstItem->offset = 0;	
	secondItem->value = 0;
	secondItem->offset = 0;
	thirdItem->value = 0;
	thirdItem->offset = 0;

	object = malloc(sizeof(struct object_t));
	object->type = malloc(sizeof(struct type_t));

	if(symbol->id == WRITE) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol->id == LPAR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(expression(firstItem)) {
				if(symbol->id == COMMA) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();

					if(expression(secondItem)) {
						if(symbol->id == COMMA) {
							if(hasMoreTokens() == 0) { return 0; }
							getNextToken();

							if(expression(thirdItem)) {
								if(symbol->id == RPAR) {
									if(hasMoreTokens() == 0) { return 0; }
									getNextToken();
									if(symbol->id == SEMCOL) {
										//sreg = cg_requestReg();
										//cg_put( CMD_SUBI, sreg, secondItem->reg, (secondItem->offset* (-1)) );
										cg_load(firstItem);
										cg_load(secondItem);
										cg_load(thirdItem);
										cg_put( CMD_WRC, firstItem->reg, secondItem->reg, thirdItem->reg );
										cg_releaseReg(secondItem->reg);
										cg_releaseReg(thirdItem->reg);
										copyItem(item, firstItem);
										item->type->form = TYPE_FORM_INT;
										return 1;
									} else { printError("';' missing."); }							
								} else { printError("')' missing."); }
							} else { printError("expression expected."); }
						} else { printError("',' missing."); }
					} else { printError("expression expected."); }
				} else { printError("',' missing."); }
			} else { printError("identifier expexcted."); }			
		} else { printError("'(' missing."); }
	} 
	return 0;
}

bool fileRead(struct item_t *item) {
	int sreg;
	struct item_t *firstItem;
	struct item_t *secondItem;
	struct item_t *thirdItem;
	struct object_t *object;

	firstItem = malloc(sizeof(struct item_t));
	secondItem = malloc(sizeof(struct item_t));	
	thirdItem = malloc(sizeof(struct item_t));

	firstItem->value = 0;
	firstItem->offset = 0;	
	secondItem->value = 0;
	secondItem->offset = 0;
	thirdItem->value = 0;
	thirdItem->offset = 0;
	
	object = malloc(sizeof(struct object_t));
	object->type = malloc(sizeof(struct type_t));

	if(symbol->id == READ) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol->id == LPAR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(expression(firstItem)) {
				if(symbol->id == COMMA) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();

					if(expression(secondItem)) {
						if(symbol->id == COMMA) {
							if(hasMoreTokens() == 0) { return 0; }
							getNextToken();

							if(expression(thirdItem)) {
								if(symbol->id == RPAR) {
									if(hasMoreTokens() == 0) { return 0; }
									getNextToken();
									if((symbol->id == PLUS) || (symbol->id == MINUS) || (symbol->id == TIMES) || (symbol->id == DIV) || 
										compOp() || (symbol->id == RPAR) || (symbol->id == SEMCOL) || (symbol->id == COMMA) || 
										(symbol->id == RSQBR) || (symbol->id == RCUBR) || (symbol->id == OR)) {
										sreg = cg_requestReg();
										cg_put( CMD_SUBI, sreg, secondItem->reg, (secondItem->offset* (-1)) );
										cg_load(firstItem);
										cg_load(secondItem);
										cg_load(thirdItem);
										cg_put( CMD_RDC, firstItem->reg, secondItem->reg, thirdItem->reg );		
										cg_releaseReg(secondItem->reg);
										cg_releaseReg(thirdItem->reg);
										copyItem(item, firstItem);
										item->type->form = TYPE_FORM_INT;
										return 1;
									} else { printError("';' missing."); }							
								} else { printError("')' missing."); }
							} else { printError("expression expected."); }
						} else { printError("',' missing."); }
					} else { printError("expression expected."); }
				} else { printError("',' missing."); }
			} else { printError("identifier expexcted."); }			
		} else { printError("'(' missing."); }
	} 
	return 0;
}

/* ifCmd = "if" "(" expression ")" block [elseCmd] . */	
bool ifCmd(struct item_t *item) {
	int fJumpAddress;

	if(symbol->id == IF) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol->id == LPAR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(expression(item)) {
				if (item->type->form == TYPE_FORM_BOOL) { 
					cg_loadBool(item);
					cg_cJump(item); 
					cg_fixLink(item->tru);
				} else { printError("boolean expression expected"); }

				if(symbol->id == RPAR) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					if(block()) {
						if(hasMoreTokens() == 0) { return 0; }
						getNextToken();
						if(symbol->id == ELSE) {
							if(hasMoreTokens() == 0) { return 0; }
							getNextToken();
							fJumpAddress = cg_fJump(); 
							cg_fixLink(item->fls);
							if(block()) {
								if(hasMoreTokens() == 0) { return 0; }
								getNextToken();
							}
							cg_fixUp(fJumpAddress);
						} else { cg_fixLink(item->fls); }
					}
					return 1;
				} else { printError("')' missing."); }
			} else { printError("expression expected."); }
		} else { printError("'(' missing."); }
	}
	return 0;
}

bool whileLoop(struct item_t *item) {
	int bJumpAddress;
	if (symbol->id == WHILE) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
	} else { return 0; }
	if (symbol->id == LPAR) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
	} else { printError("missing '('"); return 0;  }
	bJumpAddress = PC;
	expression(item);
	if (item->type->form == TYPE_FORM_BOOL) { 
		cg_loadBool(item);
		cg_cJump(item);
		cg_fixLink(item->tru);
	} else { printError("boolean expression expected"); return 0; } 
	if (symbol->id == RPAR) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
	} else { printError("missing ')'"); return 0; }
	if(block()) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
	}
	cg_bJump(bJumpAddress);
	cg_fixLink(item->fls);
	return 1;
}

int variableDeclarationSequence(struct object_t *head, int isStruct) {
	struct object_t *object;
	struct object_t *ptr;
	struct type_t *type;
	struct type_t *array;
	struct item_t *item;
	int off;
	int decCounter;
	item = malloc(sizeof(struct item_t));
	item->type = malloc(sizeof(struct type_t));
	decCounter = 0;

	item->offset = 0;
	item->value = 0;
	
	if(isStruct != 0) { off = 0; }
	else { off = locOffset; }
	while( 0 < 1 ) {
		type = 0;
		array = 0;
		ptr = 0;
		object = malloc(sizeof(struct object_t));
		if(isStruct == 0) { object->class = OBJECT_CLASS_VAR; } 
		else { object->class = OBJECT_CLASS_FIELD; }

		typedefDec(head);
		if(symbol->id == STRUCT) { 
			if(hasMoreTokens() == 0) { return decCounter; }		
			getNextToken();
		}
		type = newType(typeSpec(item, head));
		object->offset = off;
		if(type->form == 0) { return decCounter; } 
		else { 
			if(type->form == TYPE_FORM_RECORD) {
				ptr = lookUp(head, symbol->valueStr);
				if(ptr == 0) {
					ptr = lookUp(globList, symbol->valueStr);

					if(ptr != 0) {
						object->type = ptr->type;
						/*object->type->form = ptr->type->form;
						object->type->size = ptr->type->size;
						object->type->fields = ptr->type->fields;
						object->type->base = ptr->type->base;*/
					}
				} else { 
					object->type = ptr->type; 
					/*object->type->form = ptr->type->form;
					object->type->size = ptr->type->size;
					object->type->fields = ptr->type->fields;
					object->type->base = ptr->type->base;*/
				}
			}
		}
		off = off - 4;	/* size of local variable int,char 4byte; pointer to struct 4byte  */
		if(hasMoreTokens() == 0) { return decCounter; }
		getNextToken();
		if(symbol->id == TIMES) {
			array = newType(TYPE_FORM_ARRAY);
			array->base = type;
			if(hasMoreTokens() == 0) { return decCounter; }
			getNextToken();			
		}
		if(identifier()) {
			object->name = malloc(64 * sizeof(char));
			strnCpy(object->name, symbol->valueStr, 64);
			if(hasMoreTokens() == 0) { return decCounter; }
			getNextToken();
			if(symbol->id == SEMCOL) {
				object->scope = LOCAL_SCOPE;
				if(type->form != TYPE_FORM_RECORD) {
					if (array != 0) { object->type = array; } 
					else { object->type = type; }
				}

				if(isStruct != 0) { insert(head, object); } 
				else {
					if(lookUp(head, object->name) != 0)	{	//check additionally, if the symbol is in de paramsList
						errorCounter = errorCounter + 1;
						printf("    ");
						printString(srcfile); printf(":");
						printf(symbol->lineNr);printf(":");
						printf(symbol->colNr);printf(": ERROR: multible declaration of ");
						printf(object->name); printf("\n");
					} else {
						insert(locList, object);
					}
				}
				if(hasMoreTokens() == 0) { return decCounter; }
				getNextToken();
			} else { printError("';' missing."); }
		} else { printError("identifier expected."); }
		decCounter = decCounter + 1;
	}
}

bool typedefDec(struct object_t *head) {
	struct object_t *object;
	struct object_t *ptr;
	struct type_t *type;
	struct type_t *array;
	struct type_t *pre_array;
	struct item_t *item;
	type = 0;
	array = 0;
	ptr = 0;
	object = malloc(sizeof(struct object_t));
	object->class = OBJECT_CLASS_TYPE;
	item = malloc(sizeof(struct item_t));
	item->type = malloc(sizeof(struct type_t));
	item->offset = 0;
	item->value = 0;

	if(symbol->id == TYPEDEF) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol->id == STRUCT) { 
			if(hasMoreTokens() == 0) { return 0; }		
			getNextToken();
		}
		type = newType(typeSpec(item, head));
		if(type->form == 0) {
			return 1;
		} else {
			if(type->form == TYPE_FORM_RECORD) {
				ptr = lookUp(head, symbol->valueStr);
				if(ptr == 0) { ptr = lookUp(globList, symbol->valueStr); }
				if(ptr != 0) { object->type = ptr->type; }
			}
		}
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();

		if(symbol->id == TIMES) { 
			array = newType(TYPE_FORM_ARRAY);
			/*if(type->form == TYPE_FORM_RECORD) { 
				array->base = object->type; 
			} 
			else {*/ 
				array->base = type; 
			//}
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
		}

		if(identifier()) { 
			object->name = malloc(64 * sizeof(char));
			strnCpy(object->name, symbol->valueStr, 64);
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(symbol->id == SEMCOL) { 
				if (array != 0) { object->type = array; } 
				else { object->type = type; }
				insert(head, object);
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				return 1;
			} else { printError("';' missing."); } 
		}
	}
	return 0;
}

bool structDec() {
	struct object_t *object;
	struct object_t *fieldObj;
	struct type_t *record;
	struct object_t *count;
	struct object_t *ptr;
	struct object_t *ptr1;
	int i;
	if(symbol->id == STRUCT) {
		fieldObj = malloc(sizeof(struct object_t));
		object = malloc(sizeof(struct object_t));
		object->type = malloc(sizeof(struct type_t));
		object->class = OBJECT_CLASS_TYPE;
		record = 0;
		record = newType(TYPE_FORM_RECORD);

		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		
		if(identifier()) {
			object->name = malloc(64 * sizeof(char));
			strnCpy(object->name, symbol->valueStr, 64);

			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(symbol->id == LCUBR) {

				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				variableDeclarationSequence(fieldObj, 1);

				/* count fields */
				i = 0;
				count = fieldObj;
				while(count != 0) {
					count = count->next;
					i = i + 1;
				}
				record->size = (i * 4);
				if(symbol->id == RCUBR) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
				} else { printError("'}' missing."); }
			}
			if(symbol->id == SEMCOL) {
				record->fields = fieldObj;
				object->type = record;
				/* delete implicite struct declaration */
				if((globList != 0) && (globList->name != 0) && (lookUp(globList, object->name) != 0))	{ 
					if(globList != 0) {
						if(strCmp(globList->name, object->name) == 0) {
							ptr = globList->next;
							globList = ptr;
						} else {
							ptr = globList;
							while(ptr->next != 0) {
								ptr1 = ptr->next;
								if(strCmp(ptr1->name, object->name) == 0) {
									ptr->next = ptr1->next;
								}
								ptr = ptr1;
							}
						}
					}
				}
				object->scope = GLOBAL_SCOPE;
				insert(globList, object);
				return 1;
			} else { printError("';' missing."); }
		}
	}
	return 0;
}

struct object_t *createObject(string_t name) {
	struct object_t *object;
	object = malloc(sizeof(struct object_t));
	object->type = malloc(sizeof(struct type_t));
	object->name = name;
	object->params = 0;
	insert(globList, object);
	return object;
}

struct type_t *basicArrayRecordType() {
	struct type_t *type;
	struct type_t *array;
	struct object_t *ptr;
	ptr = malloc(sizeof(struct object_t));
	type = malloc(sizeof(struct type_t));
	if(symbol->id == INT ) { type->form = TYPE_FORM_INT;  type->size = 4; }
	if(symbol->id == CHAR) { type->form = TYPE_FORM_CHAR; type->size = 4; }
	if(symbol->id == VOID) { type->form = TYPE_FORM_VOID; type->size = 4; }
	if(symbol->id == BOOL) { type->form = TYPE_FORM_BOOL; type->size = 4; }
	if(symbol->id == STRUCT) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(identifier()) {
			ptr = lookUp(globList, symbol->valueStr);
			if((ptr != 0) && (ptr->class == OBJECT_CLASS_TYPE)) {
				type->form = ptr->type->form;
				type->size = ptr->type->size;
				type->fields = ptr->type->fields;
				type->base = ptr->type->base;
			}
		}
	}
	if(identifier()) {
		ptr = lookUp(globList, symbol->valueStr);
		if((ptr != 0) && (ptr->class == OBJECT_CLASS_TYPE)) {
			type->form = ptr->type->form;
			type->size = ptr->type->size;
			type->fields = ptr->type->fields;
			type->base = ptr->type->base;
		}
	}
	if(hasMoreTokens() == 0) { return 0; }
	getNextToken();
	if(symbol->id == TIMES) {
		array = newType(TYPE_FORM_ARRAY);
		array->base = type;
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();	
		if(type->form != TYPE_FORM_RECORD) {
			return array;
		}	
	}
	return type;
}

void prologue(int localSize) {
	cg_put(CMD_PSH, LINK, SPTR, 4); 				// save return address
	cg_put(CMD_PSH, FPTR, SPTR, 4); 				// save caller's frame
	cg_put(CMD_ADD, FPTR, 0, SPTR); 				// allocate callee's frame
	cg_put(CMD_SUBI, SPTR, SPTR, localSize/4); 		// allocate callee's local variables
}

void epilogue(int paramSize) {
	char *main_str;
	main_str = "main";
	cg_put(CMD_ADD, SPTR, 0, FPTR); 				// deallocate callee's frame and local variables 
	cg_put(CMD_POP, FPTR, SPTR, 4); 				// restore caller's frame
	cg_put(CMD_POP, LINK, SPTR, paramSize + 4); 	// restore return address
													// deallocate parameters
	if(strCmp(procedureContext->name, main_str) != 0) {cg_put(CMD_RET, 0, 0, LINK); }			// return 
	else { cg_put(CMD_TRAP, 0, 0, 0); }
}

bool procedureImplementation(struct item_t* item, string_t identifier) {
	char *main_str;
	struct object_t* object;
	main_str = "main";
	typeWA = 0;

	if(strCmp(identifier, main_str) == 0) { mainPos = PC + 1; }		//set jump to main method
	object = findProcedureObject(globList, identifier);
	if (object != 0) {
		if (object->type->form != item->type->form) { printError("return type mismatch in procedure"); return 0; }
		cg_fixLink(object->offset);
	} else {	
		object = createObject(identifier);
		object->class = OBJECT_CLASS_PROC;
	}
	object->type->form = item->type->form;
	object->type->size = item->type->size;
	object->type->fields = item->type->fields;
	object->type->base = item->type->base;
	object->offset = PC;
	object->scope = GLOBAL_SCOPE;
	formalParameters(object);
	returnFJumpAddress = 0;
	if (symbol->id == LCUBR) { 
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
	} else {
		if(symbol->id == SEMCOL) {
			if(object->offset == 1) { object->offset = 0;}
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			return 1;
		} else { printError("missing '{'"); return 0; }
	}
	prologue(variableDeclarationSequence(object, 0) * 4);
	procedureContext = object;
	statementSeq();
	cg_fixLink(returnFJumpAddress);
	epilogue(object->value * 4);
	if (symbol->id == RCUBR) { 
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken(); 
		return 1; 
	} 
	else { printError("missing '}'"); return 0; }
}

bool formalParameters(struct object_t* object) {
	int numberOfParameters; 
	struct object_t* nextParameter;
	numberOfParameters = 0;
	if (symbol->id == LPAR) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
	} 
	else { printError("missing '('"); }
	nextParameter = object->params;
	if ((symbol->id == INT) || (symbol->id == CHAR) || (symbol->id == BOOL) || identifier() || (symbol->id == STRUCT)) {
		nextParameter = formalParameter(object, nextParameter);
		numberOfParameters = numberOfParameters + 1;
		while (symbol->id == COMMA) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			nextParameter = formalParameter(object, nextParameter);
			numberOfParameters = numberOfParameters + 1;
		}
	}
	object->value = numberOfParameters;
	nextParameter = object->params;
	while (nextParameter != 0) { 
		numberOfParameters = numberOfParameters - 1; 
		nextParameter->offset = numberOfParameters * 4 + 8; 
		nextParameter = nextParameter->next;
	}
	if (symbol->id == RPAR) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		return 1;
	}
	else { printError("missing ')'"); } 
	return 0;
}

struct object_t* createFormalParameter(struct object_t* object, struct type_t* type, string_t identifier) {
	struct object_t *ptr;
	struct object_t *ptrNext;
	if(object->params == 0) { object->params = malloc(sizeof(struct object_t)); }
	ptr = object->params;
	if(ptr->name == 0) {
		ptr->name = malloc(sizeof(char)*64);
		ptr->type = malloc(sizeof(struct type_t));
		strnCpy(ptr->name, identifier, 64);
		ptr->type->form = type->form;
		ptr->type->size = type->size;
		ptr->type->fields = type->fields;
		ptr->type->base = type->base;
		ptr->scope = LOCAL_SCOPE;
		ptr->next = 0;
 		return ptr;
	} else {
		while(ptr->next != 0) {
			ptr = ptr->next;
		}
		ptr->next = malloc(sizeof(struct object_t)); 
		ptrNext = ptr->next;
		ptrNext->name = malloc(sizeof(char)*64);
		ptrNext->type = malloc(sizeof(struct type_t));
		strnCpy(ptrNext->name, identifier, 64);
		ptrNext->type->form = type->form;
		ptrNext->type->size = type->size;
		ptrNext->type->fields = type->fields;
		ptrNext->type->base = type->base;
		ptrNext->scope = LOCAL_SCOPE;
		ptrNext->next = 0;
 		return ptrNext;
	}
}

struct object_t* formalParameter(struct object_t* object, struct object_t* formalParameter) {
	struct type_t* type;
	type = basicArrayRecordType();
	if (identifier()) {
		if (formalParameter != 0) {
			if ( (type != 0) && (formalParameter->type != 0) && (type->form != formalParameter->type->form) ) 
			{ printError("type mismatch in procedure declaration and call"); }
		} else { formalParameter = createFormalParameter(object, type, symbol->valueStr); }
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		formalParameter = formalParameter->next;
	} else { printError("identifier expected"); }
	return formalParameter;
}

bool procedureReturn() {
	struct item_t* item;
	if (symbol->id == RETURN) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
	} else { return 0; }
	if ((symbol->id == PLUS) || (symbol->id == MINUS) || identifier() || (symbol->id == INT) ||
		(symbol->id == LPAR) || (symbol->id == STRING) || number() || (symbol->id == CHARACTER) ) {
		item = malloc(sizeof(struct item_t)); 
		item->type = malloc(sizeof(struct type_t));

		expression(item);

		if ( (item->type->form != procedureContext->type->form) 
			&& (item->type->form != TYPE_FORM_INT) && (procedureContext->type->form != TYPE_FORM_CHAR)
			&& (item->type->form != TYPE_FORM_CHAR) && (procedureContext->type->form != TYPE_FORM_INT) ) 
		{ printError("return type mismatch"); return 0; }
		if (item->type->form == TYPE_FORM_BOOL) { cg_unloadBool(item); }
		cg_load(item);
		cg_put(CMD_ADD, RR, 0, item->reg);
		cg_releaseReg(item->reg); 
	}
	returnFJumpAddress = fJumpChain(returnFJumpAddress);
	return 1;
}

bool fJumpChain(int branchAddress) {
	cg_put(CMD_BR, 0, 0, branchAddress);
	return PC - 1;
}

bool pushUsedRegisters() {
	int counter;
	int i; 
	i = 1;
	counter = 0;
	while (i < 25) { 
		if (regs[i] == 1) { 
			cg_put(CMD_PSH, i, SPTR, 4);
			cg_releaseReg(i);
			counter = counter + 1; 
		}
		i = i + 1;
	}
	return counter;
}

void popUsedRegisters(int counter) {
	int i; i = counter;
	while (i > 0) {
		cg_put(CMD_POP, cg_requestReg(), SPTR, 4);
		i = i - 1;
	}
}

bool isBSR(int offset) { return (out_cmd_op[offset] == CMD_BSR); }

void procedureCall(struct item_t* item, string_t procName) {
	struct object_t* object;
	int nrOfRegs;
	string_t identifier;
	nrOfRegs = 0;
	identifier = malloc(sizeof(char)*64);
	strnCpy(identifier, procName, 64);
	object = findProcedureObject(globList, identifier);
	if (object == 0) {
		printError("undeclared procedure: ");
		object = createObject(identifier);
		object->class = OBJECT_CLASS_PROC;
		object->type->form = TYPE_FORM_UNKNOWN;
		object->offset = 0;
	}
	item->mode = ITEM_MODE_REG;
	item->type = object->type; // type of return value
	nrOfRegs = pushUsedRegisters();
	actualParameters(object);
	if ((object->offset != 0) && (isBSR(object->offset) == 0)) { sJump(object->offset - PC); }
	else { object->offset = sJump(object->offset); }
	popUsedRegisters(nrOfRegs);
	item->reg = cg_requestReg(); 
	cg_put(CMD_ADD, item->reg, 0, RR);
}

bool sJump(int branchAddress) {
	cg_put(CMD_BSR, 0, 0, branchAddress);
	return PC - 1; 
}

void pushParameter(struct item_t* item) {
	if (item->type->form == TYPE_FORM_BOOL) { cg_unloadBool(item); }
	cg_load(item);
	cg_put(CMD_PSH, item->reg, SPTR, 4);
	cg_releaseReg(item->reg); 
}

bool actualParameters(struct object_t* object) {
	struct object_t* nextFormalParameter;
	struct item_t* item;
	if (symbol->id == LPAR) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
	} 
	else { printError("missing '('"); }
	nextFormalParameter = object->params;
	if ((symbol->id == PLUS) || (symbol->id == MINUS) || identifier() || (symbol->id == INT) ||
		(symbol->id == LPAR) || (symbol->id == STRING) || number()) {
		nextFormalParameter = actualParameter(object, nextFormalParameter);
		while (symbol->id == COMMA) { 
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			nextFormalParameter = actualParameter(object, nextFormalParameter);
		}
	}
	while (nextFormalParameter != 0) {
		printError("actual parameter expected");
		item = malloc(sizeof(struct item_t));
		item->type = malloc(sizeof(struct type_t));
		item->mode = ITEM_MODE_CONST; 
		item->type->form = TYPE_FORM_INT; 
		item->value = 0;
		pushParameter(item);
		nextFormalParameter = nextFormalParameter->next;
	}
	if (symbol->id == RPAR) {
		return 1;
	} 
	else { printError("missing ')'"); }
	return 0;
}

struct object_t* createAnonymousParameter(struct object_t* object, struct type_t *type) {
	struct object_t *ptr;
	struct object_t *ptrNext;
	if(object->params == 0) { object->params = malloc(sizeof(struct object_t)); }
	ptr = object->params;
	if(ptr->name == 0) {
		ptr->type = malloc(sizeof(struct type_t));
		ptr->type->form = type->form;
		ptr->type->size = type->size;
		ptr->type->fields = type->fields;
		ptr->type->base = type->base;
		ptr->scope = LOCAL_SCOPE;
		ptr->next = 0;
	} else {
		while(ptr->next != 0) {
			ptr = ptr->next;
		}
		ptr->next = malloc(sizeof(struct object_t)); 
		ptrNext = ptr->next;
		ptrNext->type = malloc(sizeof(struct type_t));
		ptrNext->type->form = type->form;
		ptrNext->type->size = type->size;
		ptrNext->type->fields = type->fields;
		ptrNext->type->base = type->base;
		ptrNext->scope = LOCAL_SCOPE;
		ptrNext->next = 0;
	}
 	return ptr;
}

struct object_t* actualParameter(struct object_t* object, struct object_t* formalParameter) {
	struct item_t* item;
	if ((symbol->id == PLUS) || (symbol->id == MINUS) || identifier() || (symbol->id == INT) ||
		(symbol->id == LPAR) || (symbol->id == STRING) || number()) {
		item = malloc(sizeof(struct item_t));
		if (expression(item)) {
			if (formalParameter != 0) {
				if ( (item->type != 0) && (item->type->form != formalParameter->type->form) ) 
				{	printError("type mismatch in procedure call"); }
			} else { formalParameter = createAnonymousParameter(object, item->type); }
			pushParameter(item);
		} else {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
		}
		formalParameter = formalParameter->next;
	} else { printError("actual parameter expected"); }
	return formalParameter;
}

bool globalDec() {
	struct object_t *object;
	struct object_t *ptr;
	struct type_t *type;
	struct type_t *array;
	struct item_t *item;
	string_t identName;

	while( 0 < 1 ) { 	
		type = 0;
		array = 0;
		ptr = 0;
		object = 0;
		item = 0;
		object = malloc(sizeof(struct object_t));
		identName = malloc(sizeof(char)*64);
		item = malloc(sizeof(struct item_t));
		item->type = malloc(sizeof(struct type_t));
		item->offset = 0;
		item->value = 0;

		while(typedefDec(globList)) {}
		while((typeSpec(item, globList) == 0) && (symbol->id != STRUCT) && hasMoreTokens()) {
			printError("typeSpec (char, int or void) expected.");
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
		}

		if(symbol->id == STRUCT) { 
			if(hasMoreTokens() == 0) { return 1; }		
			getNextToken();
		}

		type = newType(typeSpec(item, globList));
		object->offset = globOffset;
		if(type->form == 0) {
			return 0;
		} else {
			if(type->form == TYPE_FORM_RECORD) {
				ptr = lookUp(globList, symbol->valueStr);
				if(ptr != 0) { object->type = ptr->type; } 
				else { printError("Type not found."); }
			}
		}
		globOffset = globOffset - 4;

		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();

		if(symbol->id == TIMES) {
			array = newType(TYPE_FORM_ARRAY);
			array->base = type;
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();			
		}
		if(identifier()) {
			object->name = malloc(64 * sizeof(char));
			strnCpy(object->name, symbol->valueStr, 64);
			strnCpy(identName, symbol->valueStr, 64);
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			/* procHead */
			if(symbol->id == LPAR) {
				locList = 0;
				locList = malloc(sizeof(struct object_t));
				locOffset = -4;
				procedureImplementation(item, identName); 
			} 
			/* declaration */
			else {
				if(symbol->id == SEMCOL) {
					nrOfGVar = nrOfGVar + 1;
					object->class = OBJECT_CLASS_VAR;
					object->scope = GLOBAL_SCOPE;
					if(type->form != TYPE_FORM_RECORD) {
						if (array != 0) {
							object->type = array;
						} else {
							object->type = type;
						}
					}
					insert(globList, object);
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
				}
			}
		}
	}
}

bool statementSeq () {
	struct item_t *item;
	if(symbol->id == RCUBR) { return 1; }
	while( 0 < 1 ) {
		item = malloc(sizeof(struct item_t));
		item->type = malloc(sizeof(struct type_t));
		item->offset = 0;
		item->value = 0;

		while( (identifier() == 0) && (number() == 0) && (symbol->id != WHILE) && (symbol->id != IF)
				&& (symbol->id != RETURN) && (symbol->id != LPAR) && (symbol->id != ELSE) && (symbol->id != PRINTF)
				&& (symbol->id != OPEN) && (symbol->id != CLOSE) && (symbol->id != WRITE) && (symbol->id != READ)) {
			printError("identifier, number, while, if or return expected.");
			if(hasMoreTokens() == 0) { return 0; }			
			getNextToken();
		}

		if(ifCmd(item)) {} 
		else { 
			if(printMethod(item)) {}
			else { 
				if(whileLoop(item)) {}
				else { 
					if(expression(item) || procedureReturn()) {
						if(symbol->id == SEMCOL) {
							if(hasMoreTokens() == 0) { return 0; }
							getNextToken();
						} else {
							printError("';' missing.");	
						}
					}
					else {
						while( (symbol->id != SEMCOL) && (symbol->id != RPAR) && (symbol->id != RCUBR) ) {
							if(hasMoreTokens() == 0) { return 0; }
							getNextToken();
						}
					}
				}
			}
		}
		cg_releaseReg(item->reg);
		if(symbol->id == RCUBR) { return 1; }
	}
}

bool block() {
	if(symbol->id == LCUBR) {
		if(hasMoreTokens() == 0) { return 0; }			
		getNextToken();
		statementSeq();
		if(symbol->id == RCUBR) { return 1; }
		printError("'}' missing.");			
	}
	return 0;
}

