/*
 * PSEUDOCODE - parser.c
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */
#include "parser.h"
#include "scanner.h"
#include "tokenMapping.h"
#include "./tm/tmCmd.h"

/*************************************************************
 * CODE GENERATION METHODS
 ************************************************************/
void initOutputFile() {
	out_cmd_buff = malloc(sizeof(struct cmd_t *) * 1024);
}

void finalizeOutputFile() {
	int wb;
	int i;
	int out_fp_bin;
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
	tempBuff[0] = cg_encode(CMD_SP,	0, 0, nrOfStrs-1);	
	write(out_fp_bin, tempBuff, 4); 
	if(mainPos != -1) {
		tempBuff[0] = cg_encode(CMD_BR,	0, 0, mainPos);	
		write(out_fp_bin, tempBuff, 4); 
	} else { printError("missing main method"); }
	i = 1;
printf("\n\n\n");
	while(i < PC) {
printf("%s %i %i %i\n", getCmdName(out_cmd_buff[i]->op), out_cmd_buff[i]->a, out_cmd_buff[i]->b, out_cmd_buff[i]->c);
		tempBuff[0] = cg_encode(out_cmd_buff[i]->op, out_cmd_buff[i]->a, out_cmd_buff[i]->b, out_cmd_buff[i]->c);
		wb = write(out_fp_bin, tempBuff, 4); 
    		if ( wb != 4 ) { printf(" --- could only write %i byte.\n", wb); }
		i = i + 1;
	}
printf("\n\n\n");
	close(out_fp_bin);
}

int cg_encode(int op, int a, int b, int c) {
	if (c < 0)
	c = c + 65536; // 0x10000: 2^16
	return (((((op * 32) + a) * 32) + b) * 65536) + c;
}

void cg_put(int op, int a, int b, int c) {
	out_cmd_buff[PC] = malloc(sizeof(struct cmd_t*));
	out_cmd_buff[PC]->op = op;
	out_cmd_buff[PC]->a = a;
	out_cmd_buff[PC]->b = b;
	out_cmd_buff[PC]->c = c;
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
	else if(item->mode == ITEM_MODE_VAR) { cg_var2Reg(item); }
	else if(item->mode == ITEM_MODE_REF) { cg_ref2Reg(item); }
}

void cg_simpleExpOR(struct item_t* item) { 
	if (item->type->form == TYPE_FORM_BOOL) {
		cg_loadBool(item);
		cg_put(cg_branch(item->op), item->reg, 0, item->tru);
		cg_releaseReg(item->reg);
		item->tru = PC - 1;
		cg_fixLink(item->fls);
		item->fls = 0;
	} else { printError("[simpleExpOR] boolean expression expected"); }
}

void cg_termOpAND(struct item_t* item) { 
	if (item->type->form == TYPE_FORM_BOOL) {
		cg_loadBool(item);
		cg_put(cg_branch(cg_negateOperator(item->op)), item->reg, 0, item->fls);
		cg_releaseReg(item->reg);
		item->fls = PC - 1;
		cg_fixLink(item->tru);
		item->tru = 0;
	} else { printError("[termOpAND] boolean expression expected"); }
}

