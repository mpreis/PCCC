/*
 * PSEUDOCODE - parser.c
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */
#include "parser.h"
#include "scanner.h"
#include "tokenMapping.h"
#include "symboltable.h"
#include "./tm/tmCmd.h"

struct item_t {
	int mode;
	struct type_t *type;
	int reg;
	int offset;
	int value;
};

char *srcfile;
char *outfile;
int out_fp_bin;
int out_fp_ass;
struct object_t *globList;
struct object_t *locList;

int *regs;
int nrOfRegs;
int ITEM_MODE_CON;
int ITEM_MODE_VAR;
int ITEM_MODE_REF;
int ITEM_MODE_REG;

/*************************************************************
 * CODE GENERATION METHODS
 ************************************************************/

void testCodeGen() {
	/* 2 = 5 - 3 */
	put(ADDI, 1, 0, 5);
	put(ADDI, 2, 0, 3);
	put(SUB,  1, 1, 2);
	put(TRAP, 0, 0, 0);
}

int initOutputFile() {
	out_fp_bin = open(outfile, 65, 448); /* 65 ... O_CREAT | O_WRONLY 448 ... S_IWUSR | S_IRUSR | S_IXUSR */
	out_fp_ass = open("./out.txt", 65, 448); /* 65 ... O_CREAT | O_WRONLY 448 ... S_IWUSR | S_IRUSR | S_IXUSR */
	if(out_fp_bin < 0) {
		printError("can not open/create output file.");
		return -1;
	}
}

int encode(int op, int a, int b, int c) {
	if (c < 0)
	c = c + 65536; // 0x10000: 2^16
	return (((((op * 32) + a) * 32) + b) * 65536) + c;
}

void put(int op, int a, int b, int c) {
	int wb; int size;
	int *abuffParam;
	int *buff; 
	char *abuff;

	write (out_fp_ass, getCmdName(op), 4);
	write (out_fp_ass, " ", 1);
	size = 6 * sizeof(char);
	abuff = malloc(size);
	abuff[0] = (a + '0');
	abuff[1] = ' ';
	abuff[2] = (b + '0');
	abuff[3] = ' ';
	abuff[4] = (c + '0');
	abuff[5] = 0;
	write (out_fp_ass, abuff, 5);
	write (out_fp_ass, "\n", 1);

/*
	size = 2 * sizeof(char *);
	abuff = malloc (size);
	abuff[0] = '\n';
	abuff[1] = getCmdName(op);
	write (out_fp_ass, abuff, size);


	size = 3 * sizeof(int);
	abuffParam = malloc(size);
	write (out_fp_ass, abuffParam, size);
*/
	buff = malloc(1*32);
	buff[0] = encode(op,a,b,c);	
	wb = write(out_fp_bin, buff, 4);
    if ( wb != 4 ) { printf(" --- could only write %i byte.\n", wb); }
}

void initItemModes() {
	ITEM_MODE_CON = 0;
	ITEM_MODE_VAR = 1;
	ITEM_MODE_REF = 2;
	ITEM_MODE_REG = 3;
}

/***  REGISTER METHODS ***/
void initRegs() {
	int i; i = 0;
	nrOfRegs = 32;
	regs = malloc( nrOfRegs*sizeof(int) );
	while (i < nrOfRegs) { regs[i] = 0; i = i + 1; }
}
int requestReg() {
	int i; i = 0;
	while (i < nrOfRegs) { 
		if (regs[i] == 0) { return i; }
		i = i + 1;
	}
	return -1;		
}
void releaseReg(int r) { regs[r] = 0; }

/*************************************************************
 * HELP METHODS
 ************************************************************/
void printError(char *msg) {
	printf("\t%s:%4i:%4i: ERROR: %s (found: %s/%i)\n", srcfile, symbol->lineNr, symbol->colNr, msg, symbol->valueStr, symbol->id);
}

/*************************************************************
 * PASING METHODS
 ************************************************************/

/* ["-"] digit {digit} . */
int number() {
	if(symbol->id == MINUS)  { 
		if(hasMoreTokens() == 0) { return 0; }	
		getNextToken(); 
	}
	if(symbol->id == NUMBER) { return 1; }
	return 0;
}

/* "==" | "!=" | "<=" | ">=" | "<" | ">" | "&&" | "||" . */
int boolOp() {
	if( symbol->id == EQ || symbol->id == NEQ || symbol->id == LT || symbol->id == GT || 
		symbol->id == LET || symbol->id == GET || symbol->id == AND || symbol->id == OR) {
		return 1;
	}
	return 0;
}

