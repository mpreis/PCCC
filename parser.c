/*
 * PSEUDOCODE - parser.c
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */
#include "parser.h"
#include "scanner.h"
#include "tokenMapping.h"

void printError(char *msg) {
	printf("\t%i:%i: error: %s\n", symbol.lineNr, symbol.colNr, msg);
}

/* ["-"] digit {digit} . */
int number() {
	if(symbol.id == MINUS)  { getNextToken(); }
	if(symbol.id == NUMBER) { return 1; }
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

int op() {
	if(symbol.id == PLUS || symbol.id == MINUS || symbol.id == DIV || symbol.id == TIMES || boolOp()) {
		return 1;
	}
	return 0;
}

/* letter {letter | digit} . */
int identifier() {
	if(symbol.id == IDENT) { return 1; }
	return 0;
}

/* include . */
int include() {
	if(symbol.id == INCLUDE) { return 1; }
	return 0;
}

/* identifier | number | procCall | reference | "(" arithExp ")" . */
int factor() {
	if(symbol.id == NUMBER || symbol.id == CHARACTER || symbol.id == STRING) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(op()) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			return expression();
		}
		return 1;
	}
	else if(symbol.id == LPAR) { 
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(expression()) {
			if(symbol.id == RPAR) {
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
	else if(identifier()) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();

		/* var = */		
		if(symbol.id == EQSIGN) {
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

		/* var . */
		else if(symbol.id == DOT) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(identifier()) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();

				if(symbol.id == LSQBR) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();	
					if(identifier() || number()) {
						if(hasMoreTokens() == 0) { return 0; }
						getNextToken();
						if(symbol.id == RSQBR) {
							if(hasMoreTokens() == 0) { return 0; }
							getNextToken();													
						}
					}
				}

				if(symbol.id == EQSIGN || op()) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					return expression();
				}
				return 1;
			}
			return 0;
		}

		/* var ) */
		else if(symbol.id == RPAR) {
			return 1;
		}

		/* var ( */
		else if(symbol.id == LPAR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			paramList();
			if(symbol.id == RPAR) {
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
		else if(symbol.id == LSQBR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(number() || identifier()) {	
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				if(symbol.id == RSQBR) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					/* var [1] = | var [1] + */
					if(symbol.id == EQSIGN || op()) {
						if(hasMoreTokens() == 0) { return 0; }
						getNextToken();
						return expression();	
					}
					return 1;
				} else {
					printError("']' missing.");
				}
			} else {
				printError("identifier or number expected.");
			}
			return 0;
		}

		/* var = */		
		if(symbol.id == SEMCOL) { return 1; }
	}
	return 0;
}

/* factor { ( "*" | "/" ) factor } . */
int term() {
	while(1) {
		if(factor() == 0) { return 0; }
		if(symbol.id == PLUS || symbol.id == MINUS || boolOp() || symbol.id == RPAR || symbol.id == SEMCOL) {
			return 1;
		}
		if(symbol.id == TIMES || symbol.id == DIV) {
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
	if(symbol.id == MINUS) { 
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken(); 
	}
	while(1) {
		if(term() == 0) { return 0; }
		if(boolOp() || symbol.id == RPAR || symbol.id == SEMCOL || hasMoreTokens() == 0) {
			return 1;
		}
		if(symbol.id == PLUS || symbol.id == MINUS) {
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
		if(symbol.id == RPAR || symbol.id == SEMCOL || hasMoreTokens() == 0) {
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
	if(symbol.id == RPAR) { return 1; }
	while(1) {
		if(identifier() == 0 && number() == 0 && symbol.id != STRING && symbol.id != CHARACTER) {
			printError("paramList: identifier expected.");
			return 0;
		}	
		if(hasMoreTokens() == 0) { return 1; }
		getNextToken();
		if(symbol.id == DOT) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(identifier()) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();	
				if(symbol.id == LSQBR) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();	
					if(identifier() || number()) {
						if(hasMoreTokens() == 0) { return 0; }
						getNextToken();
						if(symbol.id == RSQBR) {
							if(hasMoreTokens() == 0) { return 0; }
							getNextToken();													
						} else {
							printError("']' missing.");
						}
					}
				}
			} else {
				printError("paramList(2): identifier expected.");
			}
		}
		if(symbol.id == RPAR) { return 1; }
		if(symbol.id == COMMA) {
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
	if(symbol.id == IF) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol.id == LPAR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(expression()) {
				if(symbol.id == RPAR) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					if(block()) {
						if(hasMoreTokens() == 0) { return 0; }
						getNextToken();
						if(symbol.id == ELSE) {
							if(hasMoreTokens() == 0) { return 0; }
							getNextToken();
							if(symbol.id == IF) {
								ifCmd();
							} else {
								block();
								if(hasMoreTokens() == 0) { return 0; }
								getNextToken();
							}
						}
					}
					return 1;
				} else {
					printError("')' missing.");
				}
			} else {
				printError("expression expected.");
			}
		} else {
			printError("'(' missing.");
		}
	}
	return 0;
}

/* "while" "(" expression ")" block . */
int whileLoop() {
	if(symbol.id == WHILE) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol.id == LPAR) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(expression()) {
				if(symbol.id == RPAR) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					if(block()) { 
						if(hasMoreTokens() == 0) { return 0; }
						getNextToken();
						return 1; 
					}
				} else {
					printError("')' missing.");
				}
			} else {
				printError("expression expected.");
			}
		} else {
			printError("'(' missing.");
		}
	}
	return 0;
}