void cg_simpleExpBinOp(struct item_t *leftItem, struct item_t *rightItem, int op) {
	if(op == OP_OR) {
		if ((leftItem->type->form == TYPE_FORM_BOOL) && (rightItem->type->form == TYPE_FORM_BOOL)) { 
			cg_loadBool(rightItem);
			leftItem->reg = rightItem->reg; 
			leftItem->fls = rightItem->fls;
			leftItem->tru = cg_concatenate(rightItem->tru, leftItem->tru);
			leftItem->op = rightItem->op;
		} else { printError("[simpleExpBinOp] boolean expressions expected"); }
	}
	else if((leftItem->type->form == TYPE_FORM_INT  && rightItem->type->form == TYPE_FORM_INT) || 
			(leftItem->type->form == TYPE_FORM_CHAR && rightItem->type->form == TYPE_FORM_CHAR)) {
		if(rightItem->mode == ITEM_MODE_CONST) {
			if(leftItem->mode == ITEM_MODE_CONST) {
					 if(op == OP_ADD) { leftItem->value = leftItem->value + rightItem->value; } 
				else if(op == OP_SUB) { leftItem->value = leftItem->value - rightItem->value; } 
				else { printError("nich so gut..."); }
			} else {
				cg_load(leftItem);
					 if(op == OP_ADD) { cg_put(CMD_ADDI, leftItem->reg, leftItem->reg, rightItem->value); } 
				else if(op == OP_SUB) { cg_put(CMD_SUBI, leftItem->reg, leftItem->reg, rightItem->value); } 
				else { printError("nich so gut..."); }
			}
		} else {
			cg_load(leftItem);
			cg_load(rightItem);
				 if(op == OP_ADD) { cg_put(CMD_ADD, leftItem->reg, leftItem->reg, rightItem->reg); } 
			else if(op == OP_SUB) { cg_put(CMD_SUB, leftItem->reg, leftItem->reg, rightItem->reg); } 
			else { printError("nich so gut..."); }
			cg_releaseReg(rightItem->reg);
		}
	} else { printError("Type mismatch (int expected)."); }
}

void cg_termOperator(struct item_t *leftItem, struct item_t *rightItem, int op) {
	if(op == OP_AND) {
		if ((leftItem->type->form == TYPE_FORM_BOOL) && (rightItem->type->form == TYPE_FORM_BOOL)) {  
			cg_loadBool(rightItem);
			leftItem->reg = rightItem->reg;
			leftItem->fls = cg_concatenate(rightItem->fls, leftItem->fls);
			leftItem->tru = rightItem->tru;
			leftItem->op = rightItem->op;
		} else { printError("[termOperator] boolean expressions expected"); }
	}	
	else if((leftItem->type->form == TYPE_FORM_INT  && rightItem->type->form == TYPE_FORM_INT) || 
			(leftItem->type->form == TYPE_FORM_CHAR && rightItem->type->form == TYPE_FORM_CHAR)) {
		if(rightItem->mode == ITEM_MODE_CONST) {
			if(leftItem->mode == ITEM_MODE_CONST) {
				if(op == OP_MUL) { leftItem->value = leftItem->value * rightItem->value; } 
				else if(op == OP_DIV) { leftItem->value = leftItem->value / rightItem->value; } 
				else { printError("nich so gut..."); }
			} else {
				cg_load(leftItem);
				if(op == OP_MUL) { cg_put(CMD_MULI, leftItem->reg, leftItem->reg, rightItem->value); } 
				else if(op == OP_DIV) { cg_put(CMD_DIVI, leftItem->reg, leftItem->reg, rightItem->value); } 
				else { printError("nich so gut..."); }
			}
		} else {
			cg_load(leftItem);
			cg_load(rightItem);
			if(op == OP_MUL) { cg_put(CMD_MUL, leftItem->reg, leftItem->reg, rightItem->reg); } 
			else if(op == OP_DIV) { cg_put(CMD_DIV, leftItem->reg, leftItem->reg, rightItem->reg); } 
			else { printError("nich so gut..."); }
			cg_releaseReg(rightItem->reg);
		}
	} else { printError("Type mismatch (int expected)."); }
}