int op() {
	if(symbol->id == PLUS || symbol->id == MINUS || symbol->id == DIV || symbol->id == TIMES || boolOp()) {
		return 1;
	}
	return 0;
}

/* letter {letter | digit} . */
int identifier() {
	if(symbol->id == IDENT) { return 1; }
	return 0;
}

/* "int" | "char" . */ 
int typeSpec(struct object_t *head) {	
	if(symbol->id == TYPEDEF) { 
		if(hasMoreTokens() == 0) { return 0; }		
		getNextToken(); 
	}
	if(symbol->id == INT ) { return 1; }
	if(symbol->id == CHAR) { return 2; }		/* return value for symbol table */
	if(symbol->id == VOID) { return 3; }
	if(identifier()) {
		if(lookUp(globList, symbol->valueStr) != 0) { 
			return 5;
		}
		if(lookUp(head, symbol->valueStr) != 0) { 
			return 5;
		} else {
			printError("unknown type.");
		}
	}
	return 0;
}

/* 
	ident.ident 
		 |
	current position
		 |
	ident->ident
*/
int reference() {
	if(symbol->id != ARROW && symbol->id != DOT) {
		return 0;
	}
	if(hasMoreTokens() == 0) { return 0; }
	getNextToken();
	if(identifier()) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol->id == LSQBR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();	
			if(identifier() || number()) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				if(symbol->id == RSQBR) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					return 1; 
				} 
				else { printError("']' missing."); }
			}
		}
		return 1;
	}
	return 0;
}

/* include . */
int include() {
	if(symbol->id == INCLUDE) { return 1; }
	return 0;
}

/* sizeof "(" (typeSpec | identifier) ")" . */
int sizeOf(struct object_t *head) {
	if(symbol->id == SIZEOF) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol->id == LPAR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(typeSpec(head)) {
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

/* identifier | number | procCall | reference | "(" arithExp ")" . */
int factor() {
	if(number() || symbol->id == CHARACTER || symbol->id == STRING) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(op()) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			return expression();
		}
		return 1;
	}
	else if(symbol->id == LPAR) { 
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(expression()) {
			if(symbol->id == RPAR) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				return 1; 
			} else {
				printError("')' missing.");
			}
		} else {
			printError("expression expected.");
		}
	}
	else if(symbol->id == TIMES) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(identifier()) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			return 1;
		}
	}
	else if(sizeOf(globList)) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(op()) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			return expression(); 
		}
		return 1;
	}
	else if(identifier()) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		/* var = */		
		if(symbol->id == EQSIGN) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			return expression();
		}

		/* var +  */
		else if(op()) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			return expression();
		}		

		/* var -> | var . */
		else if(reference()) {			
			if(symbol->id == EQSIGN || op()) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				return expression();
			}
			return 1; 
		}

		/* var ) */
		else if(symbol->id == RPAR) {
			return 1;
		}

		/* var ( */
		else if(symbol->id == LPAR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			paramList();
			if(symbol->id == RPAR) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				if(op()) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					return expression();
				}
				return 1;
			} else {
				printError("')' missing.");
			}
			return 0;
		}

		/* var [ */
		else if(symbol->id == LSQBR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(number() || identifier()) {	
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				if(symbol->id == RSQBR) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					/* var [1] = | var [1] + */
					if(symbol->id == EQSIGN || op()) {
						if(hasMoreTokens() == 0) { return 0; }
						getNextToken();
						return expression();	
					}
					return 1;
				} else { printError("']' missing."); }
			} else { printError("identifier or number expected."); }
			return 0;
		}

		/* var = */		
		if(symbol->id == SEMCOL) { return 1; }
	}
	return 0;
}

/* factor { ( "*" | "/" ) factor } . */
int term() {
	while(1) {
		if(factor() == 0) { return 0; }
		if(symbol->id == PLUS || symbol->id == MINUS || boolOp() || symbol->id == RPAR || 
			symbol->id == SEMCOL) {
			return 1;
		}
		if(symbol->id == TIMES || symbol->id == DIV) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
		} else {
			printError("'*', '/' or ';' expected.");
			return 0;
		}
	}
}

