/*
 * PSEUDOCODE - parser.c
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */

/* TODO only to reduce warnings */
#include <stdlib.h>
#include <stdio.h>

#include "parser.h"
#include "scanner.h"
#include "tokenMapping.h"
#include "./tm/tmCmd.h"

/* code generation */
char *srcfile;
char *outfile;
int out_fp_bin;
int out_fp_ass;

int GP;
int LP;

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


/* symbol table */
int GLOBAL_SCOPE;
int LOCAL_SCOPE;

int TYPE_FORM_INT;
int	TYPE_FORM_CHAR;
int	TYPE_FORM_VOID;
int	TYPE_FORM_ARRAY;
int	TYPE_FORM_RECORD;

int	OBJECT_CLASS_VAR;
int	OBJECT_CLASS_TYPE;
int	OBJECT_CLASS_FIELD;

int globOffset;
int locOffset;
int paramOffset;
char *file;
struct object_t *globList;
struct object_t *locList;

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
//	abuff[0] = (a + '0');
	abuff[0] = a;
	abuff[1] = ' ';
//	abuff[2] = (b + '0');
	abuff[2] = b;
	abuff[3] = ' ';
//	abuff[4] = (c + '0');
	abuff[4] = c;
	abuff[5] = 0;
	write (out_fp_ass, abuff, 5);
	write (out_fp_ass, "\n", 1);

	buff = malloc(1*32);
	buff[0] = encode(op,a,b,c);	
	wb = write(out_fp_bin, buff, 4);
    if ( wb != 4 ) { printf(" --- could only write %i byte.\n", wb); }
}

void copyItem(struct item_t *copy, struct item_t *orig) {
	copy->mode = orig->mode;
	copy->type = orig->type;
	copy->reg = orig->reg;
	copy->offset = orig->offset;
	copy->value = orig->value;
}

void initItemModes() {
	ITEM_MODE_NONE = 0;
	ITEM_MODE_CONST = 1;
	ITEM_MODE_VAR = 2;
	ITEM_MODE_REF = 3;
	ITEM_MODE_REG = 4;
}