void cg_expressionOperator(struct item_t *leftItem, struct item_t *rightItem, int op) {
	if ( ((leftItem->type->form == TYPE_FORM_INT)  || (leftItem->type->form == TYPE_FORM_CHAR)) 
	  && ((rightItem->type->form == TYPE_FORM_INT) || (rightItem->type->form == TYPE_FORM_CHAR)) ) {
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
	} else { printError("integer expressions expected"); }
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
	if(leftItem->type->form == TYPE_FORM_ARRAY) {	
		if(leftItem->type->base->form != rightItem->type->form && rightItem->type->form != TYPE_FORM_VOID) { 
			printError("[assignment] Type mismatch in assignment"); 
		}
	} else {
		if(rightItem->type->form == TYPE_FORM_ARRAY) {	
				if(rightItem->type->base->form != leftItem->type->form && rightItem->type->form != TYPE_FORM_VOID) { 
					printError("[assignment] Type mismatch in assignment"); 
				}
		} else {
			if(leftItem->type->form != rightItem->type->form && rightItem->type->form != TYPE_FORM_VOID) { 
				printError("[assignment] Type mismatch in assignment"); 
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
	cg_put(CMD_MAL,HPTR,0,item->reg);
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
	if(op == OP_LET)	 { return OP_GT;  }
	if(op == OP_GET)	 { return OP_LT;  }
	if(op == OP_EQ)	 { return OP_NEQ; }
	if(op == OP_NEQ)	 { return OP_EQ;  }
	return -1;
}

int cg_branch(int op) {
	if(op == OP_LT)	 { return CMD_BLT; }
	if(op == OP_GT)	 { return CMD_BGT; }
	if(op == OP_LET)	 { return CMD_BLE; }
	if(op == OP_GET)	 { return CMD_BGE; }
	if(op == OP_EQ)	 { return CMD_BEQ; }
	if(op == OP_NEQ) { return CMD_BNE; }
	return -1;
}

void cg_encodeC(int branchAddress, int newC) {
	out_cmd_buff[branchAddress]->c = newC;
}

int cg_decodeC(int branchAddress) {
	return out_cmd_buff[branchAddress]->c;
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
void printError(char *msg) {
	errorCounter = errorCounter + 1;
	printf("\t%s:%4i:%4i: ERROR: %s (found: %s/%i)\n", srcfile, symbol->lineNr, symbol->colNr, msg, symbol->valueStr, symbol->id);
}

void printItem(struct item_t *item) {
	printf(" [ITEM] m:%i t:%i r:%i o:%i v:%i op:%i f:%i t:%i\n", item->mode, item->type->form, item->reg, item->offset, item->value, item->op, item->fls, item->tru);
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
int compOp() {
	if( symbol->id == EQ || symbol->id == NEQ || symbol->id == LT || symbol->id == GT || 
		symbol->id == LET || symbol->id == GET) {
		return 1;
	}
	return 0;
}

int op() {
	if(symbol->id == PLUS || symbol->id == MINUS || symbol->id == DIV || symbol->id == TIMES || compOp() || symbol->id == AND || symbol->id == OR) {
		return 1;
	}
	return 0;
}

/* letter {letter | digit} . */
int identifier() {
	if(symbol->id == IDENT) { return 1; }
	return 0;
}

/* "int" | "char" | "void" | "bool" | identifier. */ 
int typeSpec(struct item_t *item, struct object_t *head) {
	struct object_t *ptr;

	item->mode = ITEM_MODE_CONST;
	item->type->size = 4;
	item->value = 4;

	if(symbol->id == TYPEDEF) { 
		if(hasMoreTokens() == 0) { return 0; }		
		getNextToken(); 
	}
	if(symbol->id == INT ) { item->type->form = TYPE_FORM_INT;  return TYPE_FORM_INT;  }
	if(symbol->id == CHAR) { item->type->form = TYPE_FORM_CHAR; return TYPE_FORM_CHAR; } /* return value for symbol table */
	if(symbol->id == VOID) { item->type->form = TYPE_FORM_VOID; return TYPE_FORM_VOID; }
	if(symbol->id == BOOL) { item->type->form = TYPE_FORM_BOOL; return TYPE_FORM_BOOL; }
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
		} else if(head->scope != 0 && head->class != 0 && head->type != 0) {	// first element in list
			//TODO printError("unknown type.");
		}
	}
	return 0;
}

/* include . */
int include() {
	if(symbol->id == INCLUDE) { return 1; }
	return 0;
}

int storeString(struct item_t *item) {
	int i; int reg; int r;
	i = 0;

	r = cg_requestReg();
	cg_put( CMD_ADDI, r, STRPTR, (nrOfStrs * (-1)) );

	item->mode = ITEM_MODE_REG;
	item->offset = 0;
	item->reg = r;
	if(item->type == 0) { item->type = malloc(sizeof(struct type_t)); }
	item->type->form = TYPE_FORM_VOID;
	item->type->fields = 0;

	reg = cg_requestReg();
	while(i < symbol->digitValue) {
		cg_put( CMD_ADDI, reg, 0, symbol->valueStr[i] );
		cg_put( CMD_STW, reg, STRPTR, ( (nrOfStrs + i) * 4 * (-1)) );
		i = i + 1;
	}
	cg_put( CMD_STW, 0, STRPTR, ( (nrOfStrs + i) * 4 * (-1)) );
	nrOfStrs = nrOfStrs + i + 1;
	cg_releaseReg(reg);

	return 1;
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

/* identifier | number | procCall | "(" arithExp ")" . */
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
			printError("[factor] expression expected.");
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
	else if(symbol->id == OPEN)  { fileOpen(item);  return 1; }
	else if(symbol->id == READ)  { fileRead(item);  return 1; }
	else if(symbol->id == WRITE) { fileWrite(item); return 1; }
	else if(symbol->id == CLOSE) { fileClose(item); return 1; } 
	else if(identifier()) {
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

		/* var = */		
		if(symbol->id == EQSIGN) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			result =  expression(rightItem);
			cg_assignment(leftItem, rightItem);
			copyItem(item, leftItem);
			return result;
		}
		/* var +  */
		else if(op()) { copyItem(item,leftItem); return 1; }		
		/* var ( */
		else if(symbol->id == LPAR) {
			procedureCall(leftItem, object->name);
			if(symbol->id == RPAR) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				copyItem(item, leftItem);
				return 1;
			}
		}

		if(symbol->id == SEMCOL || symbol->id == COMMA || symbol->id == RSQBR || symbol->id == ARROW || symbol->id == RPAR) 
		{ copyItem(item, leftItem); return 1; }
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
				if(op != OP_NONE) {
					cg_termOperator(leftItem, rightItem, op);
					rightItem = 0;
					op = OP_NONE;
				} else { printError("[term] missing operator"); }
			}
		} else{ return 0; }

		if(symbol->id == PLUS || symbol->id == MINUS || symbol->id == OR || 
				compOp() || symbol->id == RPAR || symbol->id == SEMCOL || symbol->id == COMMA || 
				symbol->id == RSQBR || symbol->id == RCUBR || symbol->id == ARROW) {
			copyItem(item, leftItem);
			return 1;
		}
		if(symbol->id == TIMES || symbol->id == DIV || symbol->id == AND) {
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
					cg_simpleExpBinOp(leftItem, rightItem, op);
					rightItem = 0;
					op = OP_NONE;
				} else {
					printError("[arithExp] missing operator");
				}
			}
		} else { return 0; }

		if(compOp() || symbol->id == RPAR || symbol->id == SEMCOL || hasMoreTokens() == 0 || 
			symbol->id == COMMA  || symbol->id == RSQBR || symbol->id == RCUBR || symbol->id == ARROW) {
			copyItem(item, leftItem);
			return 1;
		}
		if(symbol->id == PLUS || symbol->id == MINUS || symbol->id == OR) {
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
int expression(struct item_t *item) {
	struct item_t *leftItem;
	struct item_t *rightItem;
	int op; 

	leftItem = 0;
	rightItem = 0;
	op = OP_NONE;

	while(1) {
		if(arithExp(item)) {
			if(leftItem == 0){
				leftItem = malloc(sizeof(struct item_t));
				leftItem->type = malloc(sizeof(struct type_t));
				copyItem(leftItem, item);
			} else if(rightItem == 0) {
				rightItem = malloc(sizeof(struct item_t));
				rightItem->type = malloc(sizeof(struct type_t));
				copyItem(rightItem, item);
			} 

			if(leftItem != 0 && rightItem != 0) {
				if(op != OP_NONE) {
					cg_expressionOperator(leftItem, rightItem, op);
					rightItem = 0;
					op = OP_NONE;
				} else {
					printError("[expression] missing operator");
				}
			}
		} else { return 0; }
		if(symbol->id == RPAR || symbol->id == SEMCOL || hasMoreTokens() == 0 || 
			symbol->id == COMMA || symbol->id == RSQBR || symbol->id == RCUBR || symbol->id == ARROW) {
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

/* identifier {"," identifier} . */
int paramList() {
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

int printMethod(struct item_t *item) {
	int i;
	int reg;
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
						cg_put(CMD_ADDI, reg, 0, '\n');
						cg_put(CMD_PRC, reg, 0, 0);
						i = i + 1;
					} else {
						cg_put(CMD_ADDI, reg, 0, symbol->valueStr[i]);
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
					if(item->type->form == TYPE_FORM_CHAR) {
						cg_put(CMD_PRC, item->reg, 0, 0);
					} else {
						cg_put(CMD_PRN, item->reg, 0, 0);
					}
				} else { printError("[printMethod] expression expected."); }
			}
			if(symbol->id == RPAR) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				if(symbol->id == SEMCOL) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					return 1;
				} else { printError("';' missing."); }
			} else { printError("[printMethod] ')' missing."); }
		} else { printError("'(' missing."); }
	}
	return 0;
}