/* ret = "return" expression . */
int ret() {
	if(symbol.id == RETURN) {
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

int procParList() {
	if(symbol.id == RPAR) { return 1; }
	while(1) {
		if(typeSpec() == 0) {
			printError("procParList: typeSpec (int, char or void) expected.");
			return 0;
		}	
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol.id == TIMES) {	
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
		}
		if(identifier()) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(symbol.id == LSQBR) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				if(symbol.id == RSQBR) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
				} else { 
					printError("']' missing.");
					return 0; 
				}
			}
			if(symbol.id == RPAR) { return 1; }
			if(symbol.id == COMMA) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
			} else {
				printError("',' missing.");
			}
		} else { return 0; }
		if(symbol.id == RPAR) { return 1; }
	}
}

int declaration() {
	while(1) { 	
		if(typeSpec() == 0) {
			return 1;		
		}
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol.id == TIMES) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();			
		}
		if(identifier()) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(symbol.id == LSQBR) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				if(identifier() || number()) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					if(symbol.id == RSQBR) {
						if(hasMoreTokens() == 0) { return 0; }
						getNextToken();
					} else {
						printError("']' missing.");
					}
				}						
			}
			if(symbol.id == SEMCOL) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
			}
		} else {
			printError("identifier expected.");
		}
	}
}

/* "typedef" "struct" "{" declaration {declaration} "}" identifier ";" . */
int structDec() {
	if(symbol.id == TYPEDEF) {
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol.id == STRUCT) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			if(symbol.id == LCUBR) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				declaration();
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				if(symbol.id == RCUBR) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					if(identifier()) {
						if(hasMoreTokens() == 0) { return 0; }
						getNextToken();
						if(symbol.id == SEMCOL) {
							return 1;
						} else { printError("';' missing."); }
					} else { printError("identifier expected."); }
				} else { printError("'}' missing."); }
			} else { printError("'{' missing."); } 
		} else { printError("struct expected."); }
	}
	return 0;
}


int globalDec() {
	while(1) { 
		while(typeSpec() == 0 && hasMoreTokens()) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			printError("globalDec: typeSpec (char, int or void) expected.");			
		}
		if(hasMoreTokens() == 0) { return 0; }
		getNextToken();
		if(symbol.id == TIMES) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();			
		}
		if(identifier()) {
			if(hasMoreTokens() == 0) { return 0; }
			getNextToken();
			/* procHead */			
			if(symbol.id == LPAR) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				if(procParList()) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					if(symbol.id == SEMCOL) { return 1; } 
					else if(symbol.id == LCUBR) {
						if(hasMoreTokens() == 0) { return 0; }
						getNextToken();

						declaration();

						if(statementSeq()) {
							if(symbol.id == RCUBR) { return 1; }
						}						
					}
				}
			} 
			/* declaration */
			else if(symbol.id == SEMCOL) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
			} else if(symbol.id == COMMA) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
				while(identifier()) {
					if(hasMoreTokens() == 0) { return 0; }
					getNextToken();
					if(symbol.id == COMMA) {
						if(hasMoreTokens() == 0) { return 0; }
						getNextToken();
					} else if (symbol.id == SEMCOL) {
						return 1;
					}
				}
			}
		}
	}
}

int statementSeq () {
	if(symbol.id == RCUBR) { return 1; }
	while(1) {
		while(identifier() == 0 && number() == 0 && symbol.id != WHILE && symbol.id != IF && symbol.id != RETURN && symbol.id != LPAR && symbol.id != ELSE) {
			if(hasMoreTokens() == 0) { return 0; }			
			getNextToken();
			printError("statSeq(1): identifier, number, while, if or return expected.");
		}
		if(expression() || ret()) {
			if(symbol.id == SEMCOL) {
				if(hasMoreTokens() == 0) { return 0; }
				getNextToken();
			} else {
				printError("';' missing.");			
			}
		}
		else if(ifCmd()) {} 
		else if(whileLoop()) {}

		if(symbol.id == RCUBR) {
			return 1;
		}
	}
}

/* "{" statementSeq "}" . */
int block() {
	if(symbol.id == LCUBR) {
		if(hasMoreTokens() == 0) { return 0; }			
		getNextToken();
		statementSeq();
		if(symbol.id == RCUBR) { return 1; }
		printError("'}' missing.");			
	}
	return 0;
}

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
/* {include} {declaration | structDec} {procedure} . */
int startParsing(){
	int i;
	printf("\nstart parsing...\n");
	while ( hasMoreTokens() ) {
		getNextToken();
		i = programm();

	}
	printf("\n -- DONE. --\n\n");
	return i;
}