/* [ "-" ] term { ( "+" | "-" ) term } . */
int arithExp() {
	if(symbol->id == MINUS) { 
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken(); 
	}
	while(1) {
		if(term() == 0) { return 0; }
		if(boolOp() || symbol->id == RPAR || symbol->id == SEMCOL || hasMoreTokens() == 0) {
			return 1;
		}
		if(symbol->id == PLUS || symbol->id == MINUS) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
		} else {
			printError("'+', '-' or ';' expected..");
			return 0;
		}
	}
}

/* arithExp { boolOp arithExp } . */
int expression() {
	while(1) {
		if(arithExp() == 0) { return 0; }
		if(symbol->id == RPAR || symbol->id == SEMCOL || hasMoreTokens() == 0) {
			return 1;
		}
		if(boolOp()) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
		} else {
			printError("boolean operator  or ';' expected.");
			return 0;
		}
	}
}

/* identifier {"," identifier} . */
int paramList() {
	if(symbol->id == RPAR) { return 1; }
	while(1) {
		if(identifier() == 0 && number() == 0 && symbol->id != STRING && symbol->id != CHARACTER) {
			printError("paramList: identifier expected.");
			return 0;
		}
		if(hasMoreTokens() == 0) { return 1; }
		getNextToken();
		if(symbol->id == DOT || symbol->id == ARROW) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(identifier()) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				if(symbol->id == LSQBR) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					if(identifier() || number()) {
						if(hasMoreTokens() == 0) { return 0; }
						getNextToken();
						if(symbol->id == RSQBR) {
							if(hasMoreTokens() == 0) { return 0; }
							getNextToken();
						} else { printError("']' missing."); }
					}
				}
			} else {
				printError("paramList(2): identifier expected.");
			}
		}
		if(symbol->id == RPAR) { return 1; }
		if(symbol->id == COMMA) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
		} else {
			printError("')' missing.");
			return 1;
		}
	}
}

/* ifCmd		=	"if" "(" expression ")" block [elseCmd] . */	
int ifCmd() {
	if(symbol->id == IF) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol->id == LPAR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(expression()) {
				if(symbol->id == RPAR) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					if(block()) {
						if(hasMoreTokens() == 0) { return 0; }
						getNextToken();
						if(symbol->id == ELSE) {
							if(hasMoreTokens() == 0) { return 0; }
							getNextToken();
							if(symbol->id == IF) {
								ifCmd();
							} else {
								block();
								if(hasMoreTokens() == 0) { return 0; }
								getNextToken();
							}
						}
					}
					return 1;
				} else { printError("')' missing."); }
			} else { printError("expression expected."); }
		} else { printError("'(' missing."); }
	}
	return 0;
}

/* "while" "(" expression ")" block . */
int whileLoop() {
	if(symbol->id == WHILE) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol->id == LPAR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(expression()) {
				if(symbol->id == RPAR) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					if(block()) { 
						if(hasMoreTokens() == 0) { return 0; }
						getNextToken();
						return 1; 
					}
				} else { printError("')' missing."); }
			} else { printError("expression expected."); }
		} else { printError("'(' missing."); }
	}
	return 0;
}

/* ret = "return" expression . */
int ret() {
	if(symbol->id == RETURN) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(expression()) { 
			return 1; 
		} else {
			printError("expression expected.");
		}
	}
	return 0;
}

int procParList(struct object_t *head) {
	struct object_t *object;
	struct object_t *ptr;
	struct type_t *type;
	struct type_t *array;

	if(symbol->id == RPAR) { return 1; }
	while(1) {
		type = 0;
		array = 0;
		ptr = 0;
		object = malloc(sizeof(struct object_t));
		object->class = 1;	/* VAR */

		if(symbol->id == RPAR) { return 1; }
		if(symbol->id == STRUCT) { 
			if(hasMoreTokens() == 0) { return 0; }		
			getNextToken();
		}
		type = newType(typeSpec(head));
		if(type->form == 0) {
			return 1;
		} else if(type->form == 5) {
			ptr = lookUp(head, symbol->valueStr);
			if(ptr == 0) {
				ptr = lookUp(globList, symbol->valueStr);
				if(ptr != 0) {
					object->type = ptr->type;
				}
			} else {
				object->type = ptr->type;
			}
		}

		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol->id == TIMES) {
			array = newType(4);
			array->base = type;
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
		}
		if(identifier()) {
			object->name = malloc(64 * sizeof(char));
			strnCpy(object->name, symbol->valueStr, 64);
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(symbol->id == LSQBR) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				if(symbol->id == RSQBR) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
				} else { 
					printError("']' missing.");
					return 0; 
				}
			}
			if(symbol->id == RPAR) {
				if(type->form != 5) {
					if (array != 0) {
						object->type = array;
					} else {
						object->type = type;
					}
				}
				insert(head, object);
				return 1; 
			}
			if(symbol->id == COMMA) {
				if(type->form != 5) {
					if (array != 0) {
						object->type = array;
					} else {
						object->type = type;
					}
				}
				insert(head, object);
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
			} else {
				printError("',' missing.");
			}
		} else { return 0; }
	}
}