int fileOpen(struct item_t *item) {
	struct item_t *firstItem;
	struct item_t *secondItem;
	struct item_t *thirdItem;
	struct object_t *object;
	firstItem = malloc(sizeof(struct item_t));
	secondItem = malloc(sizeof(struct item_t));	
	thirdItem = malloc(sizeof(struct item_t));	
	object = malloc(sizeof(struct object_t));
	object->type = malloc(sizeof(struct type_t));
	
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
									} else { printError("[fileopen] ';' missing."); }							
								} else { printError("[fileopen] ')' missing."); }
							} else { printError("[fileopen] expression expected."); }
						} else { printError("[fileopen] ',' missing."); }
					} else { printError("[fileopen] expression expected."); }
				} else { printError("[fileopen] ',' missing."); }
			} else { printError("[fileopen] identifier expexcted."); }			
		} else { printError("[fileopen] '(' missing."); }
	}
	return 0;
}

int fileClose(struct item_t *item) {
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
					} else { printError("[fileclose] ';' missing."); }
				} else { printError("[fileclose] ')' missing."); }
			} else { printError("[fileclose] expression expected."); }
		} else { printError("[fileclose] '(' missing."); }
	} 
	return 0;
}

int fileWrite(struct item_t *item) {
	int sreg;
	struct item_t *firstItem;
	struct item_t *secondItem;
	struct item_t *thirdItem;
	struct object_t *object;

	firstItem = malloc(sizeof(struct item_t));
	secondItem = malloc(sizeof(struct item_t));	
	thirdItem = malloc(sizeof(struct item_t));	

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
									} else { printError("[filewrite] ';' missing."); }							
								} else { printError("[filewrite] ')' missing."); }
							} else { printError("[filewrite] expression expected."); }
						} else { printError("[filewrite] ',' missing."); }
					} else { printError("[filewrite] expression expected."); }
				} else { printError("[filewrite] ',' missing."); }
			} else { printError("[filewrite] identifier expexcted."); }			
		} else { printError("[filewrite] '(' missing."); }
	} 
	return 0;
}