void initSymbolTable() {
	globList = malloc(sizeof(struct object_t));

	GLOBAL_SCOPE = 0;
	LOCAL_SCOPE = 1;

	TYPE_FORM_INT = 1;
	TYPE_FORM_CHAR = 2;
	TYPE_FORM_VOID = 3;
	TYPE_FORM_ARRAY = 4;
	TYPE_FORM_RECORD = 5;

	OBJECT_CLASS_VAR = 1;
	OBJECT_CLASS_TYPE = 2;
	OBJECT_CLASS_FIELD = 3;
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
void initRegs() {
	int i; i = 0;
	nrOfRegs = 32;
	GP = 28;
	LP = 29;
	regs = malloc( nrOfRegs*sizeof(int) );
	while (i < nrOfRegs) { regs[i] = 0; i = i + 1; }
}
int requestReg() {
	int i; i = 1;
	/* register for calculation: 1-27 */
	while (i < 28) { 
		if (regs[i] == 0) { return i; }
		i = i + 1;
	}
	return -1;		
}
void releaseReg(int r) { regs[r] = 0; }

void cg_const2Reg(struct item_t *item) {
	item->mode = ITEM_MODE_REG;
	item->reg = requestReg();
	put(ADDI, item->reg, 0, item->value);
	item->value = 0;
	item->offset = 0;
}

void cg_var2Reg(struct item_t *item) {
	printf(" -- cg_var2Reg\n");
	int newReg;
	item->mode = ITEM_MODE_REG;
	newReg = requestReg();
	put(LDW, newReg, item->reg, item->offset);
	item->reg = newReg;
	item->offset = 0;
}

void cg_ref2Reg(struct item_t *item) {
	item->mode = ITEM_MODE_REG;
	put(LDW, item->reg, item->reg, item->offset);
	item->offset = 0;
}

void cg_load(struct item_t *item) {
		 if(item->mode == ITEM_MODE_CONST) { cg_const2Reg(item); }
	else if(item->mode == ITEM_MODE_VAR) { cg_var2Reg(item); }
	else if(item->mode == ITEM_MODE_REF) { cg_ref2Reg(item); }
}


void cg_calcArithExp(struct item_t *leftItem, struct item_t *rightItem, int op) {
	printf("\n\n================================================================================\n");
	printf(" CG: "); printItem(leftItem);
	printf(" CG: "); printItem(rightItem);
	printf(" CG: OP: %i\n", op);
	if(leftItem->type->form == TYPE_FORM_INT && rightItem->type->form == TYPE_FORM_INT) {
		if(rightItem->mode == ITEM_MODE_CONST) {
			if(leftItem->mode == ITEM_MODE_CONST) {
					 if(op == OP_ADD) { leftItem->value = leftItem->value + rightItem->value; } 
				else if(op == OP_SUB) { leftItem->value = leftItem->value - rightItem->value; } 
				else if(op == OP_MUL) { leftItem->value = leftItem->value * rightItem->value; } 
				else if(op == OP_DIV) { leftItem->value = leftItem->value / rightItem->value; } 
				else { printError("nich so gut..."); }
			} else {
				printf(" -- var op const ");
				printItem(leftItem);
				cg_load(leftItem);
					 if(op == OP_ADD) { put(ADDI, leftItem->reg, leftItem->reg, rightItem->value); } 
				else if(op == OP_SUB) { put(SUBI, leftItem->reg, leftItem->reg, rightItem->value); } 
				else if(op == OP_MUL) { put(MULI, leftItem->reg, leftItem->reg, rightItem->value); } 
				else if(op == OP_DIV) { put(DIVI, leftItem->reg, leftItem->reg, rightItem->value); } 
				else { printError("nich so gut..."); }
			}
		} else {
			cg_load(leftItem);
			cg_load(rightItem);
				 if(op == OP_ADD) { put(ADD, leftItem->reg, leftItem->reg, rightItem->reg); } 
			else if(op == OP_SUB) { put(SUB, leftItem->reg, leftItem->reg, rightItem->reg); } 
			else if(op == OP_MUL) { put(MUL, leftItem->reg, leftItem->reg, rightItem->reg); } 
			else if(op == OP_DIV) { put(DIV, leftItem->reg, leftItem->reg, rightItem->reg); } 
			else { printError("nich so gut..."); }
			releaseReg(rightItem->reg);
		}
	} else { printError("Type mismatch (int expected)."); }
	printf(" END CG: "); printItem(leftItem);
	printf("\n================================================================================\n\n");
}

void cg_field(struct item_t *item, struct object_t *object) {
	cg_load(item);
	item->mode = ITEM_MODE_REF;
	item->type = object->type;
	item->offset = object->offset;
}

void cg_index(struct item_t *item, struct item_t *indexItem) {
	if(indexItem->mode == ITEM_MODE_CONST) {
		cg_load(item);
		item->mode = ITEM_MODE_REF;
		item->offset = indexItem->value * 4;
	} else {
		cg_load(indexItem);
		put(MULI, indexItem->reg, indexItem->reg, 4);
		cg_load(item);
		item->mode = ITEM_MODE_REF;
		put(ADD, item->reg, item->reg, indexItem->reg);	
		releaseReg(indexItem->reg);
	}
	item->type = item->type->base;
}

void cg_assignment(struct item_t *leftItem, struct item_t *rightItem) {
	printf("----- lt: %i rt: %i", leftItem->type->form, rightItem->type->form);
	if(leftItem->type->form != rightItem->type->form) { printError("type mismatch in assignment"); }
	cg_load(rightItem);
	put(STW, rightItem->reg, leftItem->reg, leftItem->offset);
	if(leftItem->mode == ITEM_MODE_REF) { releaseReg(leftItem->reg); }
	releaseReg(rightItem->reg);
}

/*************************************************************
 * HELP METHODS
 ************************************************************/
void printError(char *msg) {
	printf("\t%s:%4i:%4i: ERROR: %s (found: %s/%i)\n", srcfile, symbol->lineNr, symbol->colNr, msg, symbol->valueStr, symbol->id);
}

void printItem(struct item_t *item) {
	printf(" [ITEM] m:%i t:%i r:%i o:%i v:%i\n", item->mode, item->type->form, item->reg, item->offset, item->value);
}

/*************************************************************
 * PARSING METHODS
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

/* "int" | "char" | "void" | identifier. */ 
int typeSpec(struct object_t *head) {
	struct object_t *ptr;
	if(symbol->id == TYPEDEF) { 
		if(hasMoreTokens() == 0) { return 0; }		
		getNextToken(); 
	}
	if(symbol->id == INT ) { return TYPE_FORM_INT; }
	if(symbol->id == CHAR) { return TYPE_FORM_CHAR; }		/* return value for symbol table */
	if(symbol->id == VOID) { return TYPE_FORM_VOID; }
	if(identifier()) {
		ptr = lookUp(globList, symbol->valueStr);
		if(ptr != 0 && ptr->type->form == TYPE_FORM_RECORD) { 
			return TYPE_FORM_RECORD;
		}
		ptr = lookUp(head, symbol->valueStr);
		if(ptr != 0 && ptr->type->form == TYPE_FORM_RECORD) { 
			return TYPE_FORM_RECORD;
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
	while(1)	 {
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
			if(symbol->id != ARROW && symbol->id != DOT) {
				return 1;
			}
		}
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
			if(symbol->id == STRUCT) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
			}
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

int selector(struct item_t *item){
	struct object_t *object;
	struct item_t * indexItem;

	while(symbol->id == ARROW || symbol->id == LSQBR) {
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
		} else if(symbol->id == LSQBR) {			
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			indexItem = malloc(sizeof(struct item_t));
			indexItem->type = malloc(sizeof(struct type_t));
			expression(indexItem);
			cg_index(item, indexItem);
			if(symbol->id == RSQBR) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
			} else { printError("']' missing"); }
		}
	}
}

/* identifier | number | procCall | reference | "(" arithExp ")" . */
int factor(struct item_t *item) {
printSymbol(" -- factor: ");
	int result;

	struct item_t *leftItem;
	struct item_t *rightItem;
	struct object_t *object;

	leftItem = malloc(sizeof(struct item_t));
	leftItem->type = malloc(sizeof(struct type_t));
	rightItem = malloc(sizeof(struct item_t));
	rightItem->type = malloc(sizeof(struct type_t));
	object = malloc(sizeof(struct object_t));

	if(number() || symbol->id == CHARACTER || symbol->id == STRING) {
		if(number()) {
			item->mode = ITEM_MODE_CONST;
			item->type->form = TYPE_FORM_INT;
			item->value = symbol->digitValue;
		} else if(symbol->id == STRING) {
			/*
			storeString(item, symbol->valueStr);
			*/
		} else { /* if symbol->id == CHARACTER */
			item->mode = ITEM_MODE_CONST;
			item->type->form = TYPE_FORM_CHAR;
			item->value = symbol->valueStr[0];
		}
		
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		/*		
		if(op()) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			return expression(item);
		}
		*/
		return 1;
	}
	else if(symbol->id == LPAR) { 
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(expression(item)) {
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
			return expression(item); 
		}
		return 1;
	}
	else if(identifier()) {
		object = lookUp(locList, symbol->valueStr);
		if(object == 0) {
			object = lookUp(globList, symbol->valueStr);
			if(object == 0) {
				printError("unknown identifier");
				return 0;
			}
		}
		printf(">>>>>>>>>>>>>>>>>> "); printObject(object);
		leftItem->mode = ITEM_MODE_VAR;
		leftItem->type->form = object->type->form;
		printItem(leftItem);

		if(object->scope == GLOBAL_SCOPE) { leftItem->reg = GP; } 
		else /*if(opject->scope == LOCAL_SCOPE)*/ { leftItem->reg = LP; }
		leftItem->offset = object->offset;
		selector(item);

		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		/* var = */		
		if(symbol->id == EQSIGN) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			result = expression(rightItem);
			cg_assignment(leftItem, rightItem);
			return result;
		}

		/* var +  */
		else if(op()) {
			copyItem(item,leftItem);
			return 1;
/*
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			return expression(item);
*/
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
					return expression(item);
				}
				return 1;
			} else {
				printError("')' missing.");
			}
			return 0;
		}

		/*** replaced by selector ***/
		/* var -> | var . 
		else if(reference()) {			
			if(symbol->id == EQSIGN || op()) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				return expression();
			}
			return 1; 
		}
		*/
		/* var [
		else if(symbol->id == LSQBR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(number() || identifier()) {	
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				if(symbol->id == RSQBR) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					/* var [1] = | var [1] + /
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
		*/

		/* var; */		
		if(symbol->id == SEMCOL) { return 1; }

		/* var,  paramList*/		
		if(symbol->id == COMMA) { return 1; }
	}
	return 0;
}

