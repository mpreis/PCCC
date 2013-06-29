/*
 * PSEUDOCODE - scanner.c
 * authors: thomas huetter 1120239, mario preishuber 1120643
 *
 */
#include "scanner.h"
#include "tokenMapping.h"

int fd;
int lineNr;
int colNr;
char ungetc;

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
	symbol = malloc(sizeof(struct token_t));
	symbol->valueStr = malloc(64*sizeof(char));
	symbol->id = INIT; 
	symbol->digitValue = -1; 
	strnCpy(symbol->valueStr, "", 0);
}

int hasMoreTokens() {
	char c; c = getChar();
	while (c == '\n') {
		c = getChar();
		if (c == -1) { return 0; }
	}
	if (c == -1) { return 0; }
	ungetChar(c);
	return 1;
}

int hasMoreChars() {
	char c; c = getChar();
	if (c == -1) { return 0; }
	ungetChar(c);
	return 1;
}

void printToken(struct token_t *t) { 
	printf("TOKEN: %i %s %i (%i/%i)\n", t->id, t->valueStr, 
		t->digitValue, t->lineNr, t->colNr); 
}

void printSymbol(char *prefix) { 
	printf("%s %i %s %i (%i/%i)\n", prefix, symbol->id, symbol->valueStr, 
		symbol->digitValue, symbol->lineNr, symbol->colNr); 
}


char getChar() {
	char c;
	char *buff;
	int r;
	int charSize;

	charSize = sizeof(char);
	buff = malloc(charSize);
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
	while(*s1 == *s2) {
		if(*s1 == '\0') { return 0; }
		s1 = s1 + 1;
		s2 = s2 + 1;
	}
	if(*s1 < *s2) { return -1; }
	return 1;
}

int strnCpy(char *s1, char *s2, int n) {
	int i; i = 0;
	while (i < n && s2[i] != 0) {
		s1[i] = s2[i];
		i = i+1;
	}
	s1[i] = '\0';
	return 0;
}

int isLetter(char c) {
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

int isDigit(char c) { return (c >= '0' && c <= '9'); }

int getNumber(char nr) {
	int number; 
	char actNumber;
	number = nr - '0';
	actNumber = getChar();
	while(isDigit(actNumber)) {
		number = number * 10 + (actNumber - '0');
		actNumber = getChar();
	}
	ungetChar(actNumber);
	return number;
}

void getIdentifier(char *identifier, char c) {
	char nc;
	int i;
	nc = getChar();
	i = 1;
	identifier[0] = c;
	while((isLetter(nc) || isDigit(nc) || nc == '_') && hasMoreChars() && i < 63) {
		identifier[i] = nc;
		nc = getChar();
		i = i + 1;
	}
	identifier[i] = '\0';
	ungetChar(nc);
}

/* fd ... filedescriptor */
void getNextToken() {
	char c;
	char nc;
	char nnc;
	char *identifier;
	char *buff;
	int i;
	int identSize;
	int buffSize;

	identSize = 64*sizeof(char);
	identifier = malloc(identSize);
	buffSize = 1000*sizeof(char);
	buff = malloc(buffSize);
	c = getChar();
	symbol->id = INIT; symbol->digitValue = -1; strnCpy(symbol->valueStr, "", 0);
	
	while((c == ' ' || c == '\n' || c == '\r' || c == '\t') && hasMoreChars()) {
		c = getChar();
	}
	if(isLetter(c)) {
		getIdentifier(identifier, c);
		if(strCmp(identifier, "if"     ) == 0) { symbol->id = IF;      strnCpy(symbol->valueStr, identifier, 2); }
		if(strCmp(identifier, "int"    ) == 0) { symbol->id = INT;     strnCpy(symbol->valueStr, identifier, 3); }
		if(strCmp(identifier, "char"   ) == 0) { symbol->id = CHAR;    strnCpy(symbol->valueStr, identifier, 4); }
		if(strCmp(identifier, "void"   ) == 0) { symbol->id = VOID;    strnCpy(symbol->valueStr, identifier, 4); }
		if(strCmp(identifier, "bool"   ) == 0) { symbol->id = BOOL;    strnCpy(symbol->valueStr, identifier, 4); }
		if(strCmp(identifier, "else"   ) == 0) { symbol->id = ELSE;    strnCpy(symbol->valueStr, identifier, 4); }
		if(strCmp(identifier, "open"   ) == 0) { symbol->id = OPEN;    strnCpy(symbol->valueStr, identifier, 4); }
		if(strCmp(identifier, "read"   ) == 0) { symbol->id = READ;    strnCpy(symbol->valueStr, identifier, 4); }
		if(strCmp(identifier, "write"  ) == 0) { symbol->id = WRITE;   strnCpy(symbol->valueStr, identifier, 5); }
		if(strCmp(identifier, "close"  ) == 0) { symbol->id = CLOSE;   strnCpy(symbol->valueStr, identifier, 5); }
		if(strCmp(identifier, "while"  ) == 0) { symbol->id = WHILE;   strnCpy(symbol->valueStr, identifier, 5); }
		if(strCmp(identifier, "return" ) == 0) { symbol->id = RETURN;  strnCpy(symbol->valueStr, identifier, 6); }
		if(strCmp(identifier, "sizeof" ) == 0) { symbol->id = SIZEOF;  strnCpy(symbol->valueStr, identifier, 6); }
		if(strCmp(identifier, "malloc" ) == 0) { symbol->id = MALLOC;  strnCpy(symbol->valueStr, identifier, 6); }
		if(strCmp(identifier, "struct" ) == 0) { symbol->id = STRUCT;  strnCpy(symbol->valueStr, identifier, 6); }
		if(strCmp(identifier, "printf" ) == 0) { symbol->id = PRINTF;  strnCpy(symbol->valueStr, identifier, 6); }
		if(strCmp(identifier, "typedef") == 0) { symbol->id = TYPEDEF; strnCpy(symbol->valueStr, identifier, 7); }
		if(symbol->uid == INIT) { symbol->id = IDENT; strnCpy(symbol->valueStr, identifier, 64); }
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
				while(nc != '"' && hasMoreTokens()) { 
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
					while((nc != '*' || nnc != '/') && hasMoreChars()) {
						nc = nnc;
						nnc = getChar();
					}
					symbol->id = COMMENT;
				}
				else {
					if(nc == '/') {
						nc = getChar();
						while(nc != '\n' && hasMoreChars()) { nc = getChar(); }
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
				if(nc == '|') { printf("SCANNER: or = %i\n", OR); symbol->id = OR; }
				else {
					symbol->id = ERROR;
					symbol->valueStr[0] = c; symbol->valueStr[1] = 0;
					ungetChar(nc);
				}
			}
			if(c == '#') {
				nc = getChar();
				strnCpy(buff, "include ", 8);
				i = 0;
				while(i < 8 && nc == buff[i]) {
					nc = getChar();
					i = i + 1;		
				}
				if(i == 8) {
					if(nc == '"') {
						nc = getChar();
						i = 0;
						buff[i] = nc;
						while(buff[i] != '\"' && i < 1000) {
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