int fileRead(struct item_t *item) {
	int sreg;
	struct item_t *firstItem;
	struct item_t *secondItem;
	struct item_t *thirdItem;
	struct object_t *object;

	firstItem = malloc(sizeof(struct item_t));
	secondItem = malloc(sizeof(struct item_t));	
	thirdItem = malloc(sizeof(struct item_t));
	
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
									if(symbol->id == PLUS || symbol->id == MINUS || symbol->id == TIMES || symbol->id == DIV || 
										compOp() || symbol->id == RPAR || symbol->id == SEMCOL || symbol->id == COMMA || 
										symbol->id == RSQBR || symbol->id == RCUBR || symbol->id == OR) {
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
									} else { printError("[fileread] ';' missing."); }							
								} else { printError("[fileread] ')' missing."); }
							} else { printError("[fileread] expression expected."); }
						} else { printError("[fileread] ',' missing."); }
					} else { printError("[fileread] expression expected."); }
				} else { printError("[fileread] ',' missing."); }
			} else { printError("[fileread] identifier expexcted."); }			
		} else { printError("[fileread] '(' missing."); }
	} 
	return 0;
}

/* ifCmd = "if" "(" expression ")" block [elseCmd] . */	
int ifCmd(struct item_t *item) {
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
			} else { printError("[if] expression expected."); }
		} else { printError("'(' missing."); }
	}
	return 0;
}