/* factor { ( "*" | "/" ) factor } . */
int term(struct item_t *item) {
	struct item_t *leftItem;
	struct item_t *rightItem;
	int op; 

	op = OP_NONE;
	leftItem = 0;
	rightItem = 0;
printf("------------------------------\n");
	while(1) {
		if(factor(item)) {
	printf("l:%d, r:%d, op:%d\n", leftItem, rightItem, op);
			if(leftItem == 0){
				leftItem = malloc(sizeof(struct item_t));
				leftItem->type = malloc(sizeof(struct type_t));
				copyItem(leftItem, item);
			} 
			else if(rightItem == 0) { 
				rightItem = malloc(sizeof(struct item_t));
				rightItem->type = malloc(sizeof(struct type_t));
				copyItem(rightItem, item);
			} 
			if(leftItem != 0 && rightItem != 0) {
				if(op != OP_NONE) {
					cg_calcArithExp(leftItem, rightItem, op);
					printf(" TERM: "); printItem(leftItem);
					rightItem = 0;
					op = OP_NONE;
				} else { printError("[term] missing operator"); }
			}
		} else{ return 0; }
		printSymbol(" TERM: ");
		if(symbol->id == PLUS || symbol->id == MINUS || boolOp() || symbol->id == RPAR ||  
				symbol->id == SEMCOL || symbol->id == COMMA) {
			copyItem(item, leftItem);
			printf("END\n");
			return 1;
		}
		if(symbol->id == TIMES || symbol->id == DIV) {
			if(symbol->id == TIMES) { op = OP_MUL; }
			if(symbol->id == DIV) { op = OP_DIV; }
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
		} else {
			printError("'*', '/' or ';' expected.");
			return 0;
		}
	}
}