/* declaration = (typedefDec | typeSpec ["*"] identifier) ";" . */
/* declaration = (typedefDec | typeSpec [TIMES] identifier) SEMCOL . */
int declaration(struct object_t *head, int isStruct) {
	struct object_t *object;
	struct object_t *ptr;
	struct type_t *type;
	struct type_t *array;

	while(1) {
		type = 0;
		array = 0;
		ptr = 0;
		object = malloc(sizeof(struct object_t));
		if(isStruct == 0) {
			object->class = 1;	/* VAR */
		} else {
			object->class = 3;	/* FIELD */
		}
		typedefDec(head);
		if(symbol->id == STRUCT) { 
			if(hasMoreTokens() == 0) { return 0; }		
			getNextToken();
		}
printSymbol("before new Type: ");
		type = newType(typeSpec(head));
printSymbol("after  new Type: ");
		if(type->form == 0) {
			return 1;
		} else if(type->form == 5) {
			ptr = lookUp(head, symbol->valueStr);
			if(ptr == 0) {
				ptr = lookUp(globList, symbol->valueStr);
				if(ptr != 0) {
					object->type = ptr->type;
				}
			} else {
				object->type = ptr->type;
			}
		}

		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol->id == TIMES) {
			array = newType(4);
			array->base = type;
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();			
		}
		if(identifier()) {
			object->name = malloc(64 * sizeof(char));
			strnCpy(object->name, symbol->valueStr, 64);
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(symbol->id == SEMCOL) {
				if(type->form != 5) {
					if (array != 0) {
						object->type = array;
					} else {
						object->type = type;
					}
				}
				insert(head, object);
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
			} else { printError("';' missing."); }
		} else { printError("identifier expected."); }
	}
}

/* "typedef" ("struct" ident | typeSpec) ["*"] ident */
int typedefDec(struct object_t *head) {
	struct object_t *object;
	struct object_t *ptr;
	struct type_t *type;
	struct type_t *array;
	type = 0;
	array = 0;
	ptr = 0;
	object = malloc(sizeof(struct object_t));
	object->class = 2;	/* TYPE */

	if(symbol->id == TYPEDEF) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol->id == STRUCT) { 
			if(hasMoreTokens() == 0) { return 0; }		
			getNextToken();
		}
		type = newType(typeSpec(head));
		if(type->form == 0) {
			return 1;
		} else if(type->form == 5) {
			ptr = lookUp(head, symbol->valueStr);
			if(ptr != 0) {
				object->type = ptr->type;
			}
		}
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();

		if(symbol->id == TIMES) { 
			array = newType(4);
			array->base = type;
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
		}
		if(identifier()) { 
			object->name = malloc(64 * sizeof(char));
			strnCpy(object->name, symbol->valueStr, 64);
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(symbol->id == SEMCOL) { 
				if(type->form != 5) {
					if (array != 0) {
						object->type = array;
					} else {
						object->type = type;
					}
				}
				insert(head, object);
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				return 1;
			} else { printError("';' missing."); } }
	}
	return 0;
}

/* "typedef" "struct" "{" declaration {declaration} "}" identifier ";" . */
/* UPDATE: "struct" identifier "{" declaration {declaration} "}" ";". */
int structDec() {
	struct object_t *object;
	struct object_t *fieldObj;
	struct type_t *record;
	if(symbol->id == STRUCT) {
		fieldObj = malloc(sizeof(struct object_t));
		object = malloc(sizeof(struct object_t));
		object->class = 2;	/* TYPE */
		record = 0;
		record = newType(5);
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
				declaration(fieldObj, 1);
				if(symbol->id == RCUBR) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
				} else { printError("'}' missing."); }
			}
			if(symbol->id == SEMCOL) {
				record->fields = fieldObj;
				object->type = record;
				if(globList->name != 0 && lookUp(globList, object->name) != 0)	{ /* delete implicite declaration */
					struct object_t *ptr, *ptr1;
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
				insert(globList, object);
				return 1;
			} else { printError("';' missing."); }
		}
	}
	return 0;
}