int whileLoop(struct item_t *item) {
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
	if (symbol->id == LCUBR) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		statementSeq();
		if (symbol->id == RCUBR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
		} else { printError("missing '}'"); return 0; }
	}
	cg_bJump(bJumpAddress);
	cg_fixLink(item->fls);
	return 1;
}

/* declaration = (typedefDec | typeSpec ["*"] identifier) ";" . */
/* declaration = (typedefDec | typeSpec [TIMES] identifier) SEMCOL . */
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
	
	if(isStruct) { off = 0; }
	else { off = locOffset; }
	while(1) {
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
					if (array != 0) {
						object->type = array;
					} else {
						object->type = type;
					}
				}

				if(isStruct) {
					insert(head, object);
				} else {
					if(lookUp(head, object->name) != 0)	{	//check additionally, if the symbol is in de paramsList
						printf("error: multible declaration of %s\n", object->name);
					} else {
						insert(locList, object);
					}
				}
				if(hasMoreTokens() == 0) { return decCounter; }
				getNextToken();
			} else { printError("';' missing."); }
		} else { printError("[declaration] identifier expected."); }
		decCounter = decCounter + 1;
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
			array = newType(TYPE_FORM_ARRAY);
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
				object->scope = GLOBAL_SCOPE;
				insert(globList, object);
				return 1;
			} else { printError("';' missing."); }
		}
	}
	return 0;
}

int returnType(struct item_t *item) {
	if(symbol->id == STRUCT) { 
		if(hasMoreTokens() == 0) { return 0; }		
		getNextToken();
	}
	if(typeSpec(item, globList) == 0) {
		printError("[returnType] type expected.");
	}
	if(hasMoreTokens() == 0) { return 0; }
	getNextToken();
	return 1;
}