/* [ "-" ] term { ( "+" | "-" ) term } . */
int arithExp(struct item_t *item) {
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
	while(1) {
		if(term(item)) {
	printf(" ARITH_EXP l:%d, r:%d, op:%d\n", leftItem, rightItem, op);
			printf(" ARITH_EXP "); printItem(item);
			if(leftItem == 0){
				leftItem = malloc(sizeof(struct item_t));
				leftItem->type = malloc(sizeof(struct type_t));
				copyItem(leftItem, item);
				leftItem->value = leftItem->value * minusFlag;
			} else if(rightItem == 0) {
				rightItem = malloc(sizeof(struct item_t));
				rightItem->type = malloc(sizeof(struct type_t));
				copyItem(rightItem, item);
			} 

			if(leftItem != 0 && rightItem != 0) {
				if(op != OP_NONE) {
					cg_calcArithExp(leftItem, rightItem, op);
					rightItem = 0;
					op = OP_NONE;
				} else {
					printError("[arithExp] missing operator");
				}
			}
		} else { return 0; }

		if(boolOp() || symbol->id == RPAR || symbol->id == SEMCOL || hasMoreTokens() == 0 || symbol->id == COMMA) {
			copyItem(item, leftItem);
			return 1;
		}
		if(symbol->id == PLUS || symbol->id == MINUS) {
			if(symbol->id == PLUS) { op = OP_ADD; }
			if(symbol->id == MINUS) { op = OP_SUB; }
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
		} else {
			printError("'+', '-' or ';' expected..");
			return 0;
		}
	}
}

