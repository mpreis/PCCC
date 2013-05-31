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
int initOutputFile() {
	out_cmd_buff = malloc(sizeof(struct cmd_t *) * 1024);
}

void finalizeOutputFile() {
	int wb;
	int i;
	int out_fp_bin;
	int *tempBuff;
	tempBuff = malloc(32);
	out_fp_bin = open(outfile, 513, 448); /* 65  ... O_CREAT (64)  | O_WRONLY (1) 448 ... S_IWUSR | S_IRUSR | S_IXUSR  --> Ubuntu */
										 /* 513 ... O_CREAT (512) | O_WRONLY (1) 448 ... S_IWUSR | S_IRUSR | S_IXUSR  --> Mac */
	if(out_fp_bin < 0) {
		printError("can not open/create output file.");
		return;
	}
	cg_put(CMD_TRAP,0,0,0);
	tempBuff[0] = cg_encode(CMD_CS,	0, 0, PC);
	write(out_fp_bin, tempBuff, 4); 
	tempBuff[0] = cg_encode(CMD_GP,	0, 0, nrOfGVar*4);	/* !!! da globale variablen NICHT initialisiert werden können, */
	write(out_fp_bin, tempBuff, 4); 						/* !!! sind immer nur 4 byte speicher nötig */
	tempBuff[0] = cg_encode(CMD_SP,	0, 0, nrOfStrs);		/*TODO potentielles problem !!!*/
	write(out_fp_bin, tempBuff, 4); 
	i = 0;
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
	copy->type = orig->type;
	copy->reg = orig->reg;
	copy->offset = orig->offset;
	copy->value = orig->value;
	copy->op = orig->op;
	copy->fls = orig->fls;
	copy->tru = orig->tru;
}
/*
void copyType(struct type_t *copy, struct type_t *orig) {

	while(1) {
		copy->form = orig->form;
		copy->size = orig->size;
		if(orig->type != 0) { 
			copy->type = malloc(sizeof(struct type_t)); 
		}
	}
}*/

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
	GPTR = 28;
	LPTR = 29;
	HPTR = 30;
	regs = malloc( nrOfRegs*sizeof(int) );
	while (i < nrOfRegs) { regs[i] = 0; i = i + 1; }
}
int cg_requestReg() {
	int i; i = 1;
	/* register for calculation: 1-27 */
	while (i < 28) { 
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
			//cg_simpleExpOR(leftItem);
			cg_loadBool(rightItem);
			leftItem->reg = rightItem->reg; 
			leftItem->fls = rightItem->fls;
			leftItem->tru = cg_concatenate(rightItem->tru, leftItem->tru);
			leftItem->op = rightItem->op;
		} else { printError("boolean expressions expected"); }
	}
	else if(leftItem->type->form == TYPE_FORM_INT && rightItem->type->form == TYPE_FORM_INT) {
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
			//cg_termOpAND(leftItem);
			cg_loadBool(rightItem);
			leftItem->reg = rightItem->reg;
			leftItem->fls = cg_concatenate(rightItem->fls, leftItem->fls);
			leftItem->tru = rightItem->tru;
			leftItem->op = rightItem->op;
		} else { printError("boolean expressions expected"); }
	}	
	else if(leftItem->type->form == TYPE_FORM_INT && rightItem->type->form == TYPE_FORM_INT) {
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
	if ((leftItem->type->form == TYPE_FORM_INT) && (rightItem->type->form == TYPE_FORM_INT)) {
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
	if(leftItem->type->form != rightItem->type->form && rightItem->type->form != TYPE_FORM_VOID) { 
		printError("[assignment] Type mismatch in assignment"); 
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
	cg_put(CMD_MAL,0,0,item->reg);
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
			printError("unknown type.");
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
		leftItem->type->form = object->type->form;
		leftItem->type->size = object->type->size;
		leftItem->type->fields = object->type->fields;
		leftItem->type->base = object->type->base;

		if(object->scope == GLOBAL_SCOPE) { leftItem->reg = GPTR; } 
		else { leftItem->reg = LPTR; }
		leftItem->offset = object->offset;

		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();

		selector(leftItem);

		/* var = */		
		if(symbol->id == EQSIGN) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
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
				copyItem(item,leftItem);
				return 1;
			} else {
				printError("')' missing.");
			}
			return 0;
		}

		if(symbol->id == SEMCOL || symbol->id == COMMA || symbol->id == RSQBR || symbol->id == ARROW) 
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
				symbol->id == RSQBR || symbol->id == RSQBR || symbol->id == ARROW) {
			copyItem(item, leftItem);
			return 1;
		}
		if(symbol->id == TIMES || symbol->id == DIV || symbol->id == AND) {
			if(symbol->id == TIMES) { op = OP_MUL; }
			if(symbol->id == DIV) { op = OP_DIV; }
			if(symbol->id == AND) { op = OP_AND; }
			cg_termOpAND(leftItem);
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
			if(symbol->id == OR) { op = OP_OR; }
			cg_simpleExpOR(leftItem);
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
							if(symbol->id == IF) {
								ifCmd(item);
							} else {
								block();
								if(hasMoreTokens() == 0) { return 0; }
								getNextToken();
								cg_fixUp(fJumpAddress);
							}
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
	int off;
	item = malloc(sizeof(struct item_t));
	item->type = malloc(sizeof(struct type_t));

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
			if(hasMoreTokens() == 0) { return 0; }		
			getNextToken();
		}
		type = newType(typeSpec(item, head));
		object->offset = off;

		if(type->form == 0) { return 1; } 
		else if(type->form == TYPE_FORM_RECORD) {
			ptr = lookUp(head, symbol->valueStr);
			if(ptr == 0) {
				ptr = lookUp(globList, symbol->valueStr);
				if(ptr != 0) {
					object->type = ptr->type;
				}
			} else { object->type = ptr->type; }
		}
		off = off - 4;	/* size of local variable int,char 4byte; pointer to struct 4byte  */

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
			if(ptr != 0) { object->type = ptr->type; } 
			else { printError("[global dec] Type not found."); }
		}
		globOffset = globOffset - 4;

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
				&& symbol->id != RETURN && symbol->id != LPAR && symbol->id != ELSE) {
			if(hasMoreTokens() == 0) { return 0; }			
			getNextToken();
			printError("statSeq(1): identifier, number, while, if or return expected.");
		}
		if(ifCmd(item)) {} 
		else if(whileLoop(item)) {}
		else if(expression(item) || ret()) {
			if(symbol->id == SEMCOL) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
			} else {
				printError("';' missing.");	
			}
		}

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
	PC = 0;	

	initTMCmd();
	cg_initRegs();
	initItemModes();
	initOperators();	
	initSymbolTable();
	initOutputFile();

	printf("\nstart parsing %s...\n", srcfile);
	while ( hasMoreTokens() ) {
		getNextToken();
		i = programm();
	}
	finalizeOutputFile();
	printf("\n -- DONE. --\n\n");
	return i;

}