struct object_t *createObject(string_t name) {
	struct object_t *object;
	object = malloc(sizeof(struct object_t));
	object->type = malloc(sizeof(struct type_t));
	object->name = name;
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
			if(ptr != 0 && ptr->class == OBJECT_CLASS_TYPE) {
				type->form = ptr->type->form;
				type->size = ptr->type->size;
				type->fields = ptr->type->fields;
				type->base = ptr->type->base;
			}
		}
	}
	if(identifier()) {
		ptr = lookUp(globList, symbol->valueStr);
		if(ptr != 0 && ptr->class == OBJECT_CLASS_TYPE) {
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
printf("LS: %d \n", localSize);
	cg_put(CMD_SUBI, SPTR, SPTR, localSize/4); 		// allocate callee's local variables
}

void epilogue(int paramSize) {
	cg_put(CMD_ADD, SPTR, 0, FPTR); 				// deallocate callee's frame and local variables 
	cg_put(CMD_POP, FPTR, SPTR, 4); 				// restore caller's frame
	cg_put(CMD_POP, LINK, SPTR, paramSize + 4); 	// restore return address
												// deallocate parameters
	if(strCmp(procedureContext->name, "main") != 0) {cg_put(CMD_RET, 0, 0, LINK); }			// return 
	else { cg_put(CMD_TRAP, 0, 0, 0); }
}

int procedureImplementation(struct item_t* item, string_t identifier) {
	struct object_t* object;
	if(strCmp(identifier, "main") == 0) { mainPos = PC + 1; }		//set jump to main method
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
	} else if(symbol->id == SEMCOL) {
		if(object->offset == 1) { object->offset = 0;}
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		return 1;
	} else { printError("missing '{'"); return 0; }

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

int formalParameters(struct object_t* object) {
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
	if(object->params == 0) { object->params = malloc(sizeof(struct object_t)); }
	ptr = object->params;
	if(ptr->name == 0) {
		ptr->name = malloc(sizeof(char)*64);
		ptr->type = malloc(sizeof(struct type_t));
		strncpy(ptr->name, identifier, 64);
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
		ptr->next->name = malloc(sizeof(char)*64);
		ptr->next->type = malloc(sizeof(struct type_t));
		strncpy(ptr->next->name, identifier, 64);
		ptr->next->type->form = type->form;
		ptr->next->type->size = type->size;
		ptr->next->type->fields = type->fields;
		ptr->next->type->base = type->base;
		ptr->next->scope = LOCAL_SCOPE;
		ptr->next->next = 0;
	}
 	return ptr;
}

struct object_t* formalParameter(struct object_t* object, struct object_t* formalParameter) {
	struct type_t* type;
	type = basicArrayRecordType();
	if (identifier()) {
		if (formalParameter != 0) {
			if (type->form != formalParameter->type->form) { printError("type mismatch in procedure declaration and call"); }
			//if (lookUp(object->params, symbol->valueStr) != 0) { printError("parameter name already used: "); } /* TODO:  */
		} else { formalParameter = createFormalParameter(object, type, symbol->valueStr); }
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		formalParameter = formalParameter->next;
	} else { printError("[formalParameter] identifier expected"); }
	return formalParameter;
}

int procedureReturn() {
	struct item_t* item;
	if (symbol->id == RETURN) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
	} else { return 0; }

	if ((symbol->id == PLUS) || (symbol->id == MINUS) || identifier() || (symbol->id == INT) ||
		(symbol->id == LPAR) || (symbol->id == STRING || number())) {
		item = malloc(sizeof(struct item_t)); 
		item->type = malloc(sizeof(struct type_t));
		expression(item); 
		if (item->type->form != procedureContext->type->form) { printError("return type mismatch"); return 0; }
		if (item->type->form == TYPE_FORM_BOOL) { cg_unloadBool(item); }
		cg_load(item);
		cg_put(CMD_ADD, RR, 0, item->reg);
		cg_releaseReg(item->reg); 
	}
	returnFJumpAddress = fJumpChain(returnFJumpAddress);
	return 1;
}

int fJumpChain(int branchAddress) {
	cg_put(CMD_BR, 0, 0, branchAddress);
	return PC - 1;
}