/* arithExp { boolOp arithExp } . */
int expression(struct item_t *item) {
	while(1) {
		if(arithExp(item) == 0) { return 0; }
		/*TODO*/
		if(symbol->id == RPAR || symbol->id == SEMCOL || hasMoreTokens() == 0 || symbol->id == COMMA) {
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
	int i;
	struct item_t *item;
	item = malloc(sizeof(struct item_t));
	item->type = malloc(sizeof(struct type_t));

	if(symbol->id == RPAR) { return 1; }
	while(1) {
			if(expression(item) == 0) {
				printError("paramList: identifier expected.");
				return 0;
			}

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

/* ifCmd = "if" "(" expression ")" block [elseCmd] . */	
int ifCmd() {
	struct item_t *item;
	item = malloc(sizeof(struct item_t));
	item->type = malloc(sizeof(struct type_t));
	if(symbol->id == IF) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol->id == LPAR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(expression(item)) {
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
	struct item_t *item;
	item = malloc(sizeof(struct item_t));
	item->type = malloc(sizeof(struct type_t));
	if(symbol->id == WHILE) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol->id == LPAR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(expression(item)) {
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
	struct item_t *item;
	item = malloc(sizeof(struct item_t));
	item->type = malloc(sizeof(struct type_t));
	if(symbol->id == RETURN) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(expression(item)) { 
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
		object->class = OBJECT_CLASS_VAR;

		if(symbol->id == RPAR) { return 1; }
		if(symbol->id == STRUCT) { 
			if(hasMoreTokens() == 0) { return 0; }		
			getNextToken();
		}
		type = newType(typeSpec(head));
		object->offset = paramOffset;
		if(type->form == 0) {
			return 1;
		} else if(type->form == TYPE_FORM_RECORD) {
			ptr = lookUp(head, symbol->valueStr);
			if(ptr == 0) {
				ptr = lookUp(globList, symbol->valueStr);
				if(ptr != 0) {
					object->type = ptr->type;
				}
			} else {
				object->type = ptr->type;
				paramOffset = paramOffset + ptr->offset; /* add size of struct */
			}
		} else { paramOffset = paramOffset + 4; } /* add size of datatype */

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
				if(type->form != TYPE_FORM_RECORD) {
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
				object->scope = LOCAL_SCOPE;
				if(type->form != TYPE_FORM_RECORD) {
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
			object->class = OBJECT_CLASS_VAR;
		} else {
			object->class = OBJECT_CLASS_FIELD;
		}
		typedefDec(head);
		if(symbol->id == STRUCT) { 
			if(hasMoreTokens() == 0) { return 0; }		
			getNextToken();
		}
		type = newType(typeSpec(head));
		object->offset = locOffset;
		if(type->form == 0) {
			return 1;
		} else if(type->form == TYPE_FORM_RECORD) {
			ptr = lookUp(head, symbol->valueStr);
			if(ptr == 0) {
				ptr = lookUp(globList, symbol->valueStr);
				if(ptr != 0) {
					object->type = ptr->type;
				}
			} else {
				object->type = ptr->type;
				locOffset = locOffset - ptr->offset; /* add size of struct */
			}
		} else { locOffset = locOffset - 4; } /* add size of datatype */

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
				object->scope = LOCAL_SCOPE;
				if(type->form != TYPE_FORM_RECORD) {
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
	object->class = OBJECT_CLASS_TYPE;

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
		} else if(type->form == TYPE_FORM_RECORD) {
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
				if(type->form != TYPE_FORM_RECORD) {
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
	struct object_t *count;
	struct object_t *ptr;
	struct object_t *ptr1;
	int i;

	if(symbol->id == STRUCT) {
		fieldObj = malloc(sizeof(struct object_t));
		object = malloc(sizeof(struct object_t));
		object->class = OBJECT_CLASS_TYPE;
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

				/* count fields */
				i = 1;
				count = fieldObj;
				while(count != 0) {
					count = count->next;
				}
				object->offset = globOffset;
				globOffset = globOffset + (i * 4 * (-1));

				if(symbol->id == RCUBR) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
				} else { printError("'}' missing."); }
			}
			if(symbol->id == SEMCOL) {
				record->fields = fieldObj;
				object->type = record;
				if(globList->name != 0 && lookUp(globList, object->name) != 0)	{ /* delete implicite struct declaration */
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
		while(typeSpec(globList) == 0 && symbol->id != STRUCT && hasMoreTokens()) {
			printError("globalDec: typeSpec (char, int or void) expected.");
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
		}
		if(symbol->id == STRUCT) { 
			if(hasMoreTokens() == 0) { return 1; }		
			getNextToken();
		}
		type = newType(typeSpec(globList));
		object->offset = globOffset;
		if(type->form == 0) {
			return 0;
		} else if(type->form == TYPE_FORM_RECORD) {
			ptr = lookUp(globList, symbol->valueStr);
			if(ptr != 0) {
				object->type = ptr->type;
				globOffset = globOffset - ptr->offset; /* add size of struct */
			}
		} else { globOffset = globOffset - 4; } /* add size of datatype */
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
			locList = 0;
			locList = malloc(sizeof(struct object_t));
			locOffset = 0;
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

						if(statementSeq()) {
							if(symbol->id == RCUBR) { return 1; }
							else { printError("'}' missing."); } 
						}						
					}
				}
			} 
			/* declaration */
			else if(symbol->id == SEMCOL) {
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
		printSymbol(" -3- globaldec: ");
			}
		}
	}
}

int statementSeq () {
	struct item_t *item;
	item = malloc(sizeof(struct item_t));
	item->type = malloc(sizeof(struct type_t));
	if(symbol->id == RCUBR) { return 1; }
	while(1) {
		while(identifier() == 0 && number() == 0 && symbol->id != WHILE && symbol->id != IF 
				&& symbol->id != RETURN && symbol->id != LPAR && symbol->id != ELSE) {
			if(hasMoreTokens() == 0) { return 0; }			
			getNextToken();
			printError("statSeq(1): identifier, number, while, if or return expected.");
		}
		if(expression(item) || ret()) {
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

	initRegs();
	initItemModes();
	initOperators();	
	initSymbolTable();
	initOutputFile();
	initTMCmd();

	printf("\nstart parsing %s...\n", srcfile);
	while ( hasMoreTokens() ) {
		getNextToken();
		i = programm();
	}
	printTable(globList);
	put(TRAP,0,0,0);
	close(out_fp_bin); 
	close(out_fp_ass); 
	printf("\n -- DONE. --\n\n");
	return i;

}