int globalDec() {
	struct object_t *object;
	struct object_t *ptr;
	struct type_t *type;
	struct type_t *array;

	while(1) { 	
		type = 0;
		array = 0;
		ptr = 0;
		object = malloc(sizeof(struct object_t));
		while(typedefDec(globList)) {}
//		while(structDec(globList)) {}
		while((typeSpec(globList) == 0 || symbol->id == STRUCT) && hasMoreTokens()) {
			printError("globalDec: typeSpec (char, int or void) expected.");
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
		}
		if(symbol->id == STRUCT) { 
			if(hasMoreTokens() == 0) { return 1; }		
			getNextToken();
		}
		type = newType(typeSpec(globList));
		if(type->form == 0) {
			return 0;
		} else if(type->form == 5) {
			ptr = lookUp(globList, symbol->valueStr);
			if(ptr != 0) {
				object->type = ptr->type;
			}
		}
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol->id == TIMES) {
			array = newType(4);
			array->base = type;
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();			
		}
		if(identifier()) {
printf(" %s\n", symbol->valueStr);
			object->name = malloc(64 * sizeof(char));
			strnCpy(object->name, symbol->valueStr, 64);
			locList = malloc(sizeof(struct object_t));
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			/* procHead */
			if(symbol->id == LPAR) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				if(procParList(locList)) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					if(symbol->id == SEMCOL) { return 1; } 
					else if(symbol->id == LCUBR) {
						if(hasMoreTokens() == 0) { return 0; }
						getNextToken();

						declaration(locList, 0);
if(locList->name != 0) {
	printTable(locList);
}
						if(statementSeq()) {
							if(symbol->id == RCUBR) { return 1; }
							else { printError("'}' missing."); } 
						}						
					}
				}
			} 
			/* declaration */
			else if(symbol->id == SEMCOL) {
				object->class = 1;	/* VAR */
				if(type->form != 5) {
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

int statementSeq () {
	if(symbol->id == RCUBR) { return 1; }
	while(1) {
		while(identifier() == 0 && number() == 0 && symbol->id != WHILE && symbol->id != IF 
				&& symbol->id != RETURN && symbol->id != LPAR && symbol->id != ELSE) {
			if(hasMoreTokens() == 0) { return 0; }			
			getNextToken();
			printError("statSeq(1): identifier, number, while, if or return expected.");
		}
		if(expression() || ret()) {
			if(symbol->id == SEMCOL) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
			} else {
				printError("';' missing.");			
			}
		}
		else if(ifCmd()) {} 
		else if(whileLoop()) {}

		if(symbol->id == RCUBR) {
			return 1;
		}
	}
}

/* "{" statementSeq "}" . */
int block() {
	if(symbol->id == LCUBR) {
		if(hasMoreTokens() == 0) { return 0; }			
		getNextToken();
		statementSeq();
		if(symbol->id == RCUBR) { return 1; }
		printError("'}' missing.");			
	}
	return 0;
}

/* {include} {declaration | structDec} {procedure} . */
int programm() {
	int i;
	int j;
	int k;
	i = 1; j = 1; k = 1;

	while(i) {
		i = include();
		if(i == 1) { getNextToken(); }
	}
	printf("===== include   -> done.\n");
	while(j) {
		j = structDec();
		if(j == 1) { getNextToken(); }
	}
	printf("===== structDec -> done.\n");
	while(k) {
		k = globalDec();
		if(k) { getNextToken(); }
	}
	printf("===== globalDec -> done.\n");
	return i;
}

/*************************************************************
 * START PARSER
 ************************************************************/
int startParsing(char *sfile, char *ofile){
	int i;
	srcfile = sfile;
	outfile = ofile;	

	/* init global symbol table */
	globList = malloc(sizeof(struct object_t));
	globList = 0;
	initRegs();
	initItemModes();
	initOutputFile();
	initTMCmd();

	testCodeGen();

	printf("\nstart parsing %s...\n", srcfile);
	while ( hasMoreTokens() ) {
		getNextToken();
		i = programm();
	}
	printTable(globList);
	close(out_fp_bin); 
	close(out_fp_ass); 
	printf("\n -- DONE. --\n\n");
	return i;

}