int pushUsedRegisters() {
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

int isBSR(int offset) {
	return (out_cmd_buff[offset]->op == CMD_BSR);
}

void procedureCall(struct item_t* item, string_t procName) {
	struct object_t* object;
	int nrOfRegs;
	nrOfRegs = 0;
	string_t identifier;
	identifier = malloc(sizeof(char)*64);
	strncpy(identifier, procName, 64);
	object = findProcedureObject(globList, identifier);
	if (object == 0) {
		printError("undeclared procedure: ");
		object = createObject(identifier);
		object->class = OBJECT_CLASS_PROC;
		object->type->form = TYPE_FORM_UNKNOWN; // TODO: infer return type 
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

int sJump(int branchAddress) {
	cg_put(CMD_BSR, 0, 0, branchAddress);
	return PC - 1; 
}

void pushParameter(struct item_t* item) {
	if (item->type->form == TYPE_FORM_BOOL) { cg_unloadBool(item); }
	cg_load(item);
	cg_put(CMD_PSH, item->reg, SPTR, 4);
	cg_releaseReg(item->reg); 
}

int actualParameters(struct object_t* object) {
	struct object_t* nextFormalParameter;
	struct item_t* item;
	if (symbol->id == LPAR) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
	} 
	else { printError("missing '('"); }
	nextFormalParameter = object->params;
	if ((symbol->id == PLUS) || (symbol->id == MINUS) || identifier() || (symbol->id == INT) ||
		(symbol->id == LPAR) || (symbol->id == STRING || number())) {
		nextFormalParameter = actualParameter(object, nextFormalParameter);
		while (symbol->id == COMMA) { 
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			nextFormalParameter = actualParameter(object, nextFormalParameter);
		}
	}
	while (nextFormalParameter != 0) {
		printError("[warning] actual parameter expected");
		item = malloc(sizeof(struct item_t));
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
		ptr->next->type = malloc(sizeof(struct type_t));
		ptr->next->type->form = type->form;
		ptr->next->type->size = type->size;
		ptr->next->type->fields = type->fields;
		ptr->next->type->base = type->base;
		ptr->next->scope = LOCAL_SCOPE;
		ptr->next->next = 0;
	}
 	return ptr;
}

struct object_t* actualParameter(struct object_t* object, struct object_t* formalParameter) {
	struct item_t* item;
	if ((symbol->id == PLUS) || (symbol->id == MINUS) || identifier() || (symbol->id == INT) ||
		(symbol->id == LPAR) || (symbol->id == STRING || number())) {
		item = malloc(sizeof(struct item_t));
		if (expression(item)) {
			if (formalParameter != 0) {
				if (item->type != 0 && (item->type->form != formalParameter->type->form)) 
				{	printError("[warning] type mismatch in procedure call"); }
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

int globalDec() {
	struct object_t *object;
	struct object_t *ptr;
	struct type_t *type;
	struct type_t *array;
	struct item_t *item;
	string_t identName;

	while(1) { 	
		type = 0;
		array = 0;
		ptr = 0;
		object = 0;
		item = 0;
		object = malloc(sizeof(struct object_t));
		identName = malloc(sizeof(char)*64);
		item = malloc(sizeof(struct item_t));
		item->type = malloc(sizeof(struct type_t));

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
			if(ptr != 0) { object->type = ptr->type; } 
			else { printError("[global dec] Type not found."); }
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
			else if(symbol->id == SEMCOL) {
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

int statementSeq () {
	struct item_t *item;
	if(symbol->id == RCUBR) { return 1; }
	while(1) {
		item = malloc(sizeof(struct item_t));
		item->type = malloc(sizeof(struct type_t));
		while(identifier() == 0 && number() == 0 && symbol->id != WHILE && symbol->id != IF 
				&& symbol->id != RETURN && symbol->id != LPAR && symbol->id != ELSE && symbol->id != PRINTF
				&& symbol->id != OPEN && symbol->id != CLOSE && symbol->id != WRITE && symbol->id != READ) {
			printError("statSeq(1): identifier, number, while, if or return expected.");
			if(hasMoreTokens() == 0) { return 0; }			
			getNextToken();
		}
		if(ifCmd(item)) {} 
		else if(printMethod(item)) {}
		else if(whileLoop(item)) {}
		else if(expression(item) || procedureReturn()) {
			if(symbol->id == SEMCOL) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
			} else {
				printError("[stateSeq] ';' missing.");	
			}
		} else {
			while(symbol->id != SEMCOL && symbol->id != RPAR && symbol->id != RCUBR) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
			}
		}
		if(symbol->id == RCUBR) { return 1; }
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
	while(j) {
		j = structDec();
		if(j == 1) { getNextToken(); }
	}
	while(k) {
		k = globalDec();
		if(k) { getNextToken(); }
	}
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
	errorCounter = 0;
	PC = 1;	

	initTMCmd();
	cg_initRegs();
	initItemModes();
	initOperators();	
	initSymbolTable();
	initOutputFile();

	printf("\nstart parsing %s...\n", srcfile);
	while ( hasMoreTokens() ) {
		getNextToken();
		if(symbol->id == ERROR) {
			printError("[program] file does not exist.");
			return 0;
		}
		i = programm();
	}

	if(errorCounter == 0) { finalizeOutputFile(); printf("... %s successful generated.\n", outfile); }
	else { printf("%d Errors\n", errorCounter); }
	printf("\n -- DONE. --\n\n");
	return i;
}
