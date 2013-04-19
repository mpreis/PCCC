/*
 * PSEUDOCODE - parser.c
 * authors: thomas huetter 1120239, mario preishuber 1120643
 */
#include "scanner.h"
#include "parser.h"

void printError() {
	printf("%i:%i: error: ", symbol.lineNr, symbol.colNr);
}

int digit() {
	if(isDigit(symbol.digitValue)) {
		return 1;
	}
	return 0;
}

int number() {
	if(symbol.id == MINUS) {
		getNextToken();
	}
	if(digit()) {
		getNextToken();
		while(digit()) {
			getNextToken();
		}
		return 1;
	}
	return 0;
}

int letter() {
	if(isLetter(symbol.valueStr[0])) {
		return 1;
	}
	return 0;
}

/* identifier "." identifier . */
int reference() {
	if(identifier()) {
		getNextToken();
		if(symbol.id == DOT) {
			getNextToken();
			if(identifier()) {
				return 1;
			}
		}
	}
	return 0;
}

/* identifier | number | "(" expression ")" | procCall | reference . */
int factor() {
	if(identifier() || number() || procCall() || reference() || symbol.id == LPAR) {
		if(symbol.id == LPAR) {
			getNextToken();
			if(expression()) {
				getNextToken();
				if(symbol.id == RPAR) {
					return 1;
				}
			}
		}
		else return 1;
	}
	return 0;
}

/* factor { ( "*" | "/" ) factor } . */
int term() {
	if(factor()) {
		getNextToken();
		while(symbol.id == TIMES || symbol.id == DIV) {
			getNextToken();
			if(factor()) {
				getNextToken();
			}
		}
		return 1;
	}
	return 0;
}

/* [ "-" ] term { ( "+" | "-" ) term } . */
int expression() {
	if(symbol.id == MINUS) {
		getNextToken();
	}
	if(term()) {
		getNextToken();
		while(symbol.id == PLUS || symbol.id == MINUS) {
			getNextToken();
			if(term()) {
				getNextToken();
			}
		}
		return 1;
	}
	return 0;
}

/* "==" | "!=" | "<=" | ">=" | "<" | ">" | "&&" | "||" . */
int boolOp() {
	if(symbol.id == EQ || symbol.id == NEQ || symbol.id == LT || symbol.id == GT || symbol.id == LET || symbol.id == GET || symbol.id == AND || symbol.id == OR) {
		return 1;
	}
	return 0;
}

/* "int" | "char" . */ 
int typeSpec() {
	if(symbol.id == INT || symbol.id == CHAR) {
		return 1;
	}
	return 0;
}

/* "*" identifier . */ 
int pointer() {
	if(symbol.id == TIMES) {
		getNextToken();
		if(identifier()) {
			return 1;
		}
	}
	return 0;
}

/* letter {letter | digit} . */ 
int identifier() {
	if(letter()) {
		getNextToken();
		while(letter() || digit()) {
			getNextToken();
		}
		return 1;
	}
	return 0;
}

/* "{" {command} "}" . */ 
int block() {
	if(symbol.id == LCUBR) {
		getNextToken();
		while(command()) {
			getNextToken();
		}
		if(symbol.id == RCUBR) {
			getNextToken();
		}
		return 1;
	}
	return 0;
}

/* expression { boolOp expression } . */ 
int boolExp() {
	if(expression()) {
		getNextToken();
		while(boolOp()) {
			getNextToken();
			if(expression()) {
				getNextToken();
			}
		}
		return 1;
	}
	return 0;
}

/* "while" "(" boolExp ")" block . */ 
int whileLoop() {
	if(symbol.id == WHILE) {
		getNextToken();
		if(symbol.id == LPAR) {
			getNextToken();
			if(boolExp()) {
				getNextToken();
				if(symbol.id == RPAR) {
					getNextToken();
					if(block()) {
						return 1;
					}
				}
			}
		}
	}
	return 0;
}

/* "else" (ifCmd | block) . */ 
int elseCmd() {
	if(symbol.id == ELSE) {
		getNextToken();
		if(ifCmd() || block()) {
			return 1;
		}
	}
	return 0;
}

/* "if" "(" boolExp ")" block [else] . */ 
int ifCmd() {
	if(symbol.id == IF) {
		getNextToken();
		if(symbol.id == LPAR) {
			getNextToken();
			if(boolExp()) {
				getNextToken();
				if(symbol.id == RPAR) {
					getNextToken();
					if(block()) {
						getNextToken();
						if(elseCmd()) {
							
						}
						return 1;
					}
				}
			}
		}
	}
	return 0;
}

/* identifier ["[" (number | identifier) "]"] "=" expression ";" . */ 
int init() {
	if(identifier()) {
		getNextToken();
		if(symbol.id == LSQBR) {
			getNextToken();
			if(number() || identifier()) {
				getNextToken();
				if(symbol.id == RSQBR) {
					getNextToken();
				}
			}	
		}
		if(symbol.id == EQSIGN) {
			getNextToken();
			if(expression()) {
				getNextToken();
				if(symbol.id == SEMCOL) {
					return 1;
				}
			}
		}
	}
	return 0;
}

