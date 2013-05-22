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
	/* meta data - dummy values */
printf("CS: %i \n",CS);
	put(CS,	0, 0, 0);
	put(GP,	0, 0, 0);
	put(SP,	0, 0, 0);
}

void finalizeOutputFile() {
	close(out_fp_bin);
	out_fp_bin = open(outfile, 65, 448);
	put(CS,	0, 0, nrOfCmds);
	put(GP,	0, 0, nrOfGVar*4);	/* !!! da globale variablen NICHT initialisiert werden können, */
								/* !!! sind immer nur 4 byte speicher nötig */
	put(SP,	0, 0, nrOfStrs);	/*TODO potentielles problem !!!*/
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
/*
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
*/


	buff = malloc(1*32);
	buff[0] = encode(op,a,b,c);	

//	printf("cmd: %s %i %i %i encode: %i\n",getCmdName(op),a,b,c,buff[0]);

	int *ir = malloc(sizeof(int) * 4);
	ir[0] = (buff[0] >> 26) & 63; 	// 0x3F: 6 lsbs
	ir[1] = (buff[0] >> 21) & 31; 	// 0x1F: 5 lsbs
	ir[2] = (buff[0] >> 16) & 31;	// 0x1F: 5 lsbs
	ir[3] = buff[0] & 65535; 		// 0xFFFF: 16 lsbs
	if (ir[3] >= 32768)
	ir[3] = ir[3] - 65536; 				// 0x10000: 2^16
//	printf("cmd: %i\n", buff[0]);
//	printf("DEBUG %i op: %i a: %i b: %i c: %i\n",buff[0],ir[0],ir[1],ir[2],ir[3]);



	wb = write(out_fp_bin, buff, 4);
//    if ( wb != 4 ) { printf(" --- could only write %i byte.\n", wb); }
	nrOfCmds = nrOfCmds + 1;
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
	GPTR = 28;
	LPTR = 29;
	HPTR = 30;
	regs = malloc( nrOfRegs*sizeof(int) );
	while (i < nrOfRegs) { regs[i] = 0; i = i + 1; }
}
int requestReg() {
	int i; i = 1;
	/* register for calculation: 1-27 */
	while (i < 28) { 
		if (regs[i] == 0) { regs[i] = 1; return i; }
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
	printf("L:"); printItem(leftItem);
	printf("R:"); printItem(rightItem);
	printf("OP: %i\n",op);

	if(leftItem->type->form == TYPE_FORM_INT && rightItem->type->form == TYPE_FORM_INT) {
		if(rightItem->mode == ITEM_MODE_CONST) {
			if(leftItem->mode == ITEM_MODE_CONST) {
					 if(op == OP_ADD) { leftItem->value = leftItem->value + rightItem->value; } 
				else if(op == OP_SUB) { leftItem->value = leftItem->value - rightItem->value; } 
				else if(op == OP_MUL) { leftItem->value = leftItem->value * rightItem->value; } 
				else if(op == OP_DIV) { leftItem->value = leftItem->value / rightItem->value; } 
				else { printError("nich so gut..."); }
			} else {
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
			printf("LL:");printItem(leftItem);
			printf("LR:");printItem(rightItem);
				 if(op == OP_ADD) { put(ADD, leftItem->reg, leftItem->reg, rightItem->reg); } 
			else if(op == OP_SUB) { put(SUB, leftItem->reg, leftItem->reg, rightItem->reg); } 
			else if(op == OP_MUL) { put(MUL, leftItem->reg, leftItem->reg, rightItem->reg); } 
			else if(op == OP_DIV) { put(DIV, leftItem->reg, leftItem->reg, rightItem->reg); } 
			else { printError("nich so gut..."); }
			releaseReg(rightItem->reg);
		}
	} else { printError("Type mismatch (int expected)."); }
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
		item->offset = indexItem->value * (-4);
	} else {
		cg_load(indexItem);
		put(MULI, indexItem->reg, indexItem->reg, -4);
		cg_load(item);
		item->mode = ITEM_MODE_REF;
		put(ADD, item->reg, item->reg, indexItem->reg);	
		releaseReg(indexItem->reg);
	}
	item->type = item->type->base;
}

void cg_assignment(struct item_t *leftItem, struct item_t *rightItem) {
	if(leftItem->type->form != rightItem->type->form && rightItem->type->form != TYPE_FORM_VOID) { 
		printError("[assignment] Type mismatch in assignment"); 
	}
	cg_load(rightItem);
	put(STW, rightItem->reg, leftItem->reg, leftItem->offset);
	if(leftItem->mode == ITEM_MODE_REF) { releaseReg(leftItem->reg); }
	releaseReg(rightItem->reg);
}

void cg_allocate(struct item_t *item) {
	int tempHeapOffset;

	tempHeapOffset = heapOffset;
	heapOffset = heapOffset + item->value;

	cg_load(item);
	item->type->form = TYPE_FORM_VOID;
	item->type->fields = 0;
	put(ADDI, item->reg, HPTR, heapOffset);
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
int typeSpec(struct item_t *item, struct object_t *head) {
	struct object_t *ptr;

	item->mode = ITEM_MODE_CONST;
	item->type->size = 4;
	item->value = 4;

	if(symbol->id == TYPEDEF) { 
		if(hasMoreTokens() == 0) { return 0; }		
		getNextToken(); 
	}
	if(symbol->id == INT ) { item->type->form = TYPE_FORM_INT; return TYPE_FORM_INT; }
	if(symbol->id == CHAR) { item->type->form = TYPE_FORM_CHAR; return TYPE_FORM_CHAR; } /* return value for symbol table */
	if(symbol->id == VOID) { item->type->form = TYPE_FORM_VOID; return TYPE_FORM_VOID; }
	if(identifier()) {
		ptr = lookUp(globList, symbol->valueStr);
		if(ptr != 0 && ptr->class == OBJECT_CLASS_TYPE) {
			item->type->form = ptr->type->form;
			item->type->size = ptr->type->size;
			item->value = ptr->type->size;
			return TYPE_FORM_RECORD;
		}
		ptr = lookUp(head, symbol->valueStr);
		if(ptr != 0 && ptr->class == OBJECT_CLASS_TYPE) { 
			item->type->form = ptr->type->form;
			item->type->size = ptr->type->size;
			item->value = ptr->type->size;
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
int sizeOf(struct item_t *item, struct object_t *head) {
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
			if(typeSpec(item, head)) {
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
				printError("[selector] identifier expected");
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
	return 1;
}

/* identifier | number | procCall | reference | "(" arithExp ")" . */
int factor(struct item_t *item) {
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


		printf("NUMBER: %i",symbol->digitValue);printItem(item);
		
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();

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
	else if(sizeOf(item, globList)) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		return 1;
	} 
	else if(symbol->id == MALLOC) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		result = expression(item);
		cg_allocate(item);
		return result;
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
		leftItem->mode = ITEM_MODE_VAR;
		leftItem->type = object->type;

		if(object->scope == GLOBAL_SCOPE) { leftItem->reg = GPTR; } 
		else { leftItem->reg = LPTR; }

		leftItem->offset = object->offset;

		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();

		printSymbol("XXXXXXXXXX: ");
//		printSymbol("IDENT: ");printItem(leftItem);

		selector(leftItem);

		/* var = */		
		if(symbol->id == EQSIGN) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
		printSymbol("XXXXXXXXX1: ");
			result = expression(rightItem);
			cg_assignment(leftItem, rightItem);
			copyItem(item, leftItem);
			return result;
		}

		/* var +  */
		else if(op()) { copyItem(item,leftItem); return 1; }		
		/* var ) */
		else if(symbol->id == RPAR) { copyItem(item,leftItem); return 1; }
		/* var ( */
		else if(symbol->id == LPAR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			paramList();
			if(symbol->id == RPAR) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
/*
				if(op()) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					return expression(item);
				}
*/
				copyItem(item,leftItem);
				return 1;
			} else {
				printError("')' missing.");
			}
			return 0;
		}

		/* var; */		
		if(symbol->id == SEMCOL) { copyItem(item, leftItem); return 1; }

		/* var,  paramList*/		
		if(symbol->id == COMMA) { copyItem(item, leftItem); return 1; }
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
	while(1) {
printf(" -- term beginn\n");
		if(factor(item)) {
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
printf("op: %i \n", op);
				if(op != OP_NONE) {
					cg_calcArithExp(leftItem, rightItem, op);
					rightItem = 0;
					op = OP_NONE;
				} else { printError("[term] missing operator"); }
			}
		} else{ return 0; }
		if(symbol->id == PLUS || symbol->id == MINUS || boolOp() || symbol->id == RPAR ||  
				symbol->id == SEMCOL || symbol->id == COMMA || symbol->id == RSQBR) {
			copyItem(item, leftItem);
			return 1;
		}
printSymbol("TERM SYM");
		if(symbol->id == TIMES || symbol->id == DIV) {
printf("TERM OP: %i\n", symbol->id);
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

		if(boolOp() || symbol->id == RPAR || symbol->id == SEMCOL || hasMoreTokens() == 0 || symbol->id == COMMA  || symbol->id == RSQBR) {
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
		if(symbol->id == RPAR || symbol->id == SEMCOL || hasMoreTokens() == 0 || symbol->id == COMMA || symbol->id == RSQBR) {
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
	struct item_t *item;

	item = malloc(sizeof(struct item_t));
	item->type = malloc(sizeof(struct type_t));

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
		type = newType(typeSpec(item, head));
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
	struct item_t *item;

	item = malloc(sizeof(struct item_t));
	item->type = malloc(sizeof(struct type_t));

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
		type = newType(typeSpec(item, head));
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
printSymbol("dec: ");
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
		} else { printError("[declaration] identifier expected."); }
	}
}

/* "typedef" ("struct" ident | typeSpec) ["*"] ident */
int typedefDec(struct object_t *head) {
	struct object_t *object;
	struct object_t *ptr;
	struct type_t *type;
	struct type_t *array;
	struct item_t *item;
	type = 0;
	array = 0;
	ptr = 0;
	object = malloc(sizeof(struct object_t));
	object->class = OBJECT_CLASS_TYPE;
	item = malloc(sizeof(struct item_t));
	item->type = malloc(sizeof(struct type_t));

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
		object->type = malloc(sizeof(struct type_t));
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
				if(globList != 0 && globList->name != 0 && lookUp(globList, object->name) != 0)	{ 
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
				object->offset = globOffset;
				globOffset = globOffset - 4;
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
	struct item_t *item;

	item = malloc(sizeof(struct item_t));
	item->type = malloc(sizeof(struct type_t));

	while(1) { 	
		type = 0;
		array = 0;
		ptr = 0;
		object = malloc(sizeof(struct object_t));
		while(typedefDec(globList)) {}
		while(typeSpec(item, globList) == 0 && symbol->id != STRUCT && hasMoreTokens()) {
			printError("globalDec: typeSpec (char, int or void) expected.");
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
		} else if(type->form == TYPE_FORM_RECORD) {
			ptr = lookUp(globList, symbol->valueStr);
			if(ptr != 0) {
				object->type = ptr->type;
				globOffset = globOffset - 4;	/* add size of struct */
			} else { printError("[global dec] Type not found."); }
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

	nrOfGVar = 0;
	nrOfStrs = 0;
	nrOfCmds = 0;	

	initTMCmd();
	initRegs();
	initItemModes();
	initOperators();	
	initSymbolTable();
	initOutputFile();

	printf("\nstart parsing %s...\n", srcfile);
	while ( hasMoreTokens() ) {
		getNextToken();
		i = programm();
	}
	printTable(globList);
	put(TRAP,0,0,0);
	finalizeOutputFile();
	close(out_fp_bin); 
	close(out_fp_ass); 
	printf("\n -- DONE. --\n\n");
	return i;

}
