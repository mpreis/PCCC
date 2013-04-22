/*
 * PSEUDOCODE - parser.c
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */
#include "parser.h"
#include "scanner.h"
#include "tokenMapping.h"

void printError(char *msg) {
	printf("%i:%i: error: %s\n", symbol.lineNr, symbol.colNr, msg);
}

/* 0, .. 9 */
int digit() {
	if(isDigit(symbol.digitValue)) { return 1; }
	return 0;
}

/* ["-"] digit {digit} . */
int number() {
	if(symbol.id == MINUS)  { getNextToken(); }
	if(symbol.id == NUMBER) { return 1; }
	return 0;
}

/* a, .. z, A, .. Z */
int letter() {
	if(isLetter(symbol.valueStr[0])) { return 1; }
	return 0;
}

/* "int" | "char" . */ 
int typeSpec() {
	if(symbol.id == TYPEDEF) { getNextToken(); }
	if(symbol.id == INT || symbol.id == CHAR || symbol.id == VOID) { return 1; }
	return 0;
}

/* "==" | "!=" | "<=" | ">=" | "<" | ">" | "&&" | "||" . */
int boolOp() {
	if(symbol.id == EQ || symbol.id == NEQ || symbol.id == LT || symbol.id == GT || symbol.id == LET || symbol.id == GET || symbol.id == AND || symbol.id == OR) {
		return 1;
	}
	return 0;
}

/* letter {letter | digit} . */
int identifier() {
	if(symbol.id == IDENT) { return 1; }
	return 0;
}

/* include */
int include() {
	if(symbol.id == INCLUDE) { return 1; }
	return 0;
}

/* identifier | number | procCall | reference | "(" arithExp ")" . */
int factor() {
	if(identifier() || number() /*|| procCall || reference*/ || symbol.id == LPAR) {
		if(symbol.id == LPAR) {
			if(hasMoreTokens() == 0) { return 0; }			
			getNextToken();
			if(expression()) {
				if(symbol.id == RPAR) {
					return 1;
				}
			}
		}
		return 1;
	}
	return 0;
}

/* factor { ( "*" | "/" ) factor } . */
int term() {
	printf("start_term: "); printToken(symbol);
	while(1) {
		if(factor() == 0) {
			printError("term(1): identifier, number, procedure call, reference or (arithExp) expected.");
			return 0;
		}	
		if(hasMoreTokens() == 0) { return 1; }
		getNextToken();
		if(symbol.id == PLUS || symbol.id == MINUS || boolOp() || symbol.id == RPAR || symbol.id == SEMCOL) {
			return 1;
		}
		if(symbol.id == TIMES || symbol.id == DIV) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
		} else {
			printError("* or / missing.");
			return 1;
		}
	}
}

/* [ "-" ] term { ( "+" | "-" ) term } . */
int arithExp() {
	if(symbol.id == MINUS) { 
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken(); 
	}
	while(1) {
		printToken(symbol);
		if(term() == 0) {
			printError("arithExp(1): identifier, number, procedure call, reference or (arithExp) expected.");
			printToken(symbol);
			return 0;
		}
		if(boolOp() || symbol.id == RPAR || symbol.id == SEMCOL || hasMoreTokens() == 0) {
			return 1;
		}
		if(symbol.id == PLUS || symbol.id == MINUS) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
		} else {
			printError("+ or - missing.");
			return 1;
		}
	}
}

/* arithExp { boolOp arithExp } . */
int expression() {
	while(1) {
		if(arithExp() == 0) {
			printError("expression(1): identifier, number, procedure call, reference or (arithExp) expected.");
			return 0;
		}
		if(symbol.id == RPAR || symbol.id == SEMCOL || hasMoreTokens() == 0) {
			return 1;
		}
		if(boolOp()) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
		} else {
			printError("boolean operator missing.");
			return 1;
		}
	}
}

int statementSeq () {
	while(1) {
		/* TODO add: if while return; remove: number (only for testing!) */
		while(identifier() == 0 && number() == 0 && symbol.id != LPAR) {
			if(hasMoreTokens() == 0) { return 0; }			
			getNextToken();
			printError("statSeq(1): identifier expected.");
		}
		printf("\n");
		if(init() || expression()) {
			printf("im if: "); printToken(symbol);
			if(symbol.id == SEMCOL) {				
				printf("--- IS STATESEGMENT SEQ!\n\n");
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
			} else {
				printError("; missing.");			
			}
			if(symbol.id == RCUBR) {
				printf("--- end of statement seq. --- \n\n");
				return 1;
			}
		}
	}
}

/* identifier ["[" (number | identifier) "]"] "=" expression . */
int init() {
	if(identifier()) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol.id == LSQBR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(number() || identifier()) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				if(symbol.id == RSQBR) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
				}
			}
		}
		if(symbol.id == EQSIGN) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			printf("=: "); printToken(symbol);
			if(expression()) {
				printf("exp: "); printToken(symbol);
				return 1;
			}
		}
	}
	return 0;
}

int programm() {
	int i, j, k;
	i = 1; j = 1; k = 1;

	statementSeq();
/*
	while(1) {
		while(identifier() == 0 || number() == 0 || ...){

		}

	while(i) {
		i = arithExp();
		if(i == 1) { getNextToken(); }
		printf(" -- i:%i\n\n",i);	
	}
	


	while(i) {
		i = include();
		if(i == 1) { getNextToken(); }
		printf(" -- i:%i\n",i);	
	}

	while(j) {
		j = declaration();
		if(j == 0) {
			j = structDec();
		}
		printf(" -- j:%i\n", j);
		if(j) { getNextToken(); }
	}
	while(k) {
		k = procedure();
		printf(" -- k:%i\n",k);
		getNextToken();
	}
*/
	return i;
}
/* {include} {declaration | structDec} {procedure} . */
int startParsing(){
	int i;
	printf("\nstart parsing...\n");
	while ( hasMoreTokens() ) {
		getNextToken();
		i = programm();
		if(i == 1) {
			printf("\n -- DONE. --\n");
		}
	}
	printf("\n");
	return i;
}