/* "return" expression ";" . */ 
int ret() {
	if(symbol.id == RETURN) {
		getNextToken();
		if(expression()) {
			getNextToken();
			if(symbol.id == SEMCOL) {
				return 1;
			}	
		}
	}
	return 0;
}

/* identifier {"," identifier} . */ 
int paramList() {
	if(identifier()) {
		getNextToken();
		while(symbol.id == COMMA) {
			getNextToken();
			if(identifier()) {
				getNextToken();
			}
		}
		return 1;
	}
	return 0;
}

/* identifier "(" [paramList]")" . */ 
int procCall() {
	if(identifier()) {
		getNextToken();
		if(symbol.id == LPAR) {
			getNextToken();
			if(paramList()) {
				getNextToken();
			}
			if(symbol.id == RPAR) {
				return 1;
			}
		}
	}
	return 0;
}

/* whileLoop | ifCmd | init | ret | procCall | (expression ";"). */ 
int command() {
	if(whileLoop() || ifCmd() || init() || ret() || procCall() || expression()) {
		if(expression()) {
			getNextToken();
			if(symbol.id == SEMCOL) {
				return 1;
			}
			return 0;	
		}
		return 1;
	}
	return 0;
}

/* typeSpec (identifier | pointer) { "," typeSpec (identifier | pointer) } . */ 
int procParList() {
	if(typeSpec()) {
		getNextToken();
		if(identifier() || pointer()) {
			getNextToken();
			while(symbol.id == COMMA) {
				getNextToken();
				if(typeSpec()) {
					getNextToken();
					if(identifier() || pointer()) {
						getNextToken();
					}
				}
			}
			return 1;
		}
	}
	return 0;
}

/* ((typeSpec ["*"]) | "void") identifier "(" [procParList] ")" . */ 
int procHead() {
	if(typeSpec() || symbol.id == VOID) {
		if(typeSpec()) {
			getNextToken();
			if(symbol.id == TIMES) {
				getNextToken();
			}
		}
		if(identifier()) {
			getNextToken();
			if(symbol.id == LPAR) {
				getNextToken();
				if(procParList()) {
					getNextToken();
				}
				if(symbol.id == RPAR) {
					return 1;
				}
			}
		}
	}
	return 0;
}

/* declaration	=	typeSpec (identifier | pointer) { "," (identifier | pointer) } ";" . */
int declaration() {
	if(typeSpec()) {	
		getNextToken();
		if(identifier() || pointer()) {	
			getNextToken();
			while(symbol.id == COMMA) {
				getNextToken();
				if(identifier() == 0 && pointer() == 0) {
					return 0;
				}
				getNextToken();
			}
			if(symbol.id == SEMCOL) {
				return 1;
			}
		}
	}
	return 0;
}

/* procHead "{" {decalaration} {command} "}" . */ 
int procedure() {
	if(procHead()) {
		getNextToken();
		if(symbol.id == LCUBR) {
			getNextToken();
			while(declaration()) {
				getNextToken();
			}
			while(command()) {
				getNextToken();
			}
			if(symbol.id == RCUBR) {
				return 1;
			}
		}
	}
	return 0;
}

/* typeSpec identifier "[" (number | identifier) "]" ";" . */
int arrayDec() {
	if(typeSpec()) {	
		getNextToken();
		if(identifier()) {	
			getNextToken();
			if(symbol.id == LSQBR) {	
				getNextToken();
				if(number() || identifier()) {	
					getNextToken();
					if(symbol.id == RSQBR) {
						getNextToken();
						if(symbol.id == SEMCOL) {
							return 1;
						}
					}
				}
			}
		}
	}
	return 0;
}

/* "typedef" "struct" "{" declaration {declaration} "}" identifier ";" . */
int structDec() {
	if(symbol.id == TYPEDEF) {	
		getNextToken();
		if(symbol.id == STRUCT) {	
			getNextToken();
			if(symbol.id == LCUBR) {	
				getNextToken();
				if(declaration()) {	
					getNextToken();
					while(declaration()) {	
						getNextToken();
					}
					if(symbol.id == RCUBR) {	
						getNextToken();
						if(identifier()) {	
							getNextToken();
							if(symbol.id == SEMCOL) {
								return 1;
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

int include() {
	if(symbol.id == INCLUDE) {
		return 1;
	}
	return 0;
}

int programm() {
	int i, j, k;
	i = 1; j = 1; k = 1;
	while(i) {
		i = include();
		getNextToken();
	}
	while(j) {
		j = declaration() || structDec() || arrayDec();
		getNextToken();
	}
	while(k) {
		k = procedure();
		getNextToken();
	}
	return i && j && k;
}

void startParse(){
	while ( hasMoreTokens() ) {	
		getNextToken();
		if(programm()) {
			printf("\n -- nwir sind gut\n");
		}
		printToken(symbol);
	}
	printf("\n");
}


